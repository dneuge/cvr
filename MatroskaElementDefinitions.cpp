#include <iostream>

#include "MatroskaEBMLElementDefinitions.h"

/**
 * Constructor defines all Matroska EBML element definitions.
 */
MatroskaEBMLElementDefinitions::MatroskaEBMLElementDefinitions() {
    elementDefinitions = std::map<std::string, EBMLElement*>();
    
    // NOTE: All definitions have been automatically extracted from the Matroska
    //       website using the included script tools/generate_ebml_dict_code.py
    //       Do not manipulate below code manually!
    
    // code automatically generated from http://www.matroska.org/technical/specs/index.html
    // date of retrieval: 24 Dec 2013

    // EBML Header
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("EBML", new EBMLElement(4, (const unsigned char[]) {0x1A, 0x45, 0xDF, 0xA3})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("EBMLVersion", new EBMLElement(2, (const unsigned char[]) {0x42, 0x86})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("EBMLReadVersion", new EBMLElement(2, (const unsigned char[]) {0x42, 0xF7})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("EBMLMaxIDLength", new EBMLElement(2, (const unsigned char[]) {0x42, 0xF2})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("EBMLMaxSizeLength", new EBMLElement(2, (const unsigned char[]) {0x42, 0xF3})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("DocType", new EBMLElement(2, (const unsigned char[]) {0x42, 0x82})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("DocTypeVersion", new EBMLElement(2, (const unsigned char[]) {0x42, 0x87})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("DocTypeReadVersion", new EBMLElement(2, (const unsigned char[]) {0x42, 0x85})));

    // Global elements (used everywhere in the format)
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("Void", new EBMLElement(1, (const unsigned char[]) {0xEC})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("CRC-32", new EBMLElement(1, (const unsigned char[]) {0xBF})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("SignatureSlot", new EBMLElement(4, (const unsigned char[]) {0x1B, 0x53, 0x86, 0x67})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("SignatureAlgo", new EBMLElement(2, (const unsigned char[]) {0x7E, 0x8A})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("SignatureHash", new EBMLElement(2, (const unsigned char[]) {0x7E, 0x9A})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("SignaturePublicKey", new EBMLElement(2, (const unsigned char[]) {0x7E, 0xA5})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("Signature", new EBMLElement(2, (const unsigned char[]) {0x7E, 0xB5})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("SignatureElements", new EBMLElement(2, (const unsigned char[]) {0x7E, 0x5B})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("SignatureElementList", new EBMLElement(2, (const unsigned char[]) {0x7E, 0x7B})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("SignedElement", new EBMLElement(2, (const unsigned char[]) {0x65, 0x32})));

    // Segment
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("Segment", new EBMLElement(4, (const unsigned char[]) {0x18, 0x53, 0x80, 0x67})));

    // Meta Seek Information
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("SeekHead", new EBMLElement(4, (const unsigned char[]) {0x11, 0x4D, 0x9B, 0x74})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("Seek", new EBMLElement(2, (const unsigned char[]) {0x4D, 0xBB})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("SeekID", new EBMLElement(2, (const unsigned char[]) {0x53, 0xAB})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("SeekPosition", new EBMLElement(2, (const unsigned char[]) {0x53, 0xAC})));

    // Segment Information
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("Info", new EBMLElement(4, (const unsigned char[]) {0x15, 0x49, 0xA9, 0x66})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("SegmentUID", new EBMLElement(2, (const unsigned char[]) {0x73, 0xA4})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("SegmentFilename", new EBMLElement(2, (const unsigned char[]) {0x73, 0x84})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("PrevUID", new EBMLElement(3, (const unsigned char[]) {0x3C, 0xB9, 0x23})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("PrevFilename", new EBMLElement(3, (const unsigned char[]) {0x3C, 0x83, 0xAB})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("NextUID", new EBMLElement(3, (const unsigned char[]) {0x3E, 0xB9, 0x23})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("NextFilename", new EBMLElement(3, (const unsigned char[]) {0x3E, 0x83, 0xBB})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("SegmentFamily", new EBMLElement(2, (const unsigned char[]) {0x44, 0x44})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("ChapterTranslate", new EBMLElement(2, (const unsigned char[]) {0x69, 0x24})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("ChapterTranslateEditionUID", new EBMLElement(2, (const unsigned char[]) {0x69, 0xFC})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("ChapterTranslateCodec", new EBMLElement(2, (const unsigned char[]) {0x69, 0xBF})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("ChapterTranslateID", new EBMLElement(2, (const unsigned char[]) {0x69, 0xA5})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("TimecodeScale", new EBMLElement(3, (const unsigned char[]) {0x2A, 0xD7, 0xB1})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("Duration", new EBMLElement(2, (const unsigned char[]) {0x44, 0x89})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("DateUTC", new EBMLElement(2, (const unsigned char[]) {0x44, 0x61})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("Title", new EBMLElement(2, (const unsigned char[]) {0x7B, 0xA9})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("MuxingApp", new EBMLElement(2, (const unsigned char[]) {0x4D, 0x80})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("WritingApp", new EBMLElement(2, (const unsigned char[]) {0x57, 0x41})));

    // Cluster
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("Cluster", new EBMLElement(4, (const unsigned char[]) {0x1F, 0x43, 0xB6, 0x75})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("Timecode", new EBMLElement(1, (const unsigned char[]) {0xE7})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("SilentTracks", new EBMLElement(2, (const unsigned char[]) {0x58, 0x54})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("SilentTrackNumber", new EBMLElement(2, (const unsigned char[]) {0x58, 0xD7})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("Position", new EBMLElement(1, (const unsigned char[]) {0xA7})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("PrevSize", new EBMLElement(1, (const unsigned char[]) {0xAB})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("SimpleBlock", new EBMLElement(1, (const unsigned char[]) {0xA3})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("BlockGroup", new EBMLElement(1, (const unsigned char[]) {0xA0})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("Block", new EBMLElement(1, (const unsigned char[]) {0xA1})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("BlockVirtual", new EBMLElement(1, (const unsigned char[]) {0xA2})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("BlockAdditions", new EBMLElement(2, (const unsigned char[]) {0x75, 0xA1})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("BlockMore", new EBMLElement(1, (const unsigned char[]) {0xA6})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("BlockAddID", new EBMLElement(1, (const unsigned char[]) {0xEE})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("BlockAdditional", new EBMLElement(1, (const unsigned char[]) {0xA5})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("BlockDuration", new EBMLElement(1, (const unsigned char[]) {0x9B})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("ReferencePriority", new EBMLElement(1, (const unsigned char[]) {0xFA})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("ReferenceBlock", new EBMLElement(1, (const unsigned char[]) {0xFB})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("ReferenceVirtual", new EBMLElement(1, (const unsigned char[]) {0xFD})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("CodecState", new EBMLElement(1, (const unsigned char[]) {0xA4})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("DiscardPadding", new EBMLElement(2, (const unsigned char[]) {0x75, 0xA2})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("Slices", new EBMLElement(1, (const unsigned char[]) {0x8E})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("TimeSlice", new EBMLElement(1, (const unsigned char[]) {0xE8})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("LaceNumber", new EBMLElement(1, (const unsigned char[]) {0xCC})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("FrameNumber", new EBMLElement(1, (const unsigned char[]) {0xCD})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("BlockAdditionID", new EBMLElement(1, (const unsigned char[]) {0xCB})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("Delay", new EBMLElement(1, (const unsigned char[]) {0xCE})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("SliceDuration", new EBMLElement(1, (const unsigned char[]) {0xCF})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("ReferenceFrame", new EBMLElement(1, (const unsigned char[]) {0xC8})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("ReferenceOffset", new EBMLElement(1, (const unsigned char[]) {0xC9})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("ReferenceTimeCode", new EBMLElement(1, (const unsigned char[]) {0xCA})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("EncryptedBlock", new EBMLElement(1, (const unsigned char[]) {0xAF})));

    // Track
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("Tracks", new EBMLElement(4, (const unsigned char[]) {0x16, 0x54, 0xAE, 0x6B})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("TrackEntry", new EBMLElement(1, (const unsigned char[]) {0xAE})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("TrackNumber", new EBMLElement(1, (const unsigned char[]) {0xD7})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("TrackUID", new EBMLElement(2, (const unsigned char[]) {0x73, 0xC5})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("TrackType", new EBMLElement(1, (const unsigned char[]) {0x83})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("FlagEnabled", new EBMLElement(1, (const unsigned char[]) {0xB9})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("FlagDefault", new EBMLElement(1, (const unsigned char[]) {0x88})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("FlagForced", new EBMLElement(2, (const unsigned char[]) {0x55, 0xAA})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("FlagLacing", new EBMLElement(1, (const unsigned char[]) {0x9C})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("MinCache", new EBMLElement(2, (const unsigned char[]) {0x6D, 0xE7})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("MaxCache", new EBMLElement(2, (const unsigned char[]) {0x6D, 0xF8})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("DefaultDuration", new EBMLElement(3, (const unsigned char[]) {0x23, 0xE3, 0x83})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("DefaultDecodedFieldDuration", new EBMLElement(3, (const unsigned char[]) {0x23, 0x4E, 0x7A})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("TrackTimecodeScale", new EBMLElement(3, (const unsigned char[]) {0x23, 0x31, 0x4F})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("TrackOffset", new EBMLElement(2, (const unsigned char[]) {0x53, 0x7F})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("MaxBlockAdditionID", new EBMLElement(2, (const unsigned char[]) {0x55, 0xEE})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("Name", new EBMLElement(2, (const unsigned char[]) {0x53, 0x6E})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("Language", new EBMLElement(3, (const unsigned char[]) {0x22, 0xB5, 0x9C})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("CodecID", new EBMLElement(1, (const unsigned char[]) {0x86})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("CodecPrivate", new EBMLElement(2, (const unsigned char[]) {0x63, 0xA2})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("CodecName", new EBMLElement(3, (const unsigned char[]) {0x25, 0x86, 0x88})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("AttachmentLink", new EBMLElement(2, (const unsigned char[]) {0x74, 0x46})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("CodecSettings", new EBMLElement(3, (const unsigned char[]) {0x3A, 0x96, 0x97})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("CodecInfoURL", new EBMLElement(3, (const unsigned char[]) {0x3B, 0x40, 0x40})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("CodecDownloadURL", new EBMLElement(3, (const unsigned char[]) {0x26, 0xB2, 0x40})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("CodecDecodeAll", new EBMLElement(1, (const unsigned char[]) {0xAA})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("TrackOverlay", new EBMLElement(2, (const unsigned char[]) {0x6F, 0xAB})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("CodecDelay", new EBMLElement(2, (const unsigned char[]) {0x56, 0xAA})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("SeekPreRoll", new EBMLElement(2, (const unsigned char[]) {0x56, 0xBB})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("TrackTranslate", new EBMLElement(2, (const unsigned char[]) {0x66, 0x24})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("TrackTranslateEditionUID", new EBMLElement(2, (const unsigned char[]) {0x66, 0xFC})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("TrackTranslateCodec", new EBMLElement(2, (const unsigned char[]) {0x66, 0xBF})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("TrackTranslateTrackID", new EBMLElement(2, (const unsigned char[]) {0x66, 0xA5})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("Video", new EBMLElement(1, (const unsigned char[]) {0xE0})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("FlagInterlaced", new EBMLElement(1, (const unsigned char[]) {0x9A})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("StereoMode", new EBMLElement(2, (const unsigned char[]) {0x53, 0xB8})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("AlphaMode", new EBMLElement(2, (const unsigned char[]) {0x53, 0xC0})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("OldStereoMode", new EBMLElement(2, (const unsigned char[]) {0x53, 0xB9})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("PixelWidth", new EBMLElement(1, (const unsigned char[]) {0xB0})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("PixelHeight", new EBMLElement(1, (const unsigned char[]) {0xBA})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("PixelCropBottom", new EBMLElement(2, (const unsigned char[]) {0x54, 0xAA})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("PixelCropTop", new EBMLElement(2, (const unsigned char[]) {0x54, 0xBB})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("PixelCropLeft", new EBMLElement(2, (const unsigned char[]) {0x54, 0xCC})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("PixelCropRight", new EBMLElement(2, (const unsigned char[]) {0x54, 0xDD})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("DisplayWidth", new EBMLElement(2, (const unsigned char[]) {0x54, 0xB0})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("DisplayHeight", new EBMLElement(2, (const unsigned char[]) {0x54, 0xBA})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("DisplayUnit", new EBMLElement(2, (const unsigned char[]) {0x54, 0xB2})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("AspectRatioType", new EBMLElement(2, (const unsigned char[]) {0x54, 0xB3})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("ColourSpace", new EBMLElement(3, (const unsigned char[]) {0x2E, 0xB5, 0x24})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("GammaValue", new EBMLElement(3, (const unsigned char[]) {0x2F, 0xB5, 0x23})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("FrameRate", new EBMLElement(3, (const unsigned char[]) {0x23, 0x83, 0xE3})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("Audio", new EBMLElement(1, (const unsigned char[]) {0xE1})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("SamplingFrequency", new EBMLElement(1, (const unsigned char[]) {0xB5})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("OutputSamplingFrequency", new EBMLElement(2, (const unsigned char[]) {0x78, 0xB5})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("Channels", new EBMLElement(1, (const unsigned char[]) {0x9F})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("ChannelPositions", new EBMLElement(2, (const unsigned char[]) {0x7D, 0x7B})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("BitDepth", new EBMLElement(2, (const unsigned char[]) {0x62, 0x64})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("TrackOperation", new EBMLElement(1, (const unsigned char[]) {0xE2})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("TrackCombinePlanes", new EBMLElement(1, (const unsigned char[]) {0xE3})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("TrackPlane", new EBMLElement(1, (const unsigned char[]) {0xE4})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("TrackPlaneUID", new EBMLElement(1, (const unsigned char[]) {0xE5})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("TrackPlaneType", new EBMLElement(1, (const unsigned char[]) {0xE6})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("TrackJoinBlocks", new EBMLElement(1, (const unsigned char[]) {0xE9})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("TrackJoinUID", new EBMLElement(1, (const unsigned char[]) {0xED})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("TrickTrackUID", new EBMLElement(1, (const unsigned char[]) {0xC0})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("TrickTrackSegmentUID", new EBMLElement(1, (const unsigned char[]) {0xC1})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("TrickTrackFlag", new EBMLElement(1, (const unsigned char[]) {0xC6})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("TrickMasterTrackUID", new EBMLElement(1, (const unsigned char[]) {0xC7})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("TrickMasterTrackSegmentUID", new EBMLElement(1, (const unsigned char[]) {0xC4})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("ContentEncodings", new EBMLElement(2, (const unsigned char[]) {0x6D, 0x80})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("ContentEncoding", new EBMLElement(2, (const unsigned char[]) {0x62, 0x40})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("ContentEncodingOrder", new EBMLElement(2, (const unsigned char[]) {0x50, 0x31})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("ContentEncodingScope", new EBMLElement(2, (const unsigned char[]) {0x50, 0x32})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("ContentEncodingType", new EBMLElement(2, (const unsigned char[]) {0x50, 0x33})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("ContentCompression", new EBMLElement(2, (const unsigned char[]) {0x50, 0x34})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("ContentCompAlgo", new EBMLElement(2, (const unsigned char[]) {0x42, 0x54})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("ContentCompSettings", new EBMLElement(2, (const unsigned char[]) {0x42, 0x55})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("ContentEncryption", new EBMLElement(2, (const unsigned char[]) {0x50, 0x35})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("ContentEncAlgo", new EBMLElement(2, (const unsigned char[]) {0x47, 0xE1})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("ContentEncKeyID", new EBMLElement(2, (const unsigned char[]) {0x47, 0xE2})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("ContentSignature", new EBMLElement(2, (const unsigned char[]) {0x47, 0xE3})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("ContentSigKeyID", new EBMLElement(2, (const unsigned char[]) {0x47, 0xE4})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("ContentSigAlgo", new EBMLElement(2, (const unsigned char[]) {0x47, 0xE5})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("ContentSigHashAlgo", new EBMLElement(2, (const unsigned char[]) {0x47, 0xE6})));

    // Cueing Data
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("Cues", new EBMLElement(4, (const unsigned char[]) {0x1C, 0x53, 0xBB, 0x6B})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("CuePoint", new EBMLElement(1, (const unsigned char[]) {0xBB})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("CueTime", new EBMLElement(1, (const unsigned char[]) {0xB3})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("CueTrackPositions", new EBMLElement(1, (const unsigned char[]) {0xB7})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("CueTrack", new EBMLElement(1, (const unsigned char[]) {0xF7})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("CueClusterPosition", new EBMLElement(1, (const unsigned char[]) {0xF1})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("CueRelativePosition", new EBMLElement(1, (const unsigned char[]) {0xF0})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("CueDuration", new EBMLElement(1, (const unsigned char[]) {0xB2})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("CueBlockNumber", new EBMLElement(2, (const unsigned char[]) {0x53, 0x78})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("CueCodecState", new EBMLElement(1, (const unsigned char[]) {0xEA})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("CueReference", new EBMLElement(1, (const unsigned char[]) {0xDB})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("CueRefTime", new EBMLElement(1, (const unsigned char[]) {0x96})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("CueRefCluster", new EBMLElement(1, (const unsigned char[]) {0x97})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("CueRefNumber", new EBMLElement(2, (const unsigned char[]) {0x53, 0x5F})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("CueRefCodecState", new EBMLElement(1, (const unsigned char[]) {0xEB})));

    // Attachment
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("Attachments", new EBMLElement(4, (const unsigned char[]) {0x19, 0x41, 0xA4, 0x69})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("AttachedFile", new EBMLElement(2, (const unsigned char[]) {0x61, 0xA7})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("FileDescription", new EBMLElement(2, (const unsigned char[]) {0x46, 0x7E})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("FileName", new EBMLElement(2, (const unsigned char[]) {0x46, 0x6E})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("FileMimeType", new EBMLElement(2, (const unsigned char[]) {0x46, 0x60})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("FileData", new EBMLElement(2, (const unsigned char[]) {0x46, 0x5C})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("FileUID", new EBMLElement(2, (const unsigned char[]) {0x46, 0xAE})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("FileReferral", new EBMLElement(2, (const unsigned char[]) {0x46, 0x75})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("FileUsedStartTime", new EBMLElement(2, (const unsigned char[]) {0x46, 0x61})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("FileUsedEndTime", new EBMLElement(2, (const unsigned char[]) {0x46, 0x62})));

    // Chapter
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("Chapters", new EBMLElement(4, (const unsigned char[]) {0x10, 0x43, 0xA7, 0x70})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("EditionEntry", new EBMLElement(2, (const unsigned char[]) {0x45, 0xB9})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("EditionUID", new EBMLElement(2, (const unsigned char[]) {0x45, 0xBC})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("EditionFlagHidden", new EBMLElement(2, (const unsigned char[]) {0x45, 0xBD})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("EditionFlagDefault", new EBMLElement(2, (const unsigned char[]) {0x45, 0xDB})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("EditionFlagOrdered", new EBMLElement(2, (const unsigned char[]) {0x45, 0xDD})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("ChapterAtom", new EBMLElement(1, (const unsigned char[]) {0xB6})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("ChapterUID", new EBMLElement(2, (const unsigned char[]) {0x73, 0xC4})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("ChapterStringUID", new EBMLElement(2, (const unsigned char[]) {0x56, 0x54})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("ChapterTimeStart", new EBMLElement(1, (const unsigned char[]) {0x91})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("ChapterTimeEnd", new EBMLElement(1, (const unsigned char[]) {0x92})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("ChapterFlagHidden", new EBMLElement(1, (const unsigned char[]) {0x98})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("ChapterFlagEnabled", new EBMLElement(2, (const unsigned char[]) {0x45, 0x98})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("ChapterSegmentUID", new EBMLElement(2, (const unsigned char[]) {0x6E, 0x67})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("ChapterSegmentEditionUID", new EBMLElement(2, (const unsigned char[]) {0x6E, 0xBC})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("ChapterPhysicalEquiv", new EBMLElement(2, (const unsigned char[]) {0x63, 0xC3})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("ChapterTrack", new EBMLElement(1, (const unsigned char[]) {0x8F})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("ChapterTrackNumber", new EBMLElement(1, (const unsigned char[]) {0x89})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("ChapterDisplay", new EBMLElement(1, (const unsigned char[]) {0x80})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("ChapString", new EBMLElement(1, (const unsigned char[]) {0x85})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("ChapLanguage", new EBMLElement(2, (const unsigned char[]) {0x43, 0x7C})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("ChapCountry", new EBMLElement(2, (const unsigned char[]) {0x43, 0x7E})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("ChapProcess", new EBMLElement(2, (const unsigned char[]) {0x69, 0x44})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("ChapProcessCodecID", new EBMLElement(2, (const unsigned char[]) {0x69, 0x55})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("ChapProcessPrivate", new EBMLElement(2, (const unsigned char[]) {0x45, 0x0D})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("ChapProcessCommand", new EBMLElement(2, (const unsigned char[]) {0x69, 0x11})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("ChapProcessTime", new EBMLElement(2, (const unsigned char[]) {0x69, 0x22})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("ChapProcessData", new EBMLElement(2, (const unsigned char[]) {0x69, 0x33})));

    // Tagging
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("Tags", new EBMLElement(4, (const unsigned char[]) {0x12, 0x54, 0xC3, 0x67})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("Tag", new EBMLElement(2, (const unsigned char[]) {0x73, 0x73})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("Targets", new EBMLElement(2, (const unsigned char[]) {0x63, 0xC0})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("TargetTypeValue", new EBMLElement(2, (const unsigned char[]) {0x68, 0xCA})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("TargetType", new EBMLElement(2, (const unsigned char[]) {0x63, 0xCA})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("TagTrackUID", new EBMLElement(2, (const unsigned char[]) {0x63, 0xC5})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("TagEditionUID", new EBMLElement(2, (const unsigned char[]) {0x63, 0xC9})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("TagChapterUID", new EBMLElement(2, (const unsigned char[]) {0x63, 0xC4})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("TagAttachmentUID", new EBMLElement(2, (const unsigned char[]) {0x63, 0xC6})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("SimpleTag", new EBMLElement(2, (const unsigned char[]) {0x67, 0xC8})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("TagName", new EBMLElement(2, (const unsigned char[]) {0x45, 0xA3})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("TagLanguage", new EBMLElement(2, (const unsigned char[]) {0x44, 0x7A})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("TagDefault", new EBMLElement(2, (const unsigned char[]) {0x44, 0x84})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("TagString", new EBMLElement(2, (const unsigned char[]) {0x44, 0x87})));
    elementDefinitions.insert(std::pair<std::string, EBMLElement*>("TagBinary", new EBMLElement(2, (const unsigned char[]) {0x44, 0x85})));
}

MatroskaEBMLElementDefinitions::~MatroskaEBMLElementDefinitions() {
    // delete all elements
    std::map<std::string, EBMLElement*>::iterator it = elementDefinitions.begin();
    while (it != elementDefinitions.end()) {
        delete it->second;
        it++;
    };
}

/**
 * Finds element definition of given name and returns reference to it as a shared pointer.
 * @param name string containing name of element (as in original specifications table)
 * @return shared pointer to element definition object or 0 on error (do not free after use!)
 */
EBMLElement* MatroskaEBMLElementDefinitions::getElementDefinitionByName(char *name) {
    std::map<std::string, EBMLElement*>::iterator it = elementDefinitions.find(name);
    if (it == elementDefinitions.end()) {
        std::cerr << "unknown EBML element definition " << name << "\n";
        return 0;
    } else {
        // *it is std::pair of key and value
        return it->second;
    }
}
