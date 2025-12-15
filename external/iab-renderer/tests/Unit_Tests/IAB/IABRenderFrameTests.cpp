/*======================================================================*
    Copyright (c) 2015-2023 DTS, Inc. and its affiliates.

    Redistribution and use in source and binary forms, with or without modification,
    are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this
    list of conditions and the following disclaimer.

    2. Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.

    3. Neither the name of the copyright holder nor the names of its contributors
    may be used to endorse or promote products derived from this software without
    specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
    ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
    ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *======================================================================*/

#include "gtest/gtest.h"
#include "common/IABElements.h"
#include "IABUtilities.h"
#include "packer/IABPacker.h"
#include "parser/IABParser.h"
#include "IABRendererAPI.h"
#include "testcfg.h"

#include <vector>
#include <memory>

using namespace SMPTE::ImmersiveAudioBitstream;

namespace
{
    // IAB Render Frame tests:
    // 1. Create IAB Frame
    // 2. Create instance for the IAB Renderer
    // 3. Call the RenderFrame API by passing the IAB Frame
    // 4. Do check the renderer outputs.
    
    class IABRenderFrame_Test : public testing::Test
    {
    protected:
        
        // Set up test variables and configuration
        void SetUp()
        {
            outBuffer_ = NULL;
            outPointers_ = NULL;
            iabRendererinterface_ = NULL;
            iabFrameSampleCount_ = 0;
            iabFrameInterface_ = NULL;
            rendererConfig_ = NULL;

			setupRenderer();
        }
        
        // void TearDown() to do any clean-up
        void TearDown()
        {
            deleteInstances();
        }
        
        void deleteInstances()
        {
        
            delete [] outBuffer_;
            delete [] outPointers_;
            
            if (iabFrameInterface_)
            {
                IABFrameInterface::Delete(iabFrameInterface_);
            }
            
            if (iabRendererinterface_)
            {
                IABRendererInterface::Delete(iabRendererinterface_);
            }
            
            if (rendererConfig_)
            {
                delete rendererConfig_;
            }
            
            outBuffer_ = NULL;
            outPointers_ = NULL;
            iabRendererinterface_ = NULL;
            rendererConfig_ = NULL;
            iabFrameInterface_ = NULL;
        }
        
        //  7.1.2 bed channels
		// Caller owns returned instances and must manage and delete them after use.
		// (currently this test replies on IABFrameInterface::Delete(iabFrameInterface_);
		// to delete all objects throughout hierarchy.)
        std::vector<IABChannel*> createBedChannels()
        {
            IABChannel *channelL = new IABChannel();
            IABChannel *channelC = new IABChannel();
            IABChannel *channelR = new IABChannel();
            IABChannel *channelLss = new IABChannel();
            IABChannel *channelLsr = new IABChannel();
            IABChannel *channelRsr = new IABChannel();
            IABChannel *channelRss = new IABChannel();
            IABChannel *channelLFE = new IABChannel();
            IABChannel *channelLts = new IABChannel();
            IABChannel *channelRts = new IABChannel();

            channelL->SetChannelID(kIABChannelID_Left);
            channelC->SetChannelID(kIABChannelID_Center);
            channelR->SetChannelID(kIABChannelID_Right);
            channelLss->SetChannelID(kIABChannelID_LeftSideSurround);
            channelLsr->SetChannelID(kIABChannelID_LeftRearSurround);
            channelRsr->SetChannelID(kIABChannelID_RightRearSurround);
            channelRss->SetChannelID(kIABChannelID_RightSideSurround);
            channelLFE->SetChannelID(kIABChannelID_LFE);
            channelLts->SetChannelID(kIABChannelID_RightTopSurround);
            channelRts->SetChannelID(kIABChannelID_LeftTopSurround);
            
            std::vector<IABChannel*> bedChannels;
            bedChannels.push_back(channelL);
            bedChannels.push_back(channelC);
            bedChannels.push_back(channelR);
            bedChannels.push_back(channelLss);
            bedChannels.push_back(channelLsr);
            bedChannels.push_back(channelRsr);
            bedChannels.push_back(channelRss);
            bedChannels.push_back(channelLFE);
            bedChannels.push_back(channelLts);
            bedChannels.push_back(channelRts);
            
            return bedChannels;
        }

		// Caller owns returned instances and must manage and delete them after use.
		// (currently this test replies on IABFrameInterface::Delete(iabFrameInterface_);
		// to delete all objects throughout hierarchy.)
        IABBedDefinition* createBedDefinitionElement(uint32_t audioID)
        {
            IABBedDefinition *bedDefinitionElement = new IABBedDefinition(kIABFrameRate_24FPS);
            
            std::vector<IABChannel*> bedChannels = createBedChannels();
            
            for (uint32_t i = 0; i < bedChannels.size(); i++)
            {
                    bedChannels.at(i)->SetAudioDataID(audioID);
            }

            bedDefinitionElement->SetConditionalBed(0);
            bedDefinitionElement->SetBedChannels(bedChannels);
            
            return bedDefinitionElement;
        }
        
		// Caller owns returned instances and must manage and delete them after use.
		// (currently this test replies on IABFrameInterface::Delete(iabFrameInterface_);
		// to delete all objects throughout hierarchy.)
		// Also note this function intentionally takes an input parameter, numPanSubBlocks,
		// such that for test purposes, numbers can be set that violates IAB specification.
		IABObjectDefinition* createObjectDefinitionElement(uint32_t numPanSubBlocks = 0)
        {
            IABObjectDefinition *objectDefinitionElement = new IABObjectDefinition(kIABFrameRate_24FPS);
            
            std::vector<IABObjectSubBlock*> panSubBlocks;
            
            for (uint8_t i = 0; i < numPanSubBlocks; i++)
            {
                IABObjectSubBlock *subBlock = new IABObjectSubBlock();
                subBlock->SetPanInfoExists(1);
        
                // Add to object
                panSubBlocks.push_back(subBlock);
            }
            
            objectDefinitionElement->SetPanSubBlocks(panSubBlocks);
            
            return objectDefinitionElement;
        }

		// Caller owns returned instances and must manage and delete them after use.
		// (currently this test relies on IABFrameInterface::Delete(iabFrameInterface_);
		// to delete all objects throughout hierarchy.)
		// Also note this function intentionally takes an input parameter, frameSampleCount,
		// such that for test purposes, numbers can be set that violates IAB specification.
		IABAudioDataDLC* createDLCDefinitionElement(IABFrameRateType frameRate, uint32_t frameSampleCount)
        {
			IABAudioDataDLC *dlcAudioElement = dynamic_cast<IABAudioDataDLC*>(IABAudioDataDLCInterface::Create(kIABFrameRate_24FPS, kIABSampleRate_48000Hz));
            dlcAudioElement->SetAudioDataID(0);
            dlcAudioElement->SetDLCSampleRate(kIABSampleRate_48000Hz);
            
            // Use silent audio samples
            int32_t* pAudioSamples = new int32_t[frameSampleCount];
            memset(pAudioSamples, 0, sizeof(int32_t)*frameSampleCount);
            
            // encode
            dlcAudioElement->EncodeMonoPCMToDLC(pAudioSamples, frameSampleCount);

			delete[] pAudioSamples;
        
            return dlcAudioElement;
        }

        // Caller owns returned instances and must manage and delete them after use.
        // (currently this test replies on IABFrameInterface::Delete(iabFrameInterface_);
        // to delete all objects throughout hierarchy.)
        IABAudioDataPCM* createPCMDefinitionElement(IABSampleRateType iSampleRate,
                                                    IABFrameRateType iFrameRate,
                                                    IABBitDepthType iBitDepth,
                                                    int32_t iFrameSampleCount)
        {
            IABAudioDataPCM *pcmAudioElement = dynamic_cast<IABAudioDataPCM*>(IABAudioDataPCMInterface::Create(iFrameRate, iSampleRate, iBitDepth));
            pcmAudioElement->SetAudioDataID(0);
            
            // Use silent audio samples
            int32_t* pAudioSamples = new int32_t[iFrameSampleCount];
            memset(pAudioSamples, 0, sizeof(int32_t)*iFrameSampleCount);
            
            pcmAudioElement->PackMonoSamplesToPCM(pAudioSamples, iFrameSampleCount);
            
            delete[] pAudioSamples;
            
            return pcmAudioElement;
        }

        // Create IAB frame with Bed defintion only
        void createIABFrameWithBedDefinition()
        {
            iabFrameInterface_ = IABFrameInterface::Create(NULL);
            
            // Set audio sampling rate
            IABSampleRateType sampleRate = kIABSampleRate_48000Hz;
            iabFrameInterface_->SetSampleRate(sampleRate);
            
			// Set Frame rate
			IABFrameRateType frameRate = kIABFrameRate_24FPS;
            iabFrameInterface_->SetFrameRate(frameRate);
            
            iabFrameSampleCount_ = GetIABNumFrameSamples(frameRate, sampleRate);
            
            IABBedDefinition *bedDefinitionElement = createBedDefinitionElement(0);

            std::vector<IABElement*> frameSubElements;
            
            frameSubElements.push_back(bedDefinitionElement);
            
            // Set frame sub elements
            iabFrameInterface_->SetSubElements(frameSubElements);
        }
        
        // Create IAB frame with Object defintion only
        void createIABFrameWithObjectDefinition()
        {
            iabFrameInterface_ = IABFrameInterface::Create(NULL);
            
			// Set audio sampling rate
			IABSampleRateType sampleRate = kIABSampleRate_48000Hz;
			iabFrameInterface_->SetSampleRate(sampleRate);

			// Set Frame rate
			IABFrameRateType frameRate = kIABFrameRate_24FPS;
			iabFrameInterface_->SetFrameRate(frameRate);

			iabFrameSampleCount_ = GetIABNumFrameSamples(frameRate, sampleRate);

            IABObjectDefinition *element = createObjectDefinitionElement(8);
            
            std::vector<IABElement*> frameSubElements;
            
            frameSubElements.push_back(element);
            
            // Set frame sub elements
            iabFrameInterface_->SetSubElements(frameSubElements);
        }
        
        // Create IAB frame with Bed defintion element with DLC audio data elemenent
        void createIABFrameWithBedDefinitionAndDLCData(uint32_t audioID)
        {
            iabFrameInterface_ = IABFrameInterface::Create(NULL);
            
			// Set audio sampling rate
			IABSampleRateType sampleRate = kIABSampleRate_48000Hz;
			iabFrameInterface_->SetSampleRate(sampleRate);

			// Set Frame rate
			IABFrameRateType frameRate = kIABFrameRate_24FPS;
			iabFrameInterface_->SetFrameRate(frameRate);

			iabFrameSampleCount_ = GetIABNumFrameSamples(frameRate, sampleRate);

            IABBedDefinition *bedDefinitionElement = createBedDefinitionElement(audioID);
            IABAudioDataDLC *dlcAudioElement = createDLCDefinitionElement(kIABFrameRate_24FPS, iabFrameSampleCount_);
            dlcAudioElement->SetAudioDataID(audioID);
            
            std::vector<IABElement*> frameSubElements;
            
            frameSubElements.push_back(bedDefinitionElement);
            frameSubElements.push_back(dlcAudioElement);
            
            // Set frame sub elements
            iabFrameInterface_->SetSubElements(frameSubElements);
        }

        // Sets up the basic iab renderer
        void setupRenderer(bool setupSpeakerMap = true)
        {
            channelList_.clear();
            
            rendererConfig_ = RenderUtils::IRendererConfigurationFile::FromBuffer((char*) c916cfg.c_str());
            
            ASSERT_TRUE(NULL != rendererConfig_);
            
            // Instantiate an IABRenderer with the configuration
            iabRendererinterface_ = IABRendererInterface::Create(*rendererConfig_);
            
            // Get output channel count from IAB renderer instance
            outputChannelCount_ = iabRendererinterface_->GetOutputChannelCount();
            
            // Get maximum sample count (per channel) from IAB renderer instance, for buffer allocation
            maxOutputSampleCount_ = iabRendererinterface_->GetMaxOutputSampleCount();
            
            // Get speaker list from the configuration. This will be used to create output wav files etc
            std::vector<RenderUtils::RenderSpeaker>::const_iterator iter;
            const std::vector<RenderUtils::RenderSpeaker>*	speakerList;
            
            rendererConfig_->GetSpeakers(speakerList);
            
            ASSERT_TRUE (0 != (*speakerList).size());
            
            if (setupSpeakerMap)
            {
                // Use speaker information to set up a speaker index and renderer output channel index map
                // The map enables us to access the correct rendered channel to write to the corresponding wav file
                // Note that the map will include all output to real/physical speakers, but exclude any "virtual" speakers that are present in "*speakerList"
                
				int32_t speakerIndex = 0;
                speakerRendererOutputMap_.clear();
                
                for (iter = (* speakerList).begin(); iter != (* speakerList).end(); iter++)
                {
                    if (iter->ch_ != -1)
                    {
                        // ch_ = -1 indicates a virtuall speaker and should be bypassed
                        channelList_.push_back(iter->name_);                       // save channel label
                        speakerRendererOutputMap_[speakerIndex++] = iter->ch_;     // save rendered channel index to its corresponding speaker index
                    }
                }
                
                ASSERT_TRUE (0 != channelList_.size());
                ASSERT_TRUE(channelList_.size() == outputChannelCount_);
                
            }
            
            // Allocate output buffers for each rendered output channel with maximum sample per channel count supported by the renderer
            // Actual number of samples per channel depends on the IAB bitstream frame rate and sample rate and is
            // less than or equal to maxOutputSampleCount_
            
            outBuffer_ = new float[outputChannelCount_ * maxOutputSampleCount_];
            outPointers_ = new float*[outputChannelCount_];
            
            // Set up output buffer pointer array for passing to IAB renderer
            for(uint32_t i = 0; i < outputChannelCount_; i++)
            {
                outPointers_[i] = outBuffer_ + i * maxOutputSampleCount_;
            }
        }
        
        // ********************
        // RenderIABFrame API tests.
        // ********************
            
        // IAB BedDefinition rendering test
        void Test_BedDefinitionRendering()
        {
            createIABFrameWithBedDefinition();

            IABRenderedOutputSampleCountType renderedOutputSampleCount = 0;
            iabError returnCode = kIABNoError;
            
            // Valid frame with bed definition.
            returnCode = iabRendererinterface_->RenderIABFrame(*iabFrameInterface_, outPointers_, outputChannelCount_, iabFrameSampleCount_, renderedOutputSampleCount);
            ASSERT_TRUE(returnCode == kIABNoError);
            
            std::vector<IABChannel*> bedChannels;
            std::vector<IABElement*> frameSubElements;
            
            // Set empty bed channels and make bed renderering fail.
            {
                IABBedDefinition* bed = createBedDefinitionElement(0);
                bed->SetBedChannels(bedChannels);
                frameSubElements.push_back(bed);
                iabFrameInterface_->SetSubElements(frameSubElements);
                
                returnCode = iabRendererinterface_->RenderIABFrame(*iabFrameInterface_, outPointers_, outputChannelCount_, iabFrameSampleCount_, renderedOutputSampleCount);
                ASSERT_TRUE(returnCode == kIABRendererBedDefinitionError);
            }
            
            // Again set valid bed channels.
            {
                IABBedDefinition* bed = createBedDefinitionElement(0);
                frameSubElements.clear();
                bedChannels = createBedChannels();
                bed->SetBedChannels(bedChannels);
                frameSubElements.push_back(bed);
                iabFrameInterface_->SetSubElements(frameSubElements);
                
                returnCode = iabRendererinterface_->RenderIABFrame(*iabFrameInterface_, outPointers_, outputChannelCount_, iabFrameSampleCount_, renderedOutputSampleCount);
                ASSERT_TRUE(returnCode == kIABNoError);
            }
            
            // Pass output pointers as NULL
            {
                IABBedDefinition* bed = createBedDefinitionElement(0);
                frameSubElements.clear();
                bedChannels = createBedChannels();
                bed->SetBedChannels(bedChannels);
                frameSubElements.push_back(bed);
                iabFrameInterface_->SetSubElements(frameSubElements);
                
                returnCode = iabRendererinterface_->RenderIABFrame(*iabFrameInterface_, NULL, outputChannelCount_, iabFrameSampleCount_, renderedOutputSampleCount);
                ASSERT_TRUE(returnCode == kIABBadArgumentsError);
            }
            
            // Pass one of the output pointer is NULL to test the memory error.
            {
                IABBedDefinition* bed = createBedDefinitionElement(0);
                frameSubElements.clear();
                bedChannels = createBedChannels();
                bed->SetBedChannels(bedChannels);
                frameSubElements.push_back(bed);
                iabFrameInterface_->SetSubElements(frameSubElements);
                
                // do backup the pointer, later this will be used to revert back.
                float *pointer_backup = outPointers_[outputChannelCount_ -1];
                
                outPointers_[outputChannelCount_ -1] = NULL;
                
                returnCode = iabRendererinterface_->RenderIABFrame(*iabFrameInterface_, outPointers_, outputChannelCount_, iabFrameSampleCount_, renderedOutputSampleCount);
                ASSERT_TRUE(returnCode == kIABMemoryError);
                
                // put it back.
                outPointers_[outputChannelCount_ -1] = pointer_backup;
                returnCode = iabRendererinterface_->RenderIABFrame(*iabFrameInterface_, outPointers_, outputChannelCount_, iabFrameSampleCount_, renderedOutputSampleCount);
                ASSERT_TRUE(returnCode == kIABNoError);
            }
        }

        // Tests the object definitions rendering
        void Test_ObjectDefinitionRendering()
        {
            createIABFrameWithObjectDefinition();
            
            IABRenderedOutputSampleCountType renderedOutputSampleCount = 0;
            iabError returnCode = kIABNoError;
            
            // Valid frame with bed definition.
            returnCode = iabRendererinterface_->RenderIABFrame(*iabFrameInterface_, outPointers_, outputChannelCount_, iabFrameSampleCount_, renderedOutputSampleCount);
            ASSERT_TRUE(returnCode == kIABNoError);
            
            std::vector<IABChannel*> bedChannels;
            std::vector<IABElement*> frameSubElements;
            
            // Pass output pointers as NULL
            {
                returnCode = iabRendererinterface_->RenderIABFrame(*iabFrameInterface_, NULL, outputChannelCount_, iabFrameSampleCount_, renderedOutputSampleCount);
                ASSERT_TRUE(returnCode == kIABBadArgumentsError);
            }
            
            // Pass output pointers as NULL
            {
                IABObjectDefinition* objectElement = createObjectDefinitionElement(8);
                frameSubElements.clear();
                frameSubElements.push_back(objectElement);
                iabFrameInterface_->SetSubElements(frameSubElements);
                
                returnCode = iabRendererinterface_->RenderIABFrame(*iabFrameInterface_, NULL, outputChannelCount_, iabFrameSampleCount_, renderedOutputSampleCount);
                ASSERT_TRUE(returnCode == kIABBadArgumentsError);
            }
            
            // Pass one of the output pointer is NULL to test the memory error.
            {
                // do backup the pointer, later this will be used to revert back.
                float *pointer_backup = outPointers_[outputChannelCount_ -1];
                
                outPointers_[outputChannelCount_ -1] = NULL;
                
                returnCode = iabRendererinterface_->RenderIABFrame(*iabFrameInterface_, outPointers_, outputChannelCount_, iabFrameSampleCount_, renderedOutputSampleCount);
                ASSERT_TRUE(returnCode == kIABMemoryError);
                
                // put it back.
                outPointers_[outputChannelCount_ -1] = pointer_backup;
                returnCode = iabRendererinterface_->RenderIABFrame(*iabFrameInterface_, outPointers_, outputChannelCount_, iabFrameSampleCount_, renderedOutputSampleCount);
                ASSERT_TRUE(returnCode == kIABNoError);
            }
        }


        // IAB BedDefinition with DLC Data rendering test
        void Test_BedDefinitionWithDLCAudioRendering()
        {
            createIABFrameWithBedDefinitionAndDLCData(5);
            
            IABRenderedOutputSampleCountType renderedOutputSampleCount = 0;
            iabError returnCode = kIABNoError;
            
            // bed channels with dlc data.
            returnCode = iabRendererinterface_->RenderIABFrame(*iabFrameInterface_, outPointers_, outputChannelCount_, iabFrameSampleCount_, renderedOutputSampleCount);
            ASSERT_TRUE(returnCode == kIABNoError);
            
			IABFrameInterface::Delete(iabFrameInterface_);
			iabFrameInterface_ = NULL;
            
            // No DLC encoding test, expecting the dlc parser decoding error..
            {
                createIABFrameWithBedDefinitionAndDLCData(10);
                
				IABAudioDataDLC *dlcAudioElement = dynamic_cast<IABAudioDataDLC*>(IABAudioDataDLCInterface::Create(kIABFrameRate_24FPS, kIABSampleRate_48000Hz));
                dlcAudioElement->SetAudioDataID(10);
                dlcAudioElement->SetDLCSampleRate(kIABSampleRate_48000Hz);
                IABBedDefinition *bedDefinitionElement = createBedDefinitionElement(10);

                
                std::vector<IABElement*> frameSubElements;
                
                frameSubElements.push_back(dlcAudioElement);
                frameSubElements.push_back(bedDefinitionElement);
                
                // Set frame sub elements
                iabFrameInterface_->SetSubElements(frameSubElements);
                
                // bed channels with dlc data.
                returnCode = iabRendererinterface_->RenderIABFrame(*iabFrameInterface_, outPointers_, outputChannelCount_, iabFrameSampleCount_, renderedOutputSampleCount);
                ASSERT_TRUE(returnCode == kIABParserDLCDecodingError);
            }
            
            
            // Test Render Bed channel As Object
            {
                std::vector<IABChannel*> bedChannels;
                std::vector<IABElement*> frameSubElements;
                IABBedDefinition* bedElement = createBedDefinitionElement(0);
                
                bedChannels = createBedChannels();
                
                for (uint32_t i = 0; i < bedChannels.size(); i++)
                {
                    bedChannels.at(i)->SetAudioDataID(10);
                }
                
                IABChannel *channelLh = new IABChannel();
                channelLh->SetChannelID(kIABChannelID_LeftSurround);
                channelLh->SetAudioDataID(10);
                bedChannels.push_back(channelLh);
                
                bedElement->SetBedChannels(bedChannels);
                frameSubElements.push_back(bedElement);
                
                IABAudioDataDLC *dlcAudioElement = createDLCDefinitionElement(kIABFrameRate_24FPS, iabFrameSampleCount_);
                dlcAudioElement->SetAudioDataID(10);
                frameSubElements.push_back(dlcAudioElement);
                
                iabFrameInterface_->SetSubElements(frameSubElements);
                
                returnCode = iabRendererinterface_->RenderIABFrame(*iabFrameInterface_, outPointers_, outputChannelCount_, iabFrameSampleCount_, renderedOutputSampleCount);
                ASSERT_TRUE(returnCode == kIABNoError);
            }
        }

        // IAB BedDefinition with PCM Data rendering test
        void Test_BedDefinitionWithPCMAudioRendering(IABSampleRateType iSampleRate, IABFrameRateType iFrameRate, IABBitDepthType iBitDepth)
        {
            iabFrameSampleCount_ = GetIABNumFrameSamples(iFrameRate, iSampleRate);
            
            {
            
                // Create and set up IAB frame
                iabFrameInterface_ = IABFrameInterface::Create(NULL);
                iabFrameInterface_->SetSampleRate(iSampleRate);
                iabFrameInterface_->SetFrameRate(iFrameRate);
                
                // Create a PCM element and use it for all bed channels
                IABAudioDataIDType audioID = 10;
                IABAudioDataPCM *pcmAudioElement = createPCMDefinitionElement(iSampleRate, iFrameRate, iBitDepth, iabFrameSampleCount_);
                pcmAudioElement->SetAudioDataID(audioID);
                
                // Test 1: Test normal bed rendering with PCM audio data
                IABBedDefinition *bedDefinitionElement = new IABBedDefinition(iFrameRate);
                
                std::vector<IABChannel*> bedChannels = createBedChannels();
                
                for (uint32_t i = 0; i < bedChannels.size(); i++)
                {
                    bedChannels.at(i)->SetAudioDataID(audioID);
                }
                
                bedDefinitionElement->SetConditionalBed(0);
                bedDefinitionElement->SetBedChannels(bedChannels);
                
                std::vector<IABElement*> frameSubElements;
                
                frameSubElements.push_back(bedDefinitionElement);
                frameSubElements.push_back(pcmAudioElement);
                
                // Set frame sub elements
                iabFrameInterface_->SetSubElements(frameSubElements);
                
                IABRenderedOutputSampleCountType renderedOutputSampleCount = 0;
                iabError returnCode = kIABNoError;
                
                // Render IAB frame with bed definition
                returnCode = iabRendererinterface_->RenderIABFrame(*iabFrameInterface_, outPointers_, outputChannelCount_, iabFrameSampleCount_, renderedOutputSampleCount);
                ASSERT_TRUE(returnCode == kIABNoError);
                
                IABFrameInterface::Delete(iabFrameInterface_);
                iabFrameInterface_ = NULL;
            }
            
            // Test 2: Add another bed definition to test rendering bed channel as object
            
            {
            
                // Create and set up IAB frame
                iabFrameInterface_ = IABFrameInterface::Create(NULL);
                iabFrameInterface_->SetSampleRate(iSampleRate);
                iabFrameInterface_->SetFrameRate(iFrameRate);
                
                std::vector<IABElement*> frameSubElements;
                
                // Create a PCM element and use it for all bed channels
                IABAudioDataIDType audioID = 10;
                IABAudioDataPCM *pcmAudioElement = createPCMDefinitionElement(iSampleRate, iFrameRate, iBitDepth, iabFrameSampleCount_);
                pcmAudioElement->SetAudioDataID(audioID);
                
                frameSubElements.push_back(pcmAudioElement);
                
                IABBedDefinition *bedDefinitionElement = new IABBedDefinition(iFrameRate);
                
                std::vector<IABChannel*> bedChannels;
                
                // Add one LS channel. The renderer configuration does not have a LS speaker, so it will be rendered as object
                IABChannel *channelLS = new IABChannel();
                channelLS->SetChannelID(kIABChannelID_LeftSurround);
                channelLS->SetAudioDataID(10);
                bedChannels.push_back(channelLS);
                
                bedDefinitionElement->SetBedChannels(bedChannels);
                
                // Add to frame
                frameSubElements.push_back(bedDefinitionElement);
                
                // Update frame elements
                iabFrameInterface_->SetSubElements(frameSubElements);
                
                IABRenderedOutputSampleCountType renderedOutputSampleCount = 0;
                iabError returnCode = kIABNoError;
                
                // Render IAB frame with bed definitions and one channel that doesn't have a corresponding speaker in the config.
                returnCode = iabRendererinterface_->RenderIABFrame(*iabFrameInterface_, outPointers_, outputChannelCount_, iabFrameSampleCount_, renderedOutputSampleCount);
                ASSERT_TRUE(returnCode == kIABNoError);
                
                IABFrameInterface::Delete(iabFrameInterface_);
                iabFrameInterface_ = NULL;
                
            }
        }

        // IAB rendering test : Object Definition with DLC Data
        void Test_RenderingIABObjectDefinitionWithDLCAudio()
        {
            createIABFrameWithObjectDefinition();
            
            IABRenderedOutputSampleCountType renderedOutputSampleCount = 0;
            iabError returnCode = kIABNoError;
            
            // Test : IAB Object definition with proper dlc audio data with encoding.
            {
                IABObjectDefinition *element = createObjectDefinitionElement(4);
                
                element->SetAudioDataID(10);

                IABAudioDataDLC *dlcAudioElement = createDLCDefinitionElement(kIABFrameRate_24FPS, 2000);
                dlcAudioElement->SetAudioDataID(10);

                std::vector<IABElement*> frameSubElements;
                frameSubElements.push_back(dlcAudioElement);
                frameSubElements.push_back(element);

                // Set frame sub elements
                iabFrameInterface_->SetSubElements(frameSubElements);
                
                // IAB Object with dlc data.
                returnCode = iabRendererinterface_->RenderIABFrame(*iabFrameInterface_, outPointers_, outputChannelCount_, iabFrameSampleCount_, renderedOutputSampleCount);
                ASSERT_TRUE(returnCode == kIABRendererObjectDefinitionError);
            }
            
            // Test : No DLC encoding test, expecting the dlc parser decoding error..
            {
                IABObjectDefinition *objectElement = createObjectDefinitionElement(8);
                
                objectElement->SetAudioDataID(10);
                
                IABAudioDataDLC *dlcAudioElement = createDLCDefinitionElement(kIABFrameRate_24FPS, 0);
                dlcAudioElement->SetAudioDataID(10);
                dlcAudioElement->SetDLCSampleRate(kIABSampleRate_48000Hz);
                
                
                std::vector<IABElement*> frameSubElements;
                
                frameSubElements.push_back(dlcAudioElement);
                frameSubElements.push_back(objectElement);
                
                // Set frame sub elements
                iabFrameInterface_->SetSubElements(frameSubElements);
                
                // bed channels with dlc data.
                returnCode = iabRendererinterface_->RenderIABFrame(*iabFrameInterface_, outPointers_, outputChannelCount_, iabFrameSampleCount_, renderedOutputSampleCount);
                ASSERT_TRUE(returnCode == kIABParserDLCDecodingError);
            }
            
        }

        // IAB rendering test : Object Definition with PCM Data
        void Test_RenderingIABObjectDefinitionWithPCMAudio(IABSampleRateType iSampleRate, IABFrameRateType iFrameRate, IABBitDepthType iBitDepth)
        {
            iabFrameSampleCount_ = GetIABNumFrameSamples(iFrameRate, iSampleRate);
            
            // Create and set up IAB frame
            iabFrameInterface_ = IABFrameInterface::Create(NULL);
            iabFrameInterface_->SetSampleRate(iSampleRate);
            iabFrameInterface_->SetFrameRate(iFrameRate);
            
            // Create a PCM element and use it for all bed channels
            IABAudioDataIDType audioID = 10;
            IABAudioDataPCM *pcmAudioElement = createPCMDefinitionElement(iSampleRate, iFrameRate, iBitDepth, iabFrameSampleCount_);
            pcmAudioElement->SetAudioDataID(audioID);
            
            std::vector<IABElement*> frameSubElements;
            frameSubElements.push_back(pcmAudioElement);
            
            IABObjectDefinition *objectDefinitionElement = new IABObjectDefinition(iFrameRate);
            
            objectDefinitionElement->SetMetadataID(1);
            objectDefinitionElement->SetAudioDataID(10);
            
            uint8_t numPanSubBlocks = GetIABNumSubBlocks(iFrameRate);
            
            std::vector<IABObjectSubBlock*> panSubBlocks;
            
            for (uint8_t i = 0; i < numPanSubBlocks; i++)
            {
                IABObjectSubBlock *subBlock = new IABObjectSubBlock();
                subBlock->SetPanInfoExists(1);
                
                // Add to object
                panSubBlocks.push_back(subBlock);
            }
            
            objectDefinitionElement->SetPanSubBlocks(panSubBlocks);

            frameSubElements.push_back(objectDefinitionElement);
            
            // Set frame sub elements
            iabFrameInterface_->SetSubElements(frameSubElements);
            
            IABRenderedOutputSampleCountType renderedOutputSampleCount = 0;
            iabError returnCode = kIABNoError;
            
            // IAB Object with dlc data.
            returnCode = iabRendererinterface_->RenderIABFrame(*iabFrameInterface_, outPointers_, outputChannelCount_, iabFrameSampleCount_, renderedOutputSampleCount);
            ASSERT_TRUE(returnCode == kIABNoError);
        }

        // Bad input argument test.
        void Test_RenderIABFrameAPIBadArgument()
        {
            createIABFrameWithBedDefinition();
            
            IABRenderedOutputSampleCountType renderedOutputSampleCount = 0;
            iabError returnCode = kIABNoError;
            
            // send output channel count as 0 and expect error.
            returnCode = iabRendererinterface_->RenderIABFrame(*iabFrameInterface_, outPointers_, 0, iabFrameSampleCount_, renderedOutputSampleCount);
            ASSERT_TRUE(returnCode == kIABBadArgumentsError);
            
            // put back actual valid output channel count and expect No error.
            returnCode = iabRendererinterface_->RenderIABFrame(*iabFrameInterface_, outPointers_, outputChannelCount_, iabFrameSampleCount_, renderedOutputSampleCount);
            ASSERT_TRUE(returnCode == kIABNoError);
            
            
            // send IAB Frame sample count as 0 and expect error.
            returnCode = iabRendererinterface_->RenderIABFrame(*iabFrameInterface_, outPointers_, 0, iabFrameSampleCount_, renderedOutputSampleCount);
            ASSERT_TRUE(returnCode == kIABBadArgumentsError);
            
            // put back actual valid IAB Frame sample count and expect No error.
            returnCode = iabRendererinterface_->RenderIABFrame(*iabFrameInterface_, outPointers_, outputChannelCount_, iabFrameSampleCount_, renderedOutputSampleCount);
            ASSERT_TRUE(returnCode == kIABNoError);
            
        }
        
        // Test rendering a frame with no sub-element.
        void Test_RenderingFrameWithNoSubElement()
        {
            IABRenderedOutputSampleCountType renderedOutputSampleCount = 0;
            iabError returnCode = kIABNoError;
            
            std::vector<IABElement*> frameSubElements;
            
            // Set empty sub-elment vector
            iabFrameInterface_->SetSubElements(frameSubElements);
            
            returnCode = iabRendererinterface_->RenderIABFrame(*iabFrameInterface_, outPointers_, outputChannelCount_, iabFrameSampleCount_, renderedOutputSampleCount);
            ASSERT_TRUE(returnCode == kIABNoError);
            
        }
            
        // Tests the object definition rendering with authoring tool element.
        void Test_ObjectRenderingWithAuthoringTool()
        {
            // Create IAB frame with Object defintion only
            iabFrameInterface_ = IABFrameInterface::Create(NULL);
            ASSERT_TRUE(iabFrameInterface_ != NULL);
            
            // Set audio sampling rate
            IABSampleRateType sampleRate = kIABSampleRate_48000Hz;
            EXPECT_EQ(kIABNoError, iabFrameInterface_->SetSampleRate(sampleRate));
            
            // Set Frame rate
            IABFrameRateType frameRate = kIABFrameRate_24FPS;
            EXPECT_EQ(kIABNoError, iabFrameInterface_->SetFrameRate(frameRate));
            
            iabFrameSampleCount_ = GetIABNumFrameSamples(frameRate, sampleRate);
            
            // Create DLC element for the object
            IABAudioDataDLC *dlcAudioElement = createDLCDefinitionElement(kIABFrameRate_24FPS, 2000);
            EXPECT_EQ(kIABNoError, dlcAudioElement->SetAudioDataID(1));
            
            // Create object definition element
            IABObjectDefinition *objectDefinition = createObjectDefinitionElement(8);
            ASSERT_TRUE(objectDefinition != NULL);
            EXPECT_EQ(kIABNoError, objectDefinition->SetMetadataID(1));
            EXPECT_EQ(kIABNoError, objectDefinition->SetAudioDataID(1));

            // Create authoring tool element
            IABAuthoringToolInfo *authoringTool = new IABAuthoringToolInfo();
            ASSERT_TRUE(authoringTool != NULL);
            
            authoringTool->SetAuthoringToolInfo("Authoring tool info for IAB unit test");
            
            IABRenderedOutputSampleCountType renderedOutputSampleCount = 0;
            
            std::vector<IABElement*> frameSubElements;
            frameSubElements.push_back(authoringTool);
            frameSubElements.push_back(objectDefinition);
            frameSubElements.push_back(dlcAudioElement);
            
            // Set frame sub elements
            iabFrameInterface_->SetSubElements(frameSubElements);
            
            // Test frame rendering
            EXPECT_EQ(kIABNoError, iabRendererinterface_->RenderIABFrame(*iabFrameInterface_, outPointers_, outputChannelCount_, iabFrameSampleCount_, renderedOutputSampleCount));
            
        }
            
        // Tests the object definition rendering with user data element.
        void Test_ObjectRenderingWithUserData()
        {
            // Create IAB frame with Object defintion only
            iabFrameInterface_ = IABFrameInterface::Create(NULL);
            ASSERT_TRUE(iabFrameInterface_ != NULL);
            
            // Set audio sampling rate
            IABSampleRateType sampleRate = kIABSampleRate_48000Hz;
            EXPECT_EQ(kIABNoError, iabFrameInterface_->SetSampleRate(sampleRate));
            
            // Set Frame rate
            IABFrameRateType frameRate = kIABFrameRate_24FPS;
            EXPECT_EQ(kIABNoError, iabFrameInterface_->SetFrameRate(frameRate));
            
            iabFrameSampleCount_ = GetIABNumFrameSamples(frameRate, sampleRate);
            
            // Create DLC element for the object
            IABAudioDataDLC *dlcAudioElement = createDLCDefinitionElement(kIABFrameRate_24FPS, 2000);
            EXPECT_EQ(kIABNoError, dlcAudioElement->SetAudioDataID(1));
            
            // Create object definition element
            IABObjectDefinition *objectDefinition = createObjectDefinitionElement(8);
            ASSERT_TRUE(objectDefinition != NULL);
            EXPECT_EQ(kIABNoError, objectDefinition->SetMetadataID(1));
            EXPECT_EQ(kIABNoError, objectDefinition->SetAudioDataID(1));
            
            // Create user data element with a made-up user ID and some test data
            IABUserData *userDataElement = new IABUserData();
            ASSERT_TRUE(userDataElement != NULL);
            std::vector<uint8_t> userData;
            
            // Add some data bytes to user data
            for (uint32_t i = 0; i < 32; i++)
            {
                userData.push_back(i);
            }

			uint8_t userID[16] = { 0x06,0x0E,0x2B,0x34,0x04,0x01,0x01,0x0D,0x0F,0x01,0x02,0x03,0x04,0x05,0x06,0x07 };
			userDataElement->SetUserID(userID);
            userDataElement->SetUserDataBytes(userData);
            
            IABRenderedOutputSampleCountType renderedOutputSampleCount = 0;
            
            std::vector<IABElement*> frameSubElements;
            frameSubElements.push_back(userDataElement);
            frameSubElements.push_back(objectDefinition);
            frameSubElements.push_back(dlcAudioElement);
            
            // Set frame sub elements
            iabFrameInterface_->SetSubElements(frameSubElements);
            
            // Test frame rendering
            EXPECT_EQ(kIABNoError, iabRendererinterface_->RenderIABFrame(*iabFrameInterface_, outPointers_, outputChannelCount_, iabFrameSampleCount_, renderedOutputSampleCount));
            
        }
        
    private:
        
        // Rendering related
        IABRendererInterface                *iabRendererinterface_;     // Pointer to IAB renderer
        RenderUtils::IRendererConfiguration *rendererConfig_;           // Pointer to the renderer configuration file
        IABRenderedOutputChannelCountType   outputChannelCount_;        // Number of rendered output channel
        IABFrameInterface                   *iabFrameInterface_;
        
        // Maximum number of samples per channel supported by the IAB Renderer. It is used for memory allocation purpose to ensure
        // Test shoud allocate enough buffer to handle the maximum size.
        // Actual number of rendered samples depends on frame rate and sample rate of the parsed IAB frame and will be
        // less than or equal to the maximum value. See iabFrameSampleCount_ below.
        IABRenderedOutputSampleCountType    maxOutputSampleCount_;
        
        // Number of samples per rendered output channel for current frame, derived from frame rate and sample rate of current parsed IAB frame
        // In practice, the entire bitstream is expected to have fixed sample rate and frame rate, so frame sample count should stay
        // the same throughtout
        IABRenderedOutputSampleCountType    iabFrameSampleCount_;
        
        // Stores channel labels from renderer configuration file. Each label is used to construct a name for the corresponding wav file.
        std::vector<std::string>            channelList_;
        
        // wav file index/rendered output channel index map
        // Map key is the wav
        std::map<int32_t, int32_t>          speakerRendererOutputMap_;
        
        // Rendered output buffer. This buffer block contains all rendered output channels, arranged in a channel by channel basis, i.e.
        // non-interleaving. Channel order is same as order in oWavFiles_
        float                   *outBuffer_;
        
        // Array of output buffer pointers. Each pointer points to the start of a rendered output channel.
        // Channel order is same as order in oWavFiles_
        float                   **outPointers_;

    };
    
    
    TEST_F(IABRenderFrame_Test, Test_BedDefinitionRendering)
    {
        Test_BedDefinitionRendering();
    }

    TEST_F(IABRenderFrame_Test, Test_ObjectDefinitionRendering)
    {
        Test_ObjectDefinitionRendering();
    }

    TEST_F(IABRenderFrame_Test, Test_BedDefinitionWithDLCDataRendering)
    {
        Test_BedDefinitionWithDLCAudioRendering();
    }

    TEST_F(IABRenderFrame_Test, Test_BedDefinitionWithPCMDataRendering_24fps)
    {
        // Test 48kHz, 24-bit PCM only
        Test_BedDefinitionWithPCMAudioRendering(kIABSampleRate_48000Hz, kIABFrameRate_24FPS, kIABBitDepth_24Bit);
    }
    
    TEST_F(IABRenderFrame_Test, Test_BedDefinitionWithPCMDataRendering_23_976fps)
    {
        // Test 48kHz, 24-bit PCM only
        Test_BedDefinitionWithPCMAudioRendering(kIABSampleRate_48000Hz, kIABFrameRate_23_976FPS, kIABBitDepth_24Bit);
    }

    TEST_F(IABRenderFrame_Test, Test_RenderingIABObjectDefinitionWithDLCAudio)
    {
        Test_RenderingIABObjectDefinitionWithDLCAudio();
    }

    TEST_F(IABRenderFrame_Test, Test_RenderingIABObjectDefinitionWithPCMAudio_24fps)
    {
        // Test 48kHz, 24-bit PCM only
        Test_RenderingIABObjectDefinitionWithPCMAudio(kIABSampleRate_48000Hz, kIABFrameRate_24FPS, kIABBitDepth_24Bit);
    }
    
    TEST_F(IABRenderFrame_Test, Test_RenderingIABObjectDefinitionWithPCMAudio_23_976fps)
    {
        // Test 48kHz, 24-bit PCM only
        Test_RenderingIABObjectDefinitionWithPCMAudio(kIABSampleRate_48000Hz, kIABFrameRate_23_976FPS, kIABBitDepth_24Bit);
    }

    TEST_F(IABRenderFrame_Test, Test_RenderIABFrameAPIArgument)
    {
        Test_RenderIABFrameAPIBadArgument();
        Test_RenderingFrameWithNoSubElement();
    }

    TEST_F(IABRenderFrame_Test, Test_ObjectRenderingWithAuthoringTool)
    {
        Test_ObjectRenderingWithAuthoringTool();
    }

    TEST_F(IABRenderFrame_Test, Test_ObjectRenderingWithUserData)
    {
        Test_ObjectRenderingWithUserData();
    }
}   // namespace
