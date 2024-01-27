/*
 Copyright (c) 2024. Steven Song (izwb-003)
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 */

#include "cineia.h"

// Because provided APIs are too simple, we choose not to use them.
#include <parser/IABParser.h>
#include <packer/IABPacker.h>
#include <common/IABElements.h>

// ST 2098-2:2019 Table 18 AudioDataDLC Sample Count versus Frame Rate Code and Sample Rate
int dlcSampleCountList[10][2] = {{2000, 4000},
                                 {1920, 3840},
                                 {1600, 3200},
                                 {1000, 2000},
                                 {960, 1920},
                                 {800, 1600},
                                 {500, 1000},
                                 {480, 960},
                                 {400, 800},
                                 {2002, 4004}};

iabError CineIA::getIABFrameInfo(std::stringstream *iInputStream, iabFrameInfo &oIABFrameInfo) {
    iabError error = kIABNoError;
    IABParser parser(iInputStream);

    error = parser.ParseIABFrame();
    const IABFrameInterface* parsedFrame;
    error = parser.GetIABFrame(parsedFrame);
    if(error != kIABNoError) return error;

    parsedFrame->GetSampleRate(oIABFrameInfo.sampleRate);
    parsedFrame->GetMaxRendered(oIABFrameInfo.maxRendered);
    parsedFrame->GetFrameRate(oIABFrameInfo.frameRate);
    parsedFrame->GetBitDepth(oIABFrameInfo.bitDepth);

    std::vector<IABElement*> subElements;
    parsedFrame->GetSubElements(subElements);

    for(IABElement* subElement : subElements) {
        if(IABBedDefinition* bedDefinition = dynamic_cast<IABBedDefinition*>(subElement)) {
            oIABFrameInfo.bedDefinitionCount ++;
            IABChannelCountType bedDefinitionChannelCount;
            bedDefinition->GetChannelCount(bedDefinitionChannelCount);
            oIABFrameInfo.bedDefinitionChannelCount = (uint32_t)bedDefinitionChannelCount;
        }
        else if(IABObjectDefinition* objectDefinition = dynamic_cast<IABObjectDefinition*>(subElement))
            oIABFrameInfo.objectDefinitionCount ++;
    }

    return error;
}

void CineIA::showError(iabError error) {
    if(error != kIABNoError) {
        std::cout<<error;
        exit(error);
    }
}

iabError CineIA::reassembleIAB(std::istream *iInputStream, std::vector<char> &oOutputBuffer, uint32_t &oOutputLength) {
    // Error variable
    iabError error = kIABNoError;

    // Build IMF parser and Atmos packer
    IABParser* parser = new IABParser(iInputStream);
    IABPacker* packer = new IABPacker();

    // Build IMF IABFrame and Atmos IABFrame pointer
    IABFrameInterface* iIABFrame;
    IABFrameInterface* oIABFrame;

    // Parse and get IMF IABFrame
    parser->ParseIABFrame();
    parser->GetIABFrameReleased(iIABFrame);

    // Get Atmos IABFrame
    packer->GetIABFrame(oIABFrame);

    // Copy necessary IABFrame settings
    IABVersionNumberType frameVersion;
    IABSampleRateType frameSampleRate;
    IABBitDepthType frameBitDepth;
    IABFrameRateType frameFrameRate;
    IABMaxRenderedRangeType frameMaxRendered;

    iIABFrame->GetVersion(frameVersion);
    iIABFrame->GetSampleRate(frameSampleRate);
    iIABFrame->GetBitDepth(frameBitDepth);
    iIABFrame->GetFrameRate(frameFrameRate);
    iIABFrame->GetMaxRendered(frameMaxRendered);

    oIABFrame->SetVersion(frameVersion);
    oIABFrame->SetSampleRate(frameSampleRate);
    oIABFrame->SetBitDepth(frameBitDepth);
    oIABFrame->SetFrameRate(frameFrameRate);
    oIABFrame->SetMaxRendered(frameMaxRendered);

    // Parse and copy subelements
    std::vector<IABElement*> iSubElements;
    std::vector<IABElement*> oSubElements;

    std::vector<IABAudioDataDLC*> oAudioDataDLCs;
    std::vector<IABBedDefinition*> oBedDefinitions;
    std::vector<IABObjectDefinition*> oObjectDefinitions;

    iIABFrame->GetSubElements(iSubElements);

    /*
     * Dolby Atmos' constraint requires ObjectDefinition's MetaID to begin from 1.
     * So we have to keep a counter to edit ObjectDefinition's MetaID in an order.
     */
    IABMetadataIDType objectDefinitionMetaIDCounter = 1;

    /*
     * In a real Dolby Atmos bitstream, there will not appear AudioDataID = 0.
     * So we have to fill those elements which uses AudioDataID = 0 with Audio data that has no sound.
     * The following two variables are used to store a counter to mark which AudioDataID the current element should use.
     */
    IABAudioDataIDType bedDefinitionBedChannelAudioDataIDCounter = 100;
    IABAudioDataIDType objectDefinitionAudioDataIDCounter = 300;
    std::vector<IABAudioDataDLC*>::iterator currentAudioDataDLCIterator;

    for(IABElement* iSubElement : iSubElements) {
        // Copy AudioData elements
        if(IABAudioDataPCM* iAudioDataPCM = dynamic_cast<IABAudioDataPCM*>(iSubElement)) {
            // Copy necessary AudioData settings
            IABFrameRateType audioDataFrameRate;
            IABSampleRateType audioDataSampleRate;
            IABAudioDataIDType audioDataID;
            audioDataFrameRate = iAudioDataPCM->GetPCMFrameRate();
            audioDataSampleRate = iAudioDataPCM->GetPCMSampleRate();
            iAudioDataPCM->GetAudioDataID(audioDataID);

            // Build output AudioDataDLC element and set settings
            IABAudioDataDLC* oAudioDataDLC = new IABAudioDataDLC(audioDataFrameRate, audioDataSampleRate, error);
            if(error != kIABNoError) return error;
            oAudioDataDLC->SetAudioDataID(audioDataID);

            // Encode DLC audio
            uint32_t audioDataPCMSampleCount = iAudioDataPCM->GetPCMSampleCount();
            int32_t* audioDataPCMSamples = new int32_t[audioDataPCMSampleCount];
            iAudioDataPCM->UnpackPCMToMonoSamples(audioDataPCMSamples, audioDataPCMSampleCount);
            oAudioDataDLC->EncodeMonoPCMToDLC(audioDataPCMSamples, audioDataPCMSampleCount);
            delete[] audioDataPCMSamples;
            audioDataPCMSamples = nullptr;

            // Add to output AudioDataDLCs vector
            oAudioDataDLCs.push_back(oAudioDataDLC);
        }
        // Copy BedDefinition Element
        else if(IABBedDefinition* iBedDefinition = dynamic_cast<IABBedDefinition*>(iSubElement)) {
            // Initialize oAudioDataDLC iterator
            currentAudioDataDLCIterator = oAudioDataDLCs.begin();

            // Build output BedDefinition element
            IABBedDefinition* oBedDefinition = new IABBedDefinition(frameFrameRate);

            // Copy necessary BedDefinition settings
            IABMetadataIDType bedDefinitionMetaID = 0;  // * Dolby Atmos' constraint limits BedDefinition's MetaID to 0.
            uint1_t bedDefinitionIsConditionalBed;
            IABUseCaseType bedDefinitionBedUseCase;

            iBedDefinition->GetConditionalBed(bedDefinitionIsConditionalBed);
            iBedDefinition->GetBedUseCase(bedDefinitionBedUseCase);

            oBedDefinition->SetMetadataID(bedDefinitionMetaID);
            oBedDefinition->SetConditionalBed(bedDefinitionIsConditionalBed);
            oBedDefinition->SetBedUseCase(bedDefinitionBedUseCase);

            // Copy bed channels
            std::vector<IABChannel*> iBedDefinitionBedChannels;
            std::vector<IABChannel*> oBedDefinitionBedChannels;

            iBedDefinition->GetBedChannels(iBedDefinitionBedChannels);

            for(IABChannel* iBedDefinitionBedChannel : iBedDefinitionBedChannels) {

                // Build output channel
                IABChannel* oBedDefinitionBedChannel = new IABChannel();

                // Copy necessary channel settings and modify AudioDataID = 0
                IABChannelIDType channelID;
                IABAudioDataIDType channelAudioDataID;
                IABGain channelGain;
                uint1_t channelDecorInfoExists;

                iBedDefinitionBedChannel->GetChannelID(channelID);
                iBedDefinitionBedChannel->GetAudioDataID(channelAudioDataID);
                if(channelAudioDataID == bedDefinitionBedChannelAudioDataIDCounter) {
                    bedDefinitionBedChannelAudioDataIDCounter ++;
                    currentAudioDataDLCIterator ++;
                }
                else if(channelAudioDataID == 0) {
                    channelAudioDataID = bedDefinitionBedChannelAudioDataIDCounter ++;
                    IABAudioDataDLC* newAudioDataDLC = new IABAudioDataDLC(frameFrameRate, frameSampleRate, error);
                    newAudioDataDLC->SetAudioDataID(channelAudioDataID);
                    int32_t* muteSamples = new int32_t[dlcSampleCountList[frameFrameRate][frameSampleRate]]();
                    newAudioDataDLC->EncodeMonoPCMToDLC(muteSamples, dlcSampleCountList[frameFrameRate][frameSampleRate]);
                    delete[] muteSamples;
                    if(error != kIABNoError) return error;
                    currentAudioDataDLCIterator = oAudioDataDLCs.insert(currentAudioDataDLCIterator, newAudioDataDLC);
                    currentAudioDataDLCIterator ++;
                }
                else
                    return kValidateErrorAudioDataDLCDuplicateAudioDataID;
                iBedDefinitionBedChannel->GetChannelGain(channelGain);
                iBedDefinitionBedChannel->GetDecorInfoExists(channelDecorInfoExists);

                oBedDefinitionBedChannel->SetChannelID(channelID);
                oBedDefinitionBedChannel->SetAudioDataID(channelAudioDataID);
                oBedDefinitionBedChannel->SetChannelGain(channelGain);
                oBedDefinitionBedChannel->SetDecorInfoExists(channelDecorInfoExists);

                // Add to output channel vector
                oBedDefinitionBedChannels.push_back(oBedDefinitionBedChannel);
            }

            oBedDefinition->SetBedChannels(oBedDefinitionBedChannels);

            // Add to output BedDefinitions vector
            oBedDefinitions.push_back(oBedDefinition);
        }
        // Copy ObjectDefinition element
        else if(IABObjectDefinition* iObjectDefinition = dynamic_cast<IABObjectDefinition*>(iSubElement)) {
            // Build output ObjectDefinition element
            IABObjectDefinition* oObjectDefinition = new IABObjectDefinition(frameFrameRate);

            // Copy necessary ObjectDefinition settings and modify AudioDataID = 0
            IABMetadataIDType objectDefinitionMetaID = objectDefinitionMetaIDCounter ++;
            IABAudioDataIDType objectDefinitionAudioDataID;
            uint1_t objectDefinitionIsConditionalObject;
            IABUseCaseType objectDefinitionObjectUseCase;

            iObjectDefinition->GetAudioDataID(objectDefinitionAudioDataID);
            if(objectDefinitionAudioDataID == objectDefinitionAudioDataIDCounter) {
                objectDefinitionAudioDataIDCounter ++;
            }
            else if(objectDefinitionAudioDataID == 0) {
                objectDefinitionAudioDataID = objectDefinitionAudioDataIDCounter ++;
                IABAudioDataDLC* newAudioDataDLC = new IABAudioDataDLC(frameFrameRate, frameSampleRate, error);
                newAudioDataDLC->SetAudioDataID(objectDefinitionAudioDataID);
                int32_t* muteSamples = new int32_t[dlcSampleCountList[frameFrameRate][frameSampleRate]]();
                newAudioDataDLC->EncodeMonoPCMToDLC(muteSamples, dlcSampleCountList[frameFrameRate][frameSampleRate]);
                delete[] muteSamples;
                if(error != kIABNoError) return error;
                oAudioDataDLCs.push_back(newAudioDataDLC);
            }
            else
                return kValidateErrorAudioDataDLCDuplicateAudioDataID;
            iObjectDefinition->GetConditionalObject(objectDefinitionIsConditionalObject);
            iObjectDefinition->GetObjectUseCase(objectDefinitionObjectUseCase);

            oObjectDefinition->SetMetadataID(objectDefinitionMetaID);
            oObjectDefinition->SetAudioDataID(objectDefinitionAudioDataID);
            oObjectDefinition->SetConditionalObject(objectDefinitionIsConditionalObject);
            oObjectDefinition->SetObjectUseCase(objectDefinitionObjectUseCase);

            // Copy object PanSubBlocks
            std::vector<IABObjectSubBlock*> iObjectDefinitionPanSubBlocks;
            std::vector<IABObjectSubBlock*> oObjectDefinitionPanSubBlocks;

            iObjectDefinition->GetPanSubBlocks(iObjectDefinitionPanSubBlocks);

            for(IABObjectSubBlock* iObjectDefinitionPanSubBlock : iObjectDefinitionPanSubBlocks) {
                // Build output PanSubBlock
                IABObjectSubBlock* oObjectDefinitionPanSubBlock = new IABObjectSubBlock();

                // Copy necessary PanSubBlock settings
                uint1_t panSubBlockPanInfoExists;
                iObjectDefinitionPanSubBlock->GetPanInfoExists(panSubBlockPanInfoExists);
                if(!panSubBlockPanInfoExists) {
                    oObjectDefinitionPanSubBlock->SetPanInfoExists(panSubBlockPanInfoExists);
                    oObjectDefinitionPanSubBlocks.push_back(oObjectDefinitionPanSubBlock);
                    continue;
                }
                oObjectDefinitionPanSubBlock->SetPanInfoExists(panSubBlockPanInfoExists);

                // Copy necessary ObjectDefinition PanSubBlock settings
                IABGain panSubBlockObjectGain;
                CartesianPosInUnitCube panSubBlockObjectPosition;
                IABObjectSnap panSubBlockObjectSnap;
                IABObjectZoneGain9 panSubBlockObjectZoneGain9;
                IABObjectSpread panSubBlockObjectSpread;
                IABDecorCoeff panSubBlockDecorCoeff;

                iObjectDefinitionPanSubBlock->GetObjectGain(panSubBlockObjectGain);
                iObjectDefinitionPanSubBlock->GetObjectPositionToUnitCube(panSubBlockObjectPosition);
                iObjectDefinitionPanSubBlock->GetObjectSnap(panSubBlockObjectSnap);
                iObjectDefinitionPanSubBlock->GetObjectZoneGains9(panSubBlockObjectZoneGain9);
                iObjectDefinitionPanSubBlock->GetObjectSpread(panSubBlockObjectSpread);
                iObjectDefinitionPanSubBlock->GetDecorCoef(panSubBlockDecorCoeff);

                oObjectDefinitionPanSubBlock->SetObjectGain(panSubBlockObjectGain);
                oObjectDefinitionPanSubBlock->SetObjectPositionFromUnitCube(panSubBlockObjectPosition);
                oObjectDefinitionPanSubBlock->SetObjectSnap(panSubBlockObjectSnap);
                oObjectDefinitionPanSubBlock->SetObjectZoneGains9(panSubBlockObjectZoneGain9);
                oObjectDefinitionPanSubBlock->SetObjectSpread(panSubBlockObjectSpread);
                oObjectDefinitionPanSubBlock->SetDecorCoef(panSubBlockDecorCoeff);

                // Add to output PanSubBlock vector
                oObjectDefinitionPanSubBlocks.push_back(oObjectDefinitionPanSubBlock);
            }

            oObjectDefinition->SetPanSubBlocks(oObjectDefinitionPanSubBlocks);

            // Add to output ObjectDefinitions vector
            oObjectDefinitions.push_back(oObjectDefinition);
        }
        else
            return kValidateErrorIAFrameUndefinedElementType;
    }

    // Assemble output subelements vector
    size_t subElementsVectorSize = oAudioDataDLCs.size() + oBedDefinitions.size() + oObjectDefinitions.size();
    oSubElements.reserve(subElementsVectorSize);

    oSubElements.insert(oSubElements.end(), std::make_move_iterator(oAudioDataDLCs.begin()), std::make_move_iterator(oAudioDataDLCs.end()));
    oSubElements.insert(oSubElements.end(), std::make_move_iterator(oBedDefinitions.begin()), std::make_move_iterator(oBedDefinitions.end()));
    oSubElements.insert(oSubElements.end(), std::make_move_iterator(oObjectDefinitions.begin()), std::make_move_iterator(oObjectDefinitions.end()));

    // Write subelements to Atmos IABFrame
    oIABFrame->SetSubElements(oSubElements);

    // Pack Atmos IABFrame and output
    packer->PackIABFrame();
    packer->GetPackedBuffer(oOutputBuffer, oOutputLength);

    // Free memory
    delete iIABFrame;
    iIABFrame = nullptr;
    delete parser;
    parser = nullptr;
    delete packer;
    packer = nullptr;

    return kIABNoError;
}

uint32_t reverseBytes(uint32_t value) {
    return ((value >> 24) & 0x000000FF) |
           ((value >> 8) & 0x0000FF00) |
           ((value << 8) & 0x00FF0000) |
           ((value << 24) & 0xFF000000);
}

void CineIA::copyPreambleValue(std::istream *iIMFBuffer, std::vector<char> &ioOutputBuffer, uint32_t &ioOutputLength) {
    // Copy PreambleLength
    uint32_t preambleLength;
    iIMFBuffer->seekg(1, std::ios::beg);
    iIMFBuffer->read(reinterpret_cast<char*>(&preambleLength), 4);
    std::copy(reinterpret_cast<char*>(&preambleLength), reinterpret_cast<char*>(&preambleLength) + 4, ioOutputBuffer.begin() + 1);
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    preambleLength = reverseBytes(preambleLength);
#endif

    // Copy PreambleValue
    std::vector<char> preambleValue(preambleLength);
    iIMFBuffer->read(preambleValue.data(), preambleLength);
    ioOutputBuffer.insert(ioOutputBuffer.begin() + 5, preambleValue.begin(), preambleValue.end());
    ioOutputLength += preambleLength;
}

int CineIA::convertFrameRate(IABFrameRateType iFrameRate) {
    switch(iFrameRate) {
        case kIABFrameRate_24FPS:
            return 24;
        case kIABFrameRate_25FPS:
            return 25;
        case kIABFrameRate_30FPS:
            return 30;
        case kIABFrameRate_48FPS:
            return 48;
        case kIABFrameRate_50FPS:
            return 50;
        case kIABFrameRate_60FPS:
            return 60;
        case kIABFrameRate_96FPS:
            return 96;
        case kIABFrameRate_100FPS:
            return 100;
        case kIABFrameRate_120FPS:
            return 120;
        default:
            return 0;
    }
    return 0;
}

int CineIA::convertSampleRate(IABSampleRateType iSampleRate) {
    switch(iSampleRate) {
        case kIABSampleRate_48000Hz:
            return 48000;
        case kIABSampleRate_96000Hz:
            return 96000;
        default:
            return 0;
    }
    return 0;
}

int CineIA::convertBitDepth(IABBitDepthType iBitDepth) {
    switch(iBitDepth) {
        case kIABBitDepth_16Bit:
            return 16;
        case kIABBitDepth_24Bit:
            return 24;
        default:
            return 0;
    }
    return 0;
}
