#include <string.h>
#include <fcntl.h>
#include <iostream>

#include "global_macros.h"

#include "MatroskaEncoder.h"

MatroskaEncoder::MatroskaEncoder(const char *fileName) {
    EBMLTreeNode *node;
    initialTimestamp = 0;
    fileOffsetClusterSize = 0;
    fileOffsetClusterPayload = 0;
    audioStreamTerminated = false;
    videoStreamTerminated = false;
    
    printf("starting new recording to file %s\n", fileName); // DEBUG
    
    // open file for output
    fh = fopen(fileName, "wb+");
    success = (ferror(fh) == 0);
    
    // get Matroska EBML definitions
    elementDefinitions = new MatroskaEBMLElementDefinitions();
    
    // start building EBML tree
    // NOTE: root node has no element/frame (only concatenated payload, thus "invisible")
    rootNode = new EBMLTreeNode();
    
    // EBML metadata
    EBMLTreeNode *ebmlNode = new EBMLTreeNode(elementDefinitions->getElementDefinitionByName("EBML"));
    rootNode->addChildNode(ebmlNode);
    
    node = new EBMLTreeNode(elementDefinitions->getElementDefinitionByName("EBMLVersion"));
    node->setIntegerContent(1);
    ebmlNode->addChildNode(node);
    
    node = new EBMLTreeNode(elementDefinitions->getElementDefinitionByName("EBMLReadVersion"));
    node->setIntegerContent(1);
    ebmlNode->addChildNode(node);
    
    node = new EBMLTreeNode(elementDefinitions->getElementDefinitionByName("EBMLMaxIDLength"));
    node->setIntegerContent(4);
    ebmlNode->addChildNode(node);
    
    node = new EBMLTreeNode(elementDefinitions->getElementDefinitionByName("EBMLMaxSizeLength"));
    node->setIntegerContent(8);
    ebmlNode->addChildNode(node);
    
    node = new EBMLTreeNode(elementDefinitions->getElementDefinitionByName("DocType"));
    node->copyString("matroska");
    ebmlNode->addChildNode(node);
    
    node = new EBMLTreeNode(elementDefinitions->getElementDefinitionByName("DocTypeVersion"));
    node->setIntegerContent(2);
    ebmlNode->addChildNode(node);
    
    node = new EBMLTreeNode(elementDefinitions->getElementDefinitionByName("DocTypeReadVersion"));
    node->setIntegerContent(2);
    ebmlNode->addChildNode(node);
    
    // start segment
    EBMLTreeNode *segmentNode = new EBMLTreeNode(elementDefinitions->getElementDefinitionByName("Segment"));
    rootNode->addChildNode(segmentNode);
    
    // meta seek information
    EBMLTreeNode *seekNode;
    
    EBMLTreeNode *seekHeadNode = new EBMLTreeNode(elementDefinitions->getElementDefinitionByName("SeekHead"));
    segmentNode->addChildNode(seekHeadNode);
    
    // -- seek information for segment information
    seekNode = new EBMLTreeNode(elementDefinitions->getElementDefinitionByName("Seek"));
    seekHeadNode->addChildNode(seekNode);
    
    node = new EBMLTreeNode(elementDefinitions->getElementDefinitionByName("SeekID"));
    node->copyBinaryContent((unsigned char[]) {0x15, 0x49, 0xA9, 0x66}, 4); // "KaxInfo"
    seekNode->addChildNode(node);
    
    EBMLTreeNode *seekPositionNodeForSegmentInfo = new EBMLTreeNode(elementDefinitions->getElementDefinitionByName("SeekPosition"));
    seekPositionNodeForSegmentInfo->setIntegerContent(0); // will be set later
    seekNode->addChildNode(seekPositionNodeForSegmentInfo);
    
    // -- seek information for track information
    seekNode = new EBMLTreeNode(elementDefinitions->getElementDefinitionByName("Seek"));
    seekHeadNode->addChildNode(seekNode);
    
    node = new EBMLTreeNode(elementDefinitions->getElementDefinitionByName("SeekID"));
    node->copyBinaryContent((unsigned char[]) {0x16, 0x54, 0xAE, 0x6B}, 4); // "KaxTracks"
    seekNode->addChildNode(node);
    
    EBMLTreeNode *seekPositionNodeForTrackInfo = new EBMLTreeNode(elementDefinitions->getElementDefinitionByName("SeekPosition"));
    seekPositionNodeForTrackInfo->setIntegerContent(0); // will be set later
    seekNode->addChildNode(seekPositionNodeForTrackInfo);
    
    // -- seek information for tags
    seekNode = new EBMLTreeNode(elementDefinitions->getElementDefinitionByName("Seek"));
    seekHeadNode->addChildNode(seekNode);
    
    node = new EBMLTreeNode(elementDefinitions->getElementDefinitionByName("SeekID"));
    node->copyBinaryContent((unsigned char[]) {0x12, 0x54, 0xC3, 0x67}, 4); // "KaxTags"
    seekNode->addChildNode(node);
    
    EBMLTreeNode *seekPositionNodeForTags = new EBMLTreeNode(elementDefinitions->getElementDefinitionByName("SeekPosition"));
    seekPositionNodeForTags->setIntegerContent(0); // will be set later
    seekNode->addChildNode(seekPositionNodeForTags);
    
    // -- seek information for cue information
    seekNode = new EBMLTreeNode(elementDefinitions->getElementDefinitionByName("Seek"));
    seekHeadNode->addChildNode(seekNode);
    
    node = new EBMLTreeNode(elementDefinitions->getElementDefinitionByName("SeekID"));
    node->copyBinaryContent((unsigned char[]) {0x1C, 0x53, 0xBB, 0x6B}, 4); // "KaxCues"
    seekNode->addChildNode(node);
    
    EBMLTreeNode *seekPositionNodeForCuePoints = new EBMLTreeNode(elementDefinitions->getElementDefinitionByName("SeekPosition"));
    seekPositionNodeForCuePoints->setIntegerContent(0); // will be set later
    seekNode->addChildNode(seekPositionNodeForCuePoints);
    
    // segment information
    EBMLTreeNode *segmentInformationNode = new EBMLTreeNode(elementDefinitions->getElementDefinitionByName("Info"));
    segmentNode->addChildNode(segmentInformationNode);
    
    node = new EBMLTreeNode(elementDefinitions->getElementDefinitionByName("TimecodeScale"));
    node->setIntegerContent(1000000); // timecodes are given in units of 1 ms (1,000,000 ns)
    segmentInformationNode->addChildNode(node);
    
    node = new EBMLTreeNode(elementDefinitions->getElementDefinitionByName("MuxingApp"));
    node->copyString(APP_NAME_VERSION_STRING);
    segmentInformationNode->addChildNode(node);
    
    node = new EBMLTreeNode(elementDefinitions->getElementDefinitionByName("WritingApp"));
    node->copyString(APP_NAME_VERSION_STRING);
    segmentInformationNode->addChildNode(node);
    
    // track information
    EBMLTreeNode *trackEntryNode;
    EBMLTreeNode *tracksNode = new EBMLTreeNode(elementDefinitions->getElementDefinitionByName("Tracks"));
    segmentNode->addChildNode(tracksNode);
    
    // -- video track
    trackEntryNode = new EBMLTreeNode(elementDefinitions->getElementDefinitionByName("TrackEntry"));
    tracksNode->addChildNode(trackEntryNode);
    
    node = new EBMLTreeNode(elementDefinitions->getElementDefinitionByName("TrackNumber"));
    node->setIntegerContent(TRACK_NUMBER_VIDEO);
    trackEntryNode->addChildNode(node);
    
    node = new EBMLTreeNode(elementDefinitions->getElementDefinitionByName("TrackUID"));
    node->setBinaryContent(generateUniqueRandomID(), EBML_FRAMED_ID_LENGTH);
    trackEntryNode->addChildNode(node);
    
    node = new EBMLTreeNode(elementDefinitions->getElementDefinitionByName("TrackType"));
    node->setIntegerContent(MATROSKA_TRACK_TYPE_VIDEO);
    trackEntryNode->addChildNode(node);
    
    node = new EBMLTreeNode(elementDefinitions->getElementDefinitionByName("FlagEnabled"));
    node->setIntegerContent(1); // track should be available
    trackEntryNode->addChildNode(node);
    
    node = new EBMLTreeNode(elementDefinitions->getElementDefinitionByName("FlagDefault"));
    node->setIntegerContent(1); // track should be shown by default
    trackEntryNode->addChildNode(node);
    
    node = new EBMLTreeNode(elementDefinitions->getElementDefinitionByName("FlagForced"));
    node->setIntegerContent(0); // don't force the track to be active
    trackEntryNode->addChildNode(node);
    
    node = new EBMLTreeNode(elementDefinitions->getElementDefinitionByName("FlagLacing"));
    node->setIntegerContent(0); // don't use lacing
    trackEntryNode->addChildNode(node);
    
    node = new EBMLTreeNode(elementDefinitions->getElementDefinitionByName("MinCache"));
    node->setIntegerContent(0); // no cache required
    trackEntryNode->addChildNode(node);
    
    node = new EBMLTreeNode(elementDefinitions->getElementDefinitionByName("MaxBlockAdditionID"));
    node->setIntegerContent(0); // don't use block additions
    trackEntryNode->addChildNode(node);
    
    node = new EBMLTreeNode(elementDefinitions->getElementDefinitionByName("CodecID"));
    node->copyString("V_MJPEG"); // NOTE: V_MJPEG is not listed to be officially supported yet but it's already being used by FFMPEG which is sufficient
    trackEntryNode->addChildNode(node);
    
    node = new EBMLTreeNode(elementDefinitions->getElementDefinitionByName("CodecName"));
    node->copyString("Motion JPEG"); // not required but in case decoding fails, this may help identifying the issue
    trackEntryNode->addChildNode(node);
    
    node = new EBMLTreeNode(elementDefinitions->getElementDefinitionByName("CodecDecodeAll"));
    node->setIntegerContent(1); // tells players that the codec is able to handle damaged frames; IMHO actually depends on implementation but unfortunately this has to be hardcoded
    trackEntryNode->addChildNode(node);
    
    /* this is Matroska v4
    node = new EBMLTreeNode(elementDefinitions->getElementDefinitionByName("SeekPreRoll"));
    node->setIntegerContent(0); // I guess no pre-rolling is necessary as we encode full frames, no differential
    trackEntryNode->addChildNode(node);
    */
    
    EBMLTreeNode *videoSettingsNode = new EBMLTreeNode(elementDefinitions->getElementDefinitionByName("Video"));
    trackEntryNode->addChildNode(videoSettingsNode);
    
    node = new EBMLTreeNode(elementDefinitions->getElementDefinitionByName("FlagInterlaced"));
    node->setIntegerContent(0); // video is not interlaced
    videoSettingsNode->addChildNode(node);
    
    // NOTE: frame dimensions may be wrong due to YUV encoding with different sub-sampling per component channel
    node = new EBMLTreeNode(elementDefinitions->getElementDefinitionByName("PixelWidth"));
    node->setIntegerContent(1280); // TODO: don't hardcode
    videoSettingsNode->addChildNode(node);
    
    node = new EBMLTreeNode(elementDefinitions->getElementDefinitionByName("PixelHeight"));
    node->setIntegerContent(720); // TODO: don't hardcode
    videoSettingsNode->addChildNode(node);
    
    /*
    // specifying any colour space will cause decoders to fail
    node = new EBMLTreeNode(elementDefinitions->getElementDefinitionByName("ColourSpace"));
    //node->copyBinaryContent((unsigned char[]) {0x36, 0x31, 0x56, 0x59}, 4); // YV16, which seems to be the planar equivalent of UYVY (which we get from the capture card); see http://fourcc.org/yuv.php
    //node->copyBinaryContent((unsigned char[]) {0x59, 0x56, 0x59, 0x55}, 4); // UYVY (written in reverse: YVYU)
    //node->copyBinaryContent((unsigned char[]) {0x55, 0x59, 0x56, 0x59}, 4); // UYVY
    //node->copyBinaryContent((unsigned char[]) {0x00, 0x00, 0x00, 0x00}, 4); // RGB (at least according to fourcc.org)
    //node->copyBinaryContent((unsigned char[]) {0x32, 0x55, 0x59, 0x49}, 4); // IYU2
    //node->copyBinaryContent((unsigned char[]) {0x49, 0x59, 0x55, 0x32}, 4); // IYU2 (in reverse reverse)
    videoSettingsNode->addChildNode(node);
    */
    
    // -- audio track
    trackEntryNode = new EBMLTreeNode(elementDefinitions->getElementDefinitionByName("TrackEntry"));
    tracksNode->addChildNode(trackEntryNode);
    
    node = new EBMLTreeNode(elementDefinitions->getElementDefinitionByName("TrackNumber"));
    node->setIntegerContent(TRACK_NUMBER_AUDIO);
    trackEntryNode->addChildNode(node);
    
    node = new EBMLTreeNode(elementDefinitions->getElementDefinitionByName("TrackUID"));
    node->setBinaryContent(generateUniqueRandomID(), EBML_FRAMED_ID_LENGTH);
    trackEntryNode->addChildNode(node);
    
    node = new EBMLTreeNode(elementDefinitions->getElementDefinitionByName("TrackType"));
    node->setIntegerContent(MATROSKA_TRACK_TYPE_AUDIO);
    trackEntryNode->addChildNode(node);
    
    node = new EBMLTreeNode(elementDefinitions->getElementDefinitionByName("FlagEnabled"));
    node->setIntegerContent(1); // track should be available
    trackEntryNode->addChildNode(node);
    
    node = new EBMLTreeNode(elementDefinitions->getElementDefinitionByName("FlagDefault"));
    node->setIntegerContent(1); // track should be shown by default
    trackEntryNode->addChildNode(node);
    
    node = new EBMLTreeNode(elementDefinitions->getElementDefinitionByName("FlagForced"));
    node->setIntegerContent(0); // don't force the track to be active
    trackEntryNode->addChildNode(node);
    
    node = new EBMLTreeNode(elementDefinitions->getElementDefinitionByName("FlagLacing"));
    node->setIntegerContent(0); // don't use lacing
    trackEntryNode->addChildNode(node);
    
    node = new EBMLTreeNode(elementDefinitions->getElementDefinitionByName("MinCache"));
    node->setIntegerContent(0); // no cache required
    trackEntryNode->addChildNode(node);
    
    node = new EBMLTreeNode(elementDefinitions->getElementDefinitionByName("MaxBlockAdditionID"));
    node->setIntegerContent(0); // don't use block additions
    trackEntryNode->addChildNode(node);
    
    node = new EBMLTreeNode(elementDefinitions->getElementDefinitionByName("CodecID"));
    node->copyString("A_PCM/INT/LIT"); // TODO: don't hardcode; PCM Little Endian
    trackEntryNode->addChildNode(node);
    
    node = new EBMLTreeNode(elementDefinitions->getElementDefinitionByName("CodecName"));
    node->copyString("PCM Little Endian"); // not required but in case decoding fails, this may help identifying the issue
    trackEntryNode->addChildNode(node);
    
    node = new EBMLTreeNode(elementDefinitions->getElementDefinitionByName("CodecDecodeAll"));
    node->setIntegerContent(1); // tells players that the codec is able to handle damaged frames; should be the case for uncompressed raw audio files
    trackEntryNode->addChildNode(node);
    
    /* this is Matroska v4
    node = new EBMLTreeNode(elementDefinitions->getElementDefinitionByName("SeekPreRoll"));
    node->setIntegerContent(0); // uncompressed audio surely doesn't need pre-rolling?
    trackEntryNode->addChildNode(node);
    */
    
    EBMLTreeNode *audioSettingsNode = new EBMLTreeNode(elementDefinitions->getElementDefinitionByName("Audio"));
    trackEntryNode->addChildNode(audioSettingsNode);
    
    node = new EBMLTreeNode(elementDefinitions->getElementDefinitionByName("SamplingFrequency"));
    node->setFloatContent(48000); // TODO: don't hardcode
    audioSettingsNode->addChildNode(node);
    
    node = new EBMLTreeNode(elementDefinitions->getElementDefinitionByName("Channels"));
    node->setIntegerContent(2); // TODO: don't hardcode
    audioSettingsNode->addChildNode(node);
    
    node = new EBMLTreeNode(elementDefinitions->getElementDefinitionByName("BitDepth"));
    node->setIntegerContent(16); // TODO: don't hardcode
    audioSettingsNode->addChildNode(node);
    
    // tags
    EBMLTreeNode *tagsNode = new EBMLTreeNode(elementDefinitions->getElementDefinitionByName("Tags"));
    segmentNode->addChildNode(tagsNode);
    
    EBMLTreeNode *tagNode = new EBMLTreeNode(elementDefinitions->getElementDefinitionByName("Tag"));
    tagsNode->addChildNode(tagNode);
    
    EBMLTreeNode *targetsNode = new EBMLTreeNode(elementDefinitions->getElementDefinitionByName("Targets"));
    tagNode->addChildNode(targetsNode);
    
    // -- encoder
    EBMLTreeNode *simpleTagNode = new EBMLTreeNode(elementDefinitions->getElementDefinitionByName("SimpleTag"));
    tagNode->addChildNode(simpleTagNode);
    
    node = new EBMLTreeNode(elementDefinitions->getElementDefinitionByName("TagName"));
    node->copyString("ENCODER");
    simpleTagNode->addChildNode(node);
    
    node = new EBMLTreeNode(elementDefinitions->getElementDefinitionByName("TagLanguage"));
    node->copyString("und");
    simpleTagNode->addChildNode(node);
    
    node = new EBMLTreeNode(elementDefinitions->getElementDefinitionByName("TagDefault"));
    node->setIntegerContent(1);
    simpleTagNode->addChildNode(node);
    
    node = new EBMLTreeNode(elementDefinitions->getElementDefinitionByName("TagString"));
    node->copyString(APP_NAME_VERSION_STRING);
    simpleTagNode->addChildNode(node);
    
    // -- title
    simpleTagNode = new EBMLTreeNode(elementDefinitions->getElementDefinitionByName("SimpleTag"));
    tagNode->addChildNode(simpleTagNode);
    
    node = new EBMLTreeNode(elementDefinitions->getElementDefinitionByName("TagName"));
    node->copyString("TITLE");
    simpleTagNode->addChildNode(node);
    
    node = new EBMLTreeNode(elementDefinitions->getElementDefinitionByName("TagLanguage"));
    node->copyString("und");
    simpleTagNode->addChildNode(node);
    
    node = new EBMLTreeNode(elementDefinitions->getElementDefinitionByName("TagDefault"));
    node->setIntegerContent(1);
    simpleTagNode->addChildNode(node);
    
    node = new EBMLTreeNode(elementDefinitions->getElementDefinitionByName("TagString"));
    node->copyString("test");
    simpleTagNode->addChildNode(node);
    
    
    // update reference pointers
    // We only have absolute byte offsets after serialization, so we need to
    // serialize once, calculate all offsets and can then update pointers in
    // tree structure. Note that we will need a second serialization to get
    // those updated values to show up on output!
    // Also note that we need to set offsets relative to the segment.
    rootNode->serialize();
    rootNode->updateOffsets(0);
    seekPositionNodeForSegmentInfo->setIntegerContent(segmentInformationNode->getRelativeOffset(segmentNode));
    seekPositionNodeForTrackInfo->setIntegerContent(tracksNode->getRelativeOffset(segmentNode));
    seekPositionNodeForTags->setIntegerContent(tagsNode->getRelativeOffset(segmentNode));
    
    // cue points are defined later, so we don't know the offset yet and need to
    // patch it later; remember offset
    fileOffsetCueReference = seekPositionNodeForCuePoints->getAbsolutePayloadOffset();
    
    // same for the total segment size (encloses clusters)
    fileOffsetSegmentSize = segmentNode->getAbsoluteDataSizeOffset();
    
    // needed for segment size calculation
    fileOffsetSegmentPayload = segmentNode->getAbsolutePayloadOffset();
}

bool MatroskaEncoder::isSuccess() {
    return success;
}

void MatroskaEncoder::writeFileHeader() {
    printf("writing file header\n"); // DEBUG
    
    // serialize once more to get the latest data
    // (required after reference offsets have been set)
    rootNode->serialize();
    
    // write to disk
    fwrite(rootNode->getBinaryContent(), rootNode->getBinarySize(), 1, fh);
    
    // tree is no longer needed, we can dispose it now
    delete rootNode;
    rootNode = 0;
}

void MatroskaEncoder::closeFile() {
    printf("finalizing recorded file\n"); // DEBUG
    
    unsigned long long offsetAfterLastCluster = ftell(fh);
    
    // close any open cluster
    finalizeCluster(offsetAfterLastCluster);
    
    // build tree for cue points
    EBMLTreeNode *cuesNode = new EBMLTreeNode(elementDefinitions->getElementDefinitionByName("Cues"));
    
    EBMLTreeNode *node;
    std::vector<MatroskaCuePoint*>::iterator it = cuePoints.begin();
    while (it != cuePoints.end()) {
        MatroskaCuePoint *cuePoint = *it;
        
        EBMLTreeNode *cuePointNode = new EBMLTreeNode(elementDefinitions->getElementDefinitionByName("CuePoint"));
        cuesNode->addChildNode(cuePointNode);
        
        node = new EBMLTreeNode(elementDefinitions->getElementDefinitionByName("CueTime"));
        node->setIntegerContent(cuePoint->timeMillis);
        cuePointNode->addChildNode(node);
        
        if (cuePoint->clusterOffsetVideoTrack != 0) {
            EBMLTreeNode *cueTrackPositionsNode = new EBMLTreeNode(elementDefinitions->getElementDefinitionByName("CueTrackPositions"));
            cuePointNode->addChildNode(cueTrackPositionsNode);
            
            node = new EBMLTreeNode(elementDefinitions->getElementDefinitionByName("CueTrack"));
            node->setIntegerContent(MATROSKA_TRACK_TYPE_VIDEO);
            cueTrackPositionsNode->addChildNode(node);
            
            node = new EBMLTreeNode(elementDefinitions->getElementDefinitionByName("CueClusterPosition"));
            node->setIntegerContent(cuePoint->clusterOffsetVideoTrack - fileOffsetSegmentPayload);
            cueTrackPositionsNode->addChildNode(node);
        }
        
        if (cuePoint->clusterOffsetAudioTrack != 0) {
            EBMLTreeNode *cueTrackPositionsNode = new EBMLTreeNode(elementDefinitions->getElementDefinitionByName("CueTrackPositions"));
            cuePointNode->addChildNode(cueTrackPositionsNode);
            
            node = new EBMLTreeNode(elementDefinitions->getElementDefinitionByName("CueTrack"));
            node->setIntegerContent(MATROSKA_TRACK_TYPE_AUDIO);
            cueTrackPositionsNode->addChildNode(node);
            
            node = new EBMLTreeNode(elementDefinitions->getElementDefinitionByName("CueClusterPosition"));
            node->setIntegerContent(cuePoint->clusterOffsetAudioTrack - fileOffsetSegmentPayload);
            cueTrackPositionsNode->addChildNode(node);
        }
        
        // cue point is no longer needed, free memory
        delete cuePoint;
        
        it++;
    }
    
    // write cue points to disk
    cuesNode->serialize();
    fwrite(cuesNode->getOuterContent(), cuesNode->getOuterSize(), 1, fh);
    
    // remember total file size, file won't grow any more
    unsigned long long totalFileSize = ftell(fh);
    
    // patch references and sizes
    
    // -- segment size
    // patch data size attribute of Segment node with actual payload size
    unsigned long long segmentSize = totalFileSize - fileOffsetSegmentPayload;
    unsigned char *segmentDataSize = EBMLTreeNode::encodeDataSize(segmentSize);
    
    fseek(fh, fileOffsetSegmentSize, SEEK_SET);
    fwrite(segmentDataSize, EBML_DATA_SIZE, 1, fh);
    
    delete segmentDataSize;
    
    // -- cue points reference
    // patch cue point reference to point to actual Cues node
    unsigned long long relativeOffset = offsetAfterLastCluster - fileOffsetSegmentPayload;
    unsigned char len = sizeof(relativeOffset);
    unsigned char *out = new unsigned char[len];
    memcpy(out, (unsigned char*) &relativeOffset, len);
    EBMLTreeNode::changeEndianness(out, len);
    
    fseek(fh, fileOffsetCueReference, SEEK_SET);
    fwrite(out, len, 1, fh);
    
    delete out;
    
    // flush to disk and close handle
    fflush(fh);
    fclose(fh);
    
    printf("recording finished\n"); // DEBUG
}

bool MatroskaEncoder::checkIDAlreadyUsed(unsigned char *checkID) {
    bool found = false;
    
    std::vector<unsigned char*>::iterator it = usedIDs.begin();
    while (it != usedIDs.end()) {
        found = (memcmp(*it, checkID, EBML_FRAMED_ID_LENGTH) == 0);
        
        if (found) {
            break;
        }
        
        it++;
    }
    
    return found;
}

unsigned char* MatroskaEncoder::generateUniqueRandomID() {
    unsigned char *id = 0;
    
    // generate a unique ID
    do {
        // free previous ID
        if (id != 0) {
            delete id;
        }
        
        // get new random ID
        id = EBMLTreeNode::createRandomID();
    } while (checkIDAlreadyUsed(id));
    
    // record ID as used
    unsigned char *copy = new unsigned char[EBML_FRAMED_ID_LENGTH];
    memcpy(copy, id, EBML_FRAMED_ID_LENGTH);
    usedIDs.push_back(copy);
    
    return id;
}

void MatroskaEncoder::finalizeCluster(unsigned long long fileOffsetBefore) {
    if (fileOffsetClusterSize != 0) {
        // patch cluster size
        unsigned long long clusterSize = fileOffsetBefore - fileOffsetClusterPayload;
        unsigned char *out = EBMLTreeNode::encodeDataSize(clusterSize);
        fseek(fh, fileOffsetClusterSize, SEEK_SET);
        fwrite(out, EBML_DATA_SIZE, 1, fh);
        delete out;
        
        // return to end of file
        fseek(fh, fileOffsetBefore, SEEK_SET);
    }
}

void MatroskaEncoder::checkAndHandleEndOfRecording() {
    if (audioStreamTerminated && videoStreamTerminated) {
        printf("end of recording\n"); // DEBUG
        closeFile();
    }
}

void MatroskaEncoder::freePacket(TimedPacket *timedPacket) {
    if ((timedPacket->data != 0) && (timedPacket->dataLength > 0)) {
        delete timedPacket->data;
    }
    
    delete timedPacket;
}

void MatroskaEncoder::consumeAudioPacket(TimedPacket *timedPacket) {
    addAudioPacket(timedPacket);
    freePacket(timedPacket);
}

void MatroskaEncoder::consumeVideoFrame(TimedPacket *timedPacket) {
    addVideoFrame(timedPacket);
    freePacket(timedPacket);
}

void MatroskaEncoder::addAudioPacket(TimedPacket* timedPacket) {
    //printf("got audio packet %lld of size %lld\n", timedPacket->index, timedPacket->dataLength); // DEBUG
    
    // check for terminating packet
    if (timedPacket->dataLength == 0) {
        audioStreamTerminated = true;
        checkAndHandleEndOfRecording();
    }
    
    // discard any packets after end of recording
    // NOTE: caller must free after use
    if (audioStreamTerminated) {
        return;
    }
}

void MatroskaEncoder::addVideoFrame(TimedPacket* timedPacket) {
    //printf("got video packet %lld of size %lld\n", timedPacket->index, timedPacket->dataLength); // DEBUG
    
    // check for terminating packet
    if (timedPacket->dataLength == 0) {
        videoStreamTerminated = true;
        checkAndHandleEndOfRecording();
    }
    
    // discard any frames after end of recording
    // NOTE: caller must free after use
    if (videoStreamTerminated) {
        return;
    }
    
    // remember timestamp if this frame start the recording
    if (initialTimestamp == 0) {
        initialTimestamp = timedPacket->timestampMillis;
    }
    
    // calculate timestamp relative to start of recording
    unsigned long long relativeTimestampRecording = timedPacket->timestampMillis - initialTimestamp;
    
    // remember file position
    unsigned long long fileOffsetBefore = ftell(fh);
    
    // each video frame starts a new cluster
    
    // -- end old cluster
    finalizeCluster(fileOffsetBefore);
        
    // -- start new cluster
    EBMLTreeNode *node;
    EBMLTreeNode *clusterNode = new EBMLTreeNode(elementDefinitions->getElementDefinitionByName("Cluster"));
    
    node = new EBMLTreeNode(elementDefinitions->getElementDefinitionByName("Timecode"));
    node->setIntegerContent(relativeTimestampRecording);
    clusterNode->addChildNode(node);
    
    clusterNode->serialize();
    clusterNode->updateOffsets(fileOffsetBefore);
    
    fileOffsetCluster = clusterNode->getAbsoluteOffset();
    fileOffsetClusterPayload = clusterNode->getAbsolutePayloadOffset();
    fileOffsetClusterSize = clusterNode->getAbsoluteDataSizeOffset();
    
    fwrite(clusterNode->getOuterContent(), clusterNode->getOuterSize(), 1, fh);
    
    // add video frame
    // SimpleBlock needs prefixed header
    unsigned long long len = timedPacket->dataLength + 6;
    unsigned char *out = new unsigned char[len];
    memcpy(out + 6, timedPacket->data, timedPacket->dataLength);
    
    // see: http://www.matroska.org/technical/specs/index.html#simpleblock_structure
    out[0] = 0b10000001; // 0x01 for video track number, identified as 8 bit (encoded like EBML data size etc.), thus prefixed MSB 1
    out[1] = 0x00; // timecode upper byte; timecode is 0 because it matches the cluster timecode
    out[2] = 0x00; // timecode lower byte
    out[3] = 0b10000001; // keyframe, not invisible, no lacing, discardable
    out[4] = 0x00; // no frames in lace
    out[5] = 0x00; // lace size (0 because we use none)
    
    node = new EBMLTreeNode(elementDefinitions->getElementDefinitionByName("SimpleBlock"));
    node->setBinaryContent(out, len);
    node->serialize();
    fwrite(node->getOuterContent(), node->getOuterSize(), 1, fh);
    
    // add cue point
    cuePoints.push_back(new MatroskaCuePoint(relativeTimestampRecording, fileOffsetCluster, 0));
    
    // free memory
    delete node; // also deletes out
    delete clusterNode;
}