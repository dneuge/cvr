#include <string.h>

#include "global_macros.h"

#include "MatroskaEncoder.h"

MatroskaEncoder::MatroskaEncoder(const char *fileName) {
    EBMLTreeNode *node;
    
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
    
    node = new EBMLTreeNode(elementDefinitions->getElementDefinitionByName("ColourSpace"));
    node->copyBinaryContent((unsigned char[]) {0x36, 0x31, 0x56, 0x59}, 4); // YV16, which seems to be the planar equivalent of UYVY (which we get from the capture card); see http://fourcc.org/yuv.php
    videoSettingsNode->addChildNode(node);

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
}

void MatroskaEncoder::writeFileHeader() {
    rootNode->serialize();
    fwrite(rootNode->getBinaryContent(), rootNode->getBinarySize(), 1, fh);
}

void MatroskaEncoder::closeFile() {
    fflush(fh);
    fclose(fh);
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
