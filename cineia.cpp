
#include "cineia.h"

// Because provided APIs are too simple, we choose not to use them.
#include <parser/IABParser.h>
#include <packer/IABPacker.h>
#include <common/IABElements.h>

void showError(iabError error) {
    if(error != kIABNoError) {
        std::cout<<error;
        exit(error);
    }
}

iabError reassembleIAB(std::istream* iInputStream, std::vector<char> &oOutputBuffer, uint32_t &oOutputLength) {
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
                if(channelAudioDataID != bedDefinitionBedChannelAudioDataIDCounter && channelAudioDataID != 0)
                    return kValidateErrorAudioDataDLCDuplicateAudioDataID;
                channelAudioDataID = bedDefinitionBedChannelAudioDataIDCounter ++;
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
            if(objectDefinitionAudioDataID != objectDefinitionAudioDataIDCounter && objectDefinitionAudioDataID != 0)
                return kValidateErrorAudioDataDLCDuplicateAudioDataID;
            objectDefinitionAudioDataID = objectDefinitionAudioDataIDCounter ++;
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

    // TODO: fill AudioDataID = 0's AudioDataDLC.

    for(IABAudioDataDLC* element : oAudioDataDLCs) {
        IABAudioDataIDType audioDataID;
        element->GetAudioDataID(audioDataID);
        std::cout<<audioDataID<<std::endl;
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
/*
void copyPreambleValue(std::istream *iInputStream, std::ostream *oOutputStream) {
}
*/