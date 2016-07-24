# Encoding

Captured content can be encoded to Matroska container format (.mkv) containing Motion JPEG with raw PCM audio. This is done in a workflow described in more detail below. MJPEG was chosen as it is very easy and fast to encode: Every MJPEG frame is just one complete JPEG file with no need for incremental frames as in higher compressed codecs. Thus, MJPEG isn't very efficient storage-wise but even older computers should be able to reliably encode full 60 fps in real-time at a sufficiently high quality. Recording this way is only meant for intermediary use. For permanent storage or distribution, a much more time-consuming but space-efficient codec and a more widely available container format may be transcoded to. The generated files are supposed to be compatible with at least FFMPEG, so it should be easy to convert them to a more convenient format after recording has finished.

The Matroska container format (DocType version 2) was chosen because it has been openly specified and it is capable of storing MJPEG at variable frame rate with uncompressed audio. Using the less complex Ogg container format was shortly evaluated but MJPEG and raw audio appeared to be unsupported because Ogg delegates storage of parameters for decoding to the streams which neither raw PCM audio nor MJPEG can do. As Matroska fails to provide any documentation for the official API and other libraries appear not to do either, encoding has been implemented from scratch in CVR by following the [specifications](http://www.matroska.org/technical/specs/index.html). To ease encoding, CVR always uses the maximum allowed value lengths for UIDs (4 bytes / 28 bit payload), data sizes (8 bytes / 56 bit payload) and values (64 bit numbers). This causes decoding to currently fail on some VLC versions (upstream issue related to compilation). Blocks are always encoded as SimpleBlocks. Currently, a Little Endian architecture (x86, ...) is assumed when encoding (values are always byte-swapped), so encoding on BE architectures (PPC, ...) will currently fail.

## Stages of processing

!cvr-queue.png!

First, raw video frames (with each nth frame discarded if frame rate reduction is active) and audio packets are assigned the timecode they were received from the capture SDK (accurate to milliseconds) and an incrementing index number. They are then cached in two separate ring buffers (one for audio and one for video). While audio packets can be muxed directly later on, video frames need to be compressed early, even for timeshifted recordings, since raw input of 720p60 video requires approximately 100MB/s to store which obviously is too much to keep around for a longer period of time. If encoding is too slow, this will be the most likely source of frame drops taking place. A configurable amount of worker threads fetches those raw images, converts color space from UYVY to RGB and compresses the resulting image to JPEG. Those results are then added ordered by the index number to a separate ring buffer for muxing. 

### Feeding the mux process for output

The muxing process accepts time coded audio and video packets which have to be supplied in the correct order. Thus, a "Mux Feeder" is used to pick data from both result buffers (labelled "Audio Queue" and "Encoded Frame Queue" above) and forward them for eased muxing. Selection of packets is done by the following rules:

1. mux feeder has to be enabled (see below)
2. the earlier timecode is used first, video frames preceed audio packets on same timecodes (decision of whether feeding an audio packet or a video frame)
3. timecodes must be consecutive per type (audio/video); if timecodes jump back, the frame/packet must be discarded
4. if index numbers indicate a missing video frame (higher than expected index), delay forwarding until additional n frames have arrived (since JPEG encoding runs multi-threaded, some earlier frames may arrive later in the result buffer; waiting for n more frames reduces risk of dropping frames because they did not encode in time)
5. if index numbers indicate a video frame has already been dropped (lower than expected index), discard frame (too late to be muxed)

When recording is not running (and timeshift is disabled), captured packets/frames will not be inserted to any ring buffers.

When timeshift is enabled but recording is not requested yet, captured packets/frames will be inserted to A/V ring buffers and JPEG encoding will take place. However, no mux will be setup and the mux feeder will remain suspended which allows to fill both result ring buffers up to the desired amount of packets/frames to be kept in advance. When a timeshifted recording is to be written to disk, a new mux process is configured, started and immediately fed with the accumulated backlog.

When a regular recording (without timeshift) is started, a mux process is configured and the feeder and input are being enabled.

When any type of recording is to be stopped, a null video frame and audio packet will be inserted to both queues as a stop marker. If timeshift is disabled, input queues will no longer be filled. If timeshift is enabled, new frames/packets will continue to be dropped into the input queues. As null frames/packets are supposed to never be inserted to the result queues, mux feeder and mux process will recognize null packets as an instruction to disable feeding and close/finish the previously recorded file.

## Matroska encoder (mux)

### Matroska in a nutshell

The Matroska file layout is actually a dual tree structure, nested like XML but flattened to the scheme <element code sequence><payload length><payload data> with child nodes of the same layout forming the payload of their parent. That format is called EBML. Sibling nodes are simply concatenated, so to read or skip an element, a parser needs to know all element code sequences so it can decode the payload length (whose length is encoded with a leading specified bit pattern). The first tree contains just EBML metadata which specifies how to read the file. The second tree contains the actual Matroska-specific EBML data as a so-called "segment" element. Those two trees do not have a (visible) common parent node. The Matroska tree contains metadata valid for the segment it describes. This includes generic segment information, parameters and descriptions for decoding tracks, tags (like ID3), cue points (frame/packet index), offsets of where to find all those essential parts in the segment ("meta seek information") and, finally, clusters. Clusters contain the actual streams, encapsulated in blocks. Every cluster contains an absolute timecode which is the reference time for its blocks, which use relative time offsets to their cluster. Each block is the container for just one packet for a given track. Blocks for multiple tracks of multiple arbitrary consecutive timecodes (allowing variable frame rates) after the cluster's one can be inserted into one common cluster. Clusters should not grow too large as they may be read in whole by a parser/player, thus it is practical to either watch their size and duration or to simply start a new one for each new video frame.

### Our implementation

CVR's Matroska encoder's constructor takes the filename/path of the output file. It internally builds an EBML and EBML Matroska document tree (with an invisible root tree to contain both trees as described above) up to the point where the so-called clusters are to be inserted. A method we call `serialize` is being used to compile the tree into its binary form. Then, `updateOffsets` is being used to calculate absolute byte offsets for all tree nodes, so that we can insert the reference offsets to the meta seek information elements and to remember byte offsets we need to patch when finalizing the file. This is required because the segment size grows as we add more clusters and blocks and the cue points (index) can only be written after the recording is complete, so everything is already on disk. `writeFileHeader` brings the complete "header" to disk. Afterwards, we are ready to store video frames and audio packets.

Video frames are stored via `addVideoFrame` which ends the previous cluster and starts a new one. Because we want to get all data out to disk as fast as possible, we need to patch the EBML data size attribute that's already on disk when closing a cluster, which happens using `finalizeCluster`. Since there are no clusters before the first frame, audio packets via `addAudioFrame` can currently only be added after the first video frame has been written. All earlier audio packets are discarded by the muxer. Whenever the first video frame or audio packet of a cluster is being written, a cue point is being prepared by adding a `MatroskaCuePoint` to the private vector `cuePoints`.

Calling `closeFile` finalizes the container by serializing all `cuePoints` to an EBML Matroska tree structure. Segment size and cue seek information are patched with the actual values, the file is flushed to disk and closed.


### Generalization

The encoder is prepared to be generalized through an interface later on, if need arises to support another container format. This is done by accessing only generic public methods that can easily be moved to an interface class, such as a constructor that opens a new file, methods to write the file header (and prepare for payload writing), add frames and audio packets and finally finish/close the file.


## Documentation TODO

* illustration: reflect introduction of index numbers when feeder fetches video frames
