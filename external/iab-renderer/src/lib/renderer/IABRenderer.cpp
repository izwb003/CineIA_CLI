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

/**
 * IABRenderer.cpp
 *
 * @file
 */

#include <assert.h>
#include <stack>
#include <vector>
#include <stdlib.h>
#include <math.h>

#include "IABDataTypes.h"
#include "common/IABElements.h"
#include "IABRenderer.h"
#include "IABVersion.h"
#include "renderer/IABTransform/IABTransform.h"
#include "IABUtilities.h"
#include "renderer/IABInterior/IABInterior.h"

#include "renderer/VBAPRenderer/VBAPRenderer.h"

// Platform
#ifdef __APPLE__
#define USE_MAC_ACCELERATE
#endif

#ifdef USE_MAC_ACCELERATE
#include "coreutils/VectDSPMacAccelerate.h"
#else
#include "coreutils/VectDSP.h"
#endif

// Uncomment this line to enable IABRenderer to issue errors or warning to std::err
//#define ENABLE_RENDERER_ERROR_REPORTING

namespace SMPTE
{
namespace ImmersiveAudioBitstream
{
	static const int32_t kIABDecorrTailingFrames = 2;

    /****************************************************************************
    *
    * IABRenderer
    *
    *****************************************************************************/

    // Create IABRenderer instance
    IABRendererInterface* IABRendererInterface::Create(RenderUtils::IRendererConfiguration &iConfig)
    {
        IABRenderer* iabRenderer = NULL;
        iabRenderer = new IABRenderer(iConfig);
        return iabRenderer;
    }
    
    // Deletes an IABRenderer instance
    void IABRendererInterface::Delete(IABRendererInterface* iInstance)
    {
        delete iInstance;
    }

	// Constructor
	IABRenderer::IABRenderer(RenderUtils::IRendererConfiguration &iConfig)
    {
		targetUseCase_ = kIABUseCase_NoUseCase;
        numRendererOutputChannels_ = 0;
        numSamplePerRendererOutputChannel_ = 0;     // This is likely one subblock long and will depend on the bitstream frame rate, will be updated later
		parentMetaID_ = 0;
        vbapObject_ = NULL;
        sampleBufferInt_ = NULL;
        sampleBufferFloat_ = NULL;
		render96kTo48k_ = true;						// Default to true for SDK v1.x

		enableSmoothing_ = true;					// Sample smoothing is enabled by default
		enableDecorrelation_ = true;				// Object decorrelation per stream metadata is enabled by default

		enableFrameGainsCache_ = true;				// Frame-wise cache is always enabled for public clients for processing optimization

		decorrOutputBuffer_ = NULL;
		decorrOutputChannelPointers_ = NULL;

		// Set up VBAPRender, GainProcessor, output channel map according to "iConfig".
		SetUp(iConfig);
    }

	// Internal-dev Constructor
	// Adding option to disable/enable between-frame gains cache.
	IABRenderer::IABRenderer(RenderUtils::IRendererConfiguration &iConfig, bool iFrameGainsCacheEnable)
	{
		targetUseCase_ = kIABUseCase_NoUseCase;
		numRendererOutputChannels_ = 0;
		numSamplePerRendererOutputChannel_ = 0;     // This is likely one subblock long and will depend on the bitstream frame rate, will be updated later
		parentMetaID_ = 0;
		vbapObject_ = NULL;
        iabObjectZone9_ = NULL;
		sampleBufferInt_ = NULL;
		sampleBufferFloat_ = NULL;
		render96kTo48k_ = true;						// Default to true for SDK v1.x

		enableSmoothing_ = true;					// Sample smoothing is enabled by default
		enableDecorrelation_ = true;				// Object decorrelation per stream metadata is enabled by default

		enableFrameGainsCache_ = iFrameGainsCacheEnable;	// Frame-wise cache enable set based on caller input

		decorrOutputBuffer_ = NULL;
		decorrOutputChannelPointers_ = NULL;

		// Set up VBAPRender, GainProcessor, output channel map according to "iConfig".
		SetUp(iConfig);
	}

	// Destructor
	IABRenderer::~IABRenderer()
    {
        if (vbapRenderer_)
        {
            delete vbapRenderer_;
        }
        
		if (channelGainsProcessor_)
		{
			delete channelGainsProcessor_;
		}
        
        if (vbapObject_)
        {
            delete vbapObject_;
        }

		if (iabDecorrelation_)
		{
			delete iabDecorrelation_;
		}

        if (iabObjectZone9_)
        {
            delete iabObjectZone9_;
        }

		if (vectDSP_)
		{
			delete vectDSP_;
		}
        
        delete [] outputBufferPointers_;
        delete [] sampleBufferInt_;
        delete [] sampleBufferFloat_;

		delete[] decorrOutputBuffer_;
		delete[] decorrOutputChannelPointers_;
	}

	// IABRenderer::SetUp() implementation
	void IABRenderer::SetUp(RenderUtils::IRendererConfiguration &iConfig)
	{
        // ===========================================================
        // Set up IAB renderer with passed in renderer configuration

		// Get smooth and IAB decorr enable flag values from configuration
		iConfig.GetSmooth(enableSmoothing_);
		iConfig.GetIABDecorrEnable(enableDecorrelation_);

        uint32_t physicalSpeakerCount = 0;
        
        // Get speaker list from configuration
        iConfig.GetPhysicalSpeakers(physicalSpeakerList_);
        physicalSpeakerCount = static_cast<uint32_t>(physicalSpeakerList_->size());

		// In addition, get total speaker list, including virtual speakers
		// This is a list from original config file parser lib, in comparison with the "physicalSpeakerList_",
		// which was later added to contain physical speakers only 
		iConfig.GetSpeakers(totalSpeakerList_);

        // Total speaker count includes both virtual and physical speakers.
        // IAB configuration files now support virtual speakers
        
        iConfig.GetTotalSpeakerCount(speakerCount_);
        
        if ((physicalSpeakerCount == 0) || (speakerCount_ == 0))
        {
            // Invalid speaker count in the configuration
            return;
        }
        
        numRendererOutputChannels_ = static_cast<IABRenderedOutputChannelCountType>(physicalSpeakerCount);
        
        // Get target soundfield from configuration
		iConfig.GetTargetSoundfield(targetSoundfield_);
        
        // Derive target use case from soundfield
		targetUseCase_ = IABConfigTables::soundfieldToIABUseCaseMap[targetSoundfield_];

		// Get URI'ed physical speaker to output index map
        physicalURIedBedSpeakerOutputIndexMap_ = iConfig.GetSpeakerURIToOutputIndexMap();

		// Also get all URI'ed speakers to "old/original-speaker index" map
		// Note the map values are "old/original-speaker index", refering index in "" list
		totalURIedSpeakerToTotalSpeakerListMap_ = iConfig.GetSpeakerURIToSpeakerIndexMap();

		// Need to map speaker index to actual output buffer index
		speakerIndexToOutputIndexMap_ = iConfig.GetSpeakerChannelToOutputIndexMap();
        
		numPanSubBlocks_ = 0;
		frameSampleCount_ = 0;

		// VBAP and Gain Processor instantiation
		vbapRenderer_ = new IABVBAP::VBAPRenderer();
		channelGainsProcessor_ = new IABGAINSPROC::ChannelGainsProcessor();
		vbapRenderer_->InitWithConfig(&iConfig);

		// Pre-allocate buffers to avoid new memory allocation when rendering a frame:
        vbapObject_ = new IABVBAP::vbapRendererObject(numRendererOutputChannels_);
        outputBufferPointers_ = new IABSampleType*[numRendererOutputChannels_];

		sampleBufferInt_ = new int32_t[kIABMaxFrameSampleCount];
		sampleBufferFloat_ = new IABSampleType[kIABMaxFrameSampleCount];

		// Allocate decorrelation buffers
		// The buffer is used to hold all rendered samples that are to be decorr-processed.
		// Decorr is performed right before passing frame rendered output back to caller in 
		// RenderIABFrame() call.
		//
		decorrOutputBuffer_ = new IABSampleType[numRendererOutputChannels_ * kIABMaxFrameSampleCount];
		decorrOutputChannelPointers_ = new IABSampleType*[numRendererOutputChannels_];

		// Set up decorr output buffer pointer array
		for (uint32_t i = 0; i < numRendererOutputChannels_; i++)
		{
			decorrOutputChannelPointers_[i] = decorrOutputBuffer_ + i * kIABMaxFrameSampleCount;
		}

		// Create an IABDecorrelation instance
		//
		iabDecorrelation_ = new IABDecorrelation();
		
		// Set up iabDecorrelation_ based on iConfig
		// Note, no error checking is performed here, as there is no upward error return path for 
		// this function, "void IABRenderer::SetUp()".
		//
		iabDecorrelation_->Setup(iConfig);

		// Reset iabDecorrelation_ to ensure initialization of all delay buffers.
		iabDecorrelation_->Reset();
		decorrelationInReset_ = true;

		// Init decorrTailingFramesCount_ to 0.
		decorrTailingFramesCount_ = 0;

		// Create an VectDSP acceleration engine instance.
		// This is used to sum up coherent output with decorr output samples.
		//
#ifdef USE_MAC_ACCELERATE
		vectDSP_ = new CoreUtils::VectDSPMacAccelerate(kIABMaxFrameSampleCount);
#else
		vectDSP_ = new CoreUtils::VectDSP();
#endif

        // Convert config file speaker VBAP coordinates to IAB unit cube coordinates to support object snapping
        // Converted coordinates are stored in vbapSpeakerChannelIABPositionMap_
        //
        CoreUtils::Vector3 speakerPos;
        CartesianPosInUnitCube IABPosition;
        IABValueX speakerIABPosX = 0.0f;
        IABValueX speakerIABPosY = 0.0f;
        IABValueX speakerIABPosZ = 0.0f;
        IABTransform iabTransform;
        iabError iabReturnCode = kIABNoError;
        
        std::vector<RenderUtils::RenderSpeaker>::const_iterator iter;
        
        // Process the config file speaker list and create a map for snap tolerance comparison
        // Only speakers in VBAP patches are added to the list.
        // Map key is the output channel index and map value is the converted speaker IAB coordinates
        //
        for (iter = (*physicalSpeakerList_).begin(); iter != (*physicalSpeakerList_).end(); iter++)
        {
            if (!iConfig.IsVBAPSpeaker(iter->getName()))
            {
                continue;
            }
                
            if (iter->uri_ != "urn:smpte:ul:060E2B34.0401010D.03020104.00000000")
            {
                // Bypass LFE channel
                speakerPos = iter->getPosition();
                
                // Convert speaker VBAP coordinate to IAB coordinate
                iabReturnCode = iabTransform.TransformCartesianVBAPToIAB(speakerPos.x, speakerPos.y, speakerPos.z,
                                                                         speakerIABPosX, speakerIABPosY, speakerIABPosZ);
                
                if (kIABNoError != iabReturnCode)
                {
                    vbapSpeakerChannelIABPositionMap_.clear();
                    break;
                }
                
                iabReturnCode = IABPosition.setIABObjectPosition(speakerIABPosX, speakerIABPosY, speakerIABPosZ);
                
                if (kIABNoError != iabReturnCode)
                {
                    vbapSpeakerChannelIABPositionMap_.clear();
                    break;
                }
                
                vbapSpeakerChannelIABPositionMap_[iter->outputIndex_] = IABPosition;
            }
        }
        
        // Set up object zone 9
        iabObjectZone9_ = new IABObjectZone9(iConfig);

#ifdef ENABLE_RENDERER_ERROR_REPORTING
        
        if (!iabObjectZone9_ || !iabObjectZone9_->isInitialised())
        {
            std::cerr << "Warning: Unable to properly initialise object zone 9 with the renderer configuration.\n";
            std::cerr << "Object zone support disabled for target configuration.\n";
        }
        
#endif        

	}

	// IABRenderer::GetOutputChannelCount() implementation
	IABRenderedOutputChannelCountType IABRenderer::GetOutputChannelCount() const
    {
        return numRendererOutputChannels_;
    }
    
	// IABRenderer::GetMaxOutputSampleCount() implementation
	IABRenderedOutputSampleCountType IABRenderer::GetMaxOutputSampleCount() const
    {
        return kIABMaxFrameSampleCount;
    }

    // Methods for rendering an IAB element of specified type    
    //
    
	// IABRenderer::RenderIABFrame() implementation
	iabError IABRenderer::RenderIABFrame(const IABFrameInterface& iIABFrame
                                         , IABSampleType **oOutputChannels
                                         , IABRenderedOutputChannelCountType iOutputChannelCount
                                         , IABRenderedOutputSampleCountType iOutputSampleBufferCount
                                         , IABRenderedOutputSampleCountType &oRenderedOutputSampleCount)
    {
        iabError iabReturnCode = kIABNoError;

        // Clear warnings
        warnings_.clear();
        
		// Get frame rate of iIABFrame
        iIABFrame.GetFrameRate(frameRate_);

		// Get sample rate of iIABFrame
		iIABFrame.GetSampleRate(sampleRate_);
        
		// Report error is the frame sample rate is anything other than 48kHz or 96kHz
		if ( (sampleRate_ != kIABSampleRate_48000Hz) && (sampleRate_ != kIABSampleRate_96000Hz) )
        {
            return kIABRendererUnsupportedSampleRateError;
        }

		// Check to see if frame rate and sample rate combination is supported for rendering
        if (!IsSupported(frameRate_, sampleRate_))
        {
            // Not a valid combination, report error
            return kIABRendererUnsupportedFrameRateError;
        }

		// Get frame sample count corresponding to the frame rate and sample rate 
		frameSampleCount_ = GetIABNumFrameSamples(frameRate_, sampleRate_);

		// For 96k, and when render96kTo48k_ is true ..
		if (sampleRate_ == kIABSampleRate_96000Hz && render96kTo48k_)
		{
			frameSampleCount_ >>= 1;
		}
        
		// Get number of sub-blocks corresponding to the frame rate 
		numPanSubBlocks_ = GetIABNumSubBlocks(frameRate_);
        
		// Report error if any, or combination, of the frame parameters breaches bounds
		if ((frameSampleCount_ == 0) || (frameSampleCount_ > kIABMaxFrameSampleCount) ||
            (numPanSubBlocks_ == 0))
        {
            return kIABGeneralError;
        }

		// Set up subBlockSampleCount_ and subBlockSampleStartOffset_ for the frame. 
		// These are used for sub-block processing of ObjectDefinition, BedRemap and Zone19 elements.
		// 
		if (frameRate_ == kIABFrameRate_23_976FPS)
		{
			// Set up 1st elements
			subBlockSampleCount_[0] = kSubblockSize_23_97FPS_48kHz[0];
			subBlockSampleStartOffset_[0] = 0;

			for (uint32_t i = 1; i < numPanSubBlocks_; i++)
			{
				subBlockSampleCount_[i] = kSubblockSize_23_97FPS_48kHz[i];
				subBlockSampleStartOffset_[i] = subBlockSampleStartOffset_[i-1] + kSubblockSize_23_97FPS_48kHz[i-1];
			}
		}
		else
		{
			uint32_t subBlockSampleCount = frameSampleCount_ / numPanSubBlocks_;

			// Set up 1st elements
			subBlockSampleCount_[0] = subBlockSampleCount;
			subBlockSampleStartOffset_[0] = 0;

			for (uint32_t i = 1; i < numPanSubBlocks_; i++)
			{
				subBlockSampleCount_[i] = subBlockSampleCount;
				subBlockSampleStartOffset_[i] = subBlockSampleStartOffset_[i-1] + subBlockSampleCount;
			}
		}
			
        // Update sample per channel count
        numSamplePerRendererOutputChannel_ = frameSampleCount_;
                
        // Check input parameters
        if ((iOutputChannelCount != numRendererOutputChannels_) ||
            (iOutputSampleBufferCount != numSamplePerRendererOutputChannel_) ||
            (oOutputChannels == NULL))
        {
            return kIABBadArgumentsError;
        }

        // Check output buffer pointers for null and clear buffers
        for (uint32_t i = 0; i < iOutputChannelCount; i++)
        {
            if (!oOutputChannels[i])
            {
                return kIABMemoryError;
            }
            
            // Reset output buffer samples
            memset(oOutputChannels[i], 0, sizeof(IABSampleType) * iOutputSampleBufferCount);
        }

		// Initialize decorr output sample buffers (all channels) before any rendering
		memset(decorrOutputBuffer_, 0, sizeof(IABSampleType) * kIABMaxFrameSampleCount * numRendererOutputChannels_);

		// Initialise to zero and set to correct value when rendering completes without error
        oRenderedOutputSampleCount = 0;
		// Use this to check total samples rendered, returned by the VBAP renderer
        IABRenderedOutputSampleCountType returnedSampleCount = 0;

		// Update gains cache at beginning of rendering an IAB Frame
		// (For Internal Dev, option to clear/delete all stored cache)
		if (enableFrameGainsCache_)
		{
			// Update VBAPRenderer extendedsource cache
			//
			vbapRenderer_->CleanupPreviouslyRendered();

			// Also update past gains history for smoothing processing
			//
			channelGainsProcessor_->UpdateGainsHistory();
		}
		else
		{
			// Clear VBAPRenderer extendedsource cache
			//
			vbapRenderer_->ResetPreviouslyRendered();

			// Also clear past gains history for smoothing processing
			//
			channelGainsProcessor_->ResetGainsHistory();
		}

        // Save input IAB frame
        iabFrameToRender_ = dynamic_cast<const IABFrame*>(&iIABFrame);
        
        // Get sub-element from the IAB frame
        std::vector<IABElement*> frameSubElements;
        iIABFrame.GetSubElements(frameSubElements);
		IABElementCountType subElementCount = 0;
		iabFrameToRender_->GetSubElementCount(subElementCount);

		if ((0 == subElementCount) || (0 == frameSubElements.size()))
		{
			// No element in this frame, so nothing to render.
            // The renderer output buffer has already been cleared, return a silent output frame to the client.
            oRenderedOutputSampleCount = iOutputSampleBufferCount;
            return kIABNoError;
		}

		// *** Render each of the sub-elements as necessary
		IABElementIDType elementID;

		// Init frame hasDecorrObjects_ to false.
		hasDecorrObjects_ = false;

		for (IABElementCountType i = 0; i < subElementCount; i++)
		{
			frameSubElements[i]->GetElementID(elementID);

			if (kIABElementID_ObjectDefinition == elementID)
			{
				const IABObjectDefinitionInterface* elementToRender = 
					dynamic_cast<const IABObjectDefinitionInterface*>(frameSubElements[i]);

				// Adding object decorr support for binary On/Off mode only.
				// - "No decorr": Off (ObjectDecorCoefPrefix = 0x0)
				// - "Max decorr" : On (ObjectDecorCoefPrefix = 0x1)
				// - Not supported : (ObjectDecorCoefPrefix� = 0x2), effectively "No decorr"
				//
				// Dev Note: Supported decorr time resolution is at Frame level only. Decorr On/Off is based 
				// on decorr parameter from 1st object panblock in the frame, which always exist.
				// (Decor changes at sub-frame/post-1st-pan-block level is not supported.)
				//

				// Get object Pan Blocks
				std::vector<IABObjectSubBlock*> objectPanSubBlocks;
				elementToRender->GetPanSubBlocks(objectPanSubBlocks);

				// Check decorr coefficient in the 1st pan block objectPanSubBlocks[0]
				//
				IABDecorCoeff decorCoeff1stPanBlock;
				objectPanSubBlocks[0]->GetDecorCoef(decorCoeff1stPanBlock);

				// Decorr is to be processed only when (ObjectDecorCoefPrefix = kIABDecorCoeffPrefix_MaxDecor)
				// For all other cases, no decorr.
				//
				// Adding "enable/disable" object decorrelation through "enableDecorrelation_"
				// flag, which in turn is controlled through the render configuration "c decorr 1|0" command
				//   - enableDecorrelation_ flag persists through all frames of a program
				//   - Gate with an additional condition
				//
				if ( enableDecorrelation_ &&
					(decorCoeff1stPanBlock.decorCoefPrefix_ == kIABDecorCoeffPrefix_MaxDecor) )
				{
					// Decorr == ON, ie. Maximum decorrelation.
					// Route rendered output to Decorr output. Decorr to be processed after rendering of all 
					// frame sub-elements.
					//
					iabReturnCode = RenderIABObject(*elementToRender
                                                   , decorrOutputChannelPointers_
                                                   , numRendererOutputChannels_
                                                   , numSamplePerRendererOutputChannel_
                                                   , returnedSampleCount);

					// Set flag hasDecorrObjects_ to true, to indicate presence of decorr object(s).
					hasDecorrObjects_ = true;
				}
				else
				{
					// For all other cases, Decorr == OFF.
					// Route rendered output as normal.
					//
					iabReturnCode = RenderIABObject(*elementToRender
                                                   , oOutputChannels
                                                   , iOutputChannelCount
                                                   , numSamplePerRendererOutputChannel_
                                                   , returnedSampleCount);
				}

                if (kIABNoError != iabReturnCode)
                {
                    return iabReturnCode;
                }
                
                if (returnedSampleCount != numSamplePerRendererOutputChannel_)
                {
                    return kIABRendererObjectDefinitionError;
                }
			}
			else if (kIABElementID_BedDefinition == elementID)
			{
				const IABBedDefinitionInterface* elementToRender = 
					dynamic_cast<const IABBedDefinitionInterface*>(frameSubElements[i]);

				iabReturnCode = RenderIABBed(*elementToRender
                                         , oOutputChannels
                                         , iOutputChannelCount
                                         , numSamplePerRendererOutputChannel_
                                         , returnedSampleCount);
                
                if (kIABNoError != iabReturnCode)
                {
                    return iabReturnCode;
                }
                
                if (returnedSampleCount != numSamplePerRendererOutputChannel_)
                {
                    return kIABRendererBedDefinitionError;
                }
			}
		}

		// **** Process object decorrelation here
		//

		// Does the frame contain decorr objects?
		//
		if (hasDecorrObjects_)
		{
			// If yes, set decorrTailingFramesCount_ to kIABDecorrTrailingFrames (2), resulting in at least 2 more frames
			// of decorr processing. with at lease 1 frame tailing off (hysteresis).
			decorrTailingFramesCount_ = kIABDecorrTailingFrames;
		}
		else;	// If no decorr object, no change to decorrTailingFramesCount_. Let it run its tailing off frames.

		// Decorrelation processing if decorrTailingFramesCount_ > 0;
		//
		if (decorrTailingFramesCount_ > 0)
		{
			// Decorrelate!
			iabDecorrelation_->DecorrelateDecorOutputs(decorrOutputChannelPointers_
                                                       , numRendererOutputChannels_
                                                       , numSamplePerRendererOutputChannel_);

			// Decrement decorrTailingFramesCount_ by 1 (frame)
			decorrTailingFramesCount_--;
			decorrelationInReset_ = false;

			// Adding decorrelated output to total frame output
			for (uint32_t i = 0; i < iOutputChannelCount; i++)
			{
				// Sum up decorrelated output samples to coherent/normal output samples
				vectDSP_->add(oOutputChannels[i]
                             , decorrOutputChannelPointers_[i]
                             , oOutputChannels[i]
                             , numSamplePerRendererOutputChannel_);
			}
		}
		else if (!decorrelationInReset_)
		{
			// Reset iabDecorrelation_
			iabDecorrelation_->Reset();
			decorrelationInReset_ = true;
		}
		else;

		// ** End of decorrelation processing.

        oRenderedOutputSampleCount = numSamplePerRendererOutputChannel_;

        // See if any warnings occurred, and issue them instead of no error
#define NUM_WARNING 2
        static const iabError warningPriority[NUM_WARNING] =
        {
            kIABRendererNoLFEInConfigForBedLFEWarning,
            kIABRendererNoLFEInConfigForRemapLFEWarning,
        };

        for (int i = 0; i < NUM_WARNING; ++i)
        {
            if (warnings_.count(warningPriority[i]) != 0) {
                return warningPriority[i];
            }
        }
        
        return kIABNoError;
    }
    
	// IABRenderer::RenderIABObject() implementation
	iabError IABRenderer::RenderIABObject(const IABObjectDefinitionInterface& iIABObject
                                          , IABSampleType **oOutputChannels
                                          , IABRenderedOutputChannelCountType iOutputChannelCount
                                          , IABRenderedOutputSampleCountType iOutputSampleBufferCount
                                          , IABRenderedOutputSampleCountType &oRenderedOutputSampleCount)
    {
        iabError iabReturnCode = kIABNoError;

        // Check input parameters
        if ((iOutputChannelCount == 0) ||
            (iOutputSampleBufferCount == 0 ) ||
            (iOutputSampleBufferCount != numSamplePerRendererOutputChannel_))
        {
            return kIABBadArgumentsError;
        }
        
        // Conditional Object logic implementation
        //
        
        // Step 1 check: Is this object (itself) activated?
        // If not, exit right away. Otherwise, continue
        if (!IsObjectActivatedForRendering(dynamic_cast<const IABObjectDefinition*>(&iIABObject)))
        {
            oRenderedOutputSampleCount = iOutputSampleBufferCount;
            return kIABNoError;
        }
        
        // Step 2 check: "this" is activated, but is there a sub-lement (if present)
        // that is also activated?
        // If yes, the activated child/sub-element is rendered, replacing the parent
        //

        IABElementCountType numSubElements = 0;
        iIABObject.GetSubElementCount(numSubElements);
        
        // Sub-elements present, check if one of them is activated.
        // (Developer note: ST2098-2 (page 29, section 10.5.1): "... of the child elements of a
        // ObjectDefinition, at most one ObjectDefinition child element is activated."
        //
        if (numSubElements != 0)
        {
            // Get sub-elements
            std::vector<IABElement*> objectSubElements;
            IABElementIDType elementID;
            
            iIABObject.GetSubElements(objectSubElements);
            
            // Loop through sub-elements
            // Check if any of the pointers is a NULL or not a valid sub-element type
            for (std::vector<IABElement*>::iterator iter = objectSubElements.begin(); iter != objectSubElements.end(); iter++)
            {
                if (NULL == *iter)
                {
                    // Early exit if containing NULL
                    return kIABRendererObjectDefinitionError;
                }
                
                // Get and check ID
                (*iter)->GetElementID(elementID);
                
                if (kIABElementID_ObjectDefinition == elementID)
                {
                    // Sub-element of ObjectDefinition type. If this is activated, this should be rendered
                    // in place of parent (this).
                    if ( IsObjectActivatedForRendering(dynamic_cast<IABObjectDefinition*>(*iter)) )
                    {
                        // Render the activated sub-element object instead
                        // (Recursive call)
                        //
                        iabReturnCode = RenderIABObject(*(dynamic_cast<IABObjectDefinitionInterface*>(*iter))
                                                     , oOutputChannels
                                                     , iOutputChannelCount
                                                     , iOutputSampleBufferCount
                                                     , oRenderedOutputSampleCount);
                        
                        // Exit, return error code as-is.
                        return iabReturnCode;
                    }
                }
                else if (kIABElementID_ObjectZoneDefinition19 == elementID)
                {
                    // ObjectZoneDefinition19 is not supported in SDK 1.0
                    // Skip for now
                    continue;
                }
				else ;	// Ignore other types of sub-elements.
            }
        }
        
        // Step check 3. No further check. If conditional object processing flow reaches this
        // point, the object (this) meets all conditions for rendering.
        // Continue to render this object.
        
        IABAudioDataIDType audioDataID;
        IABMetadataIDType objectMetaID;
        
        iIABObject.GetMetadataID(objectMetaID);
        iIABObject.GetAudioDataID(audioDataID);
        
        if (audioDataID == 0)
        {
            // set to same as iOutputSampleBufferCount fo now, so it is not seen as an error by the caller
            oRenderedOutputSampleCount = iOutputSampleBufferCount;
            
            // For IAB, when audioData ID = 0, then the object has no audio for this frame and can be skipped
            return kIABNoError;
        }
        
        // Update working audio sample buffer (sampleBufferFloat_) with object audio samples
        iabReturnCode = UpdateAudioSampleBuffer(audioDataID);
        if (kIABNoError != iabReturnCode)
        {
            // audioData ID not found or no valid sample pointer
            return iabReturnCode;
        }
        
        // Set up VBAP renderer object
		// Note, this VBAP object is to be used for rendering all pan sub blocks of
		// iIABObject.
		//
        
        if (!vbapObject_)
        {
            // vbapObject_ has not been instantiated
            return kIABRendererObjectDefinitionError;
        }
        
        // Reset working object data structure to default state before updating
        ResetVBAPObject();
        
        // Set object ID
        vbapObject_->SetId(static_cast<uint32_t>(objectMetaID));
        
        // Get panblocks
        std::vector<IABObjectSubBlock*> objectPanSubBlocks;
        iIABObject.GetPanSubBlocks(objectPanSubBlocks);
        std::vector<IABObjectSubBlock*>::const_iterator iterPanBlock;

		// Check size of objectPanSubBlocks to be non-zero.
		// Also it must divide into iOutputSampleBufferCount for non-fractional frame rates (i.e. except for 23.97fps)
		//
		uint32_t numSubBlocks = static_cast<uint32_t>(objectPanSubBlocks.size());
		uint32_t subBlockSampleCount = 0;

		if (numSubBlocks != numPanSubBlocks_)
		{
			return kIABRendererObjectDefinitionError;
		}

		oRenderedOutputSampleCount = 0;
        IABRenderedOutputSampleCountType returnedSampleCount = 0;   // Use this to accumulate total samples rendereed

		// Sub block input and output PCM buffer pointers
		IABSampleType *inputAssetSamples = NULL;

        // Render each panblock
        for (uint32_t i = 0; i < numSubBlocks; i++)
        {
			subBlockSampleCount = subBlockSampleCount_[i];

			// Update PCM input and output buffer pointers per sub block index
			inputAssetSamples = sampleBufferFloat_ + subBlockSampleStartOffset_[i];

			for (uint32_t j = 0; j < iOutputChannelCount; j++)
			{
				outputBufferPointers_[j] = oOutputChannels[j] + subBlockSampleStartOffset_[i];
			}
			
			// Render a sub block
			const IABObjectSubBlock* subblockToRender = dynamic_cast<const IABObjectSubBlock*>(objectPanSubBlocks[i]);

			RenderIABObjectSubBlock(*subblockToRender
				, vbapObject_
				, inputAssetSamples
				, outputBufferPointers_
				, iOutputChannelCount
				, subBlockSampleCount
				, returnedSampleCount);

			oRenderedOutputSampleCount += returnedSampleCount;
        }
        
        // Total rendered sample count is expected to be same as iOutputSampleBufferCount
        if (oRenderedOutputSampleCount != iOutputSampleBufferCount)
        {
            return kIABRendererGeneralError;
        }
        
        return kIABNoError;
    }

	// IABRenderer::RenderIABObjectSubBlock() implementation
    iabError IABRenderer::RenderIABObjectSubBlock(const IABObjectSubBlockInterface& iIABObjectSubBlock
                                                  , IABVBAP::vbapRendererObject  *iVbapObject
                                                  , IABSampleType *iAssetSamples
                                                  , IABSampleType **oOutputChannels
                                                  , IABRenderedOutputChannelCountType iOutputChannelCount
                                                  , IABRenderedOutputSampleCountType iOutputSampleBufferCount
                                                  , IABRenderedOutputSampleCountType &oRenderedOutputSampleCount)
    {
        // Check input parameters
        if (!iVbapObject ||
            !iAssetSamples ||
            (iOutputChannelCount == 0) ||
            (iOutputSampleBufferCount == 0 ) )
        {
            return kIABBadArgumentsError;
        }
        
        // Check output buffer pointers for null
        for (uint32_t i = 0; i < iOutputChannelCount; i++)
        {
            if (!oOutputChannels[i])
            {
                return kIABMemoryError;
            }
        }

		// Initialise to 0 and will be set to correct value when rendering completes without error
        oRenderedOutputSampleCount = 0;
        
        iabError iabReturnCode = kIABNoError;
        IABVBAP::vbapError vbapReturnCode = IABVBAP::kVBAPNoError;
		IABGAINSPROC::gainsProcError gainsProceReturnCode = IABGAINSPROC::kGainsProcNoError;

		// Pan Info present for this sub block?
		uint8_t subBlockPanExist = 0;

		// subBlockPanExist is always "1" for first sub block
		// For remaining sub blocks, presence is stream dependent.
		// 
		iIABObjectSubBlock.GetPanInfoExists(subBlockPanExist);

		if (subBlockPanExist)
		{
			// Code below sets up iVbapObject per sub block panning information.
			//
            
            IABGain objectIABGain;
            
            // Retrieve objectIABGain from sub-block
            iIABObjectSubBlock.GetObjectGain(objectIABGain);
            
            // Get the IAB x, y, z positions from the IAB Sub block.
            CartesianPosInUnitCube objectPosition;
            IABValueX iabPosX;
            IABValueY iabPosY;
            IABValueZ iabPosZ;
            iIABObjectSubBlock.GetObjectPositionToUnitCube(objectPosition);
            objectPosition.getIABObjectPosition(iabPosX, iabPosY, iabPosZ);
            
            bool objectHasSpread = false;
            IABObjectSpread objectSpread;
            float spreadXYZ = 0.0f;
            float spreadY = 0.0f;
            float spreadZ = 0.0f;

			// Get spread info
            iIABObjectSubBlock.GetObjectSpread(objectSpread);
            IABSpreadModeType objectSpreadMode = objectSpread.getIABObjectSpreadMode();
			objectSpread.getIABObjectSpread(spreadXYZ, spreadY, spreadZ);

			// Check spread mode and spread values
			if ((objectSpreadMode == kIABSpreadMode_LowResolution_1D)
				|| (objectSpreadMode == kIABSpreadMode_HighResolution_1D))
			{
				// 1D spread mode, check spreadXYZ only 
                if (spreadXYZ > 0.0f)
                {
                    objectHasSpread = true;
                }
            }
			else if (objectSpreadMode == kIABSpreadMode_HighResolution_3D)
			{
				// 3D spread mode, check spread values in each dimension
				if ((spreadXYZ > 0.0f) || (spreadY > 0.0f) || (spreadZ > 0.0f))
				{
					objectHasSpread = true;
				}
			}
			else;				// point source, Aperture & Divegence remain 0
            
            int32_t snapSpeakerIndex = -1;
            
            // Give spread priority over snap
            // Disable snap when spread is > 0
            if (!objectHasSpread)
            {
                // Check if object snap should be activated
                IABObjectSnap objectSnap;
                iIABObjectSubBlock.GetObjectSnap(objectSnap);
                
                if (objectSnap.objectSnapPresent_)
                {
                    // Convert quantized value to float range [0.0, 1.0]
                    float snapTolerance = static_cast<float>((objectSnap.objectSnapTolerance_)/4095.0f);
                    
                    // Find speaker that satisfies the snap criteria
                    // Return value of -1 means no speaker satisfies the criteria and snap is not activate and
                    // object should be rendered as normal.
                    if (snapTolerance > 0.0f)
                    {
                        snapSpeakerIndex = FindSnapSpeakerIndex(objectPosition, snapTolerance);
                    }
                }
            }
            
            if (snapSpeakerIndex != -1)
            {
                // Snap is activated. Set iVbapObject->channelGains_, only the snapped speaker channel
                // should have gain = 1.0, all other 0.0 ?
                
                for (uint32_t gainIndex = 0; gainIndex < iVbapObject->channelGains_.size(); gainIndex++)
                {
                    if (gainIndex != (uint32_t)snapSpeakerIndex)
                    {
                        iVbapObject->channelGains_[gainIndex] = 0.0f;
                    }
                    else
                    {
                        iVbapObject->channelGains_[gainIndex] = 1.0f;
                    }                    
                }
            }
            else
            {
                // snap is not activated, render object as normal
                VBAPValueAzimuth oAzimuth;
                VBAPValueElevation oElevation;
                VBAPValueRadius oRadius;
                float aperture = 0.0;
                float divergence = 0.0;
                
                // Transform IAB positions into VBAP x, y, z positions.
                // Note, IABTransform must be applied (ie. using PyramMesa algorithm) for converstion of (iabPosX, iabPosY, iabPosZ).
                // This is because that (iabPosX, iabPosY, iabPosZ) covers the full range of unit cube listening space,
                // including interior positions(!). As a result, (iabPosX, iabPosY, iabPosZ) cannot be simply converted
                // to a vbapRendererExtendedSource using generic Cartesian-ro-Polar conversion formula. Instances of
                // vbapRendererExtendedSource must have a radius value of "1.0" only (explicitly or by conversion) to
                // be properly rendered by the underlying VBAP rendering engine.
                //
                IABTransform iabTransform;
                iabReturnCode = iabTransform.TransformIABToSphericalVBAP(iabPosX, iabPosY, iabPosZ, oAzimuth, oElevation, oRadius);
                if (iabReturnCode != kIABNoError)
                {
                    return iabReturnCode;
                }
                
                // Also transform 1d spread to aperture (and divegence of 0)
                if (objectHasSpread)
                {
					// Note: for v1.x, 3D spread is supported as 1D spread.
					// (by averaging spread values in all 3 dimensions and apply the averaged 
					// value as 1D spread.)
					//
					if (objectSpreadMode == kIABSpreadMode_HighResolution_3D)
					{
						spreadXYZ = (spreadXYZ + spreadY + spreadZ) / 3.0f;				// Averaging
					}

                    // convert spreadXYZ to aperture use Transform
                    // (Note: returned divergence is fixed to 0 by current algorithm)
                    iabReturnCode = iabTransform.TransformIAB1DSpreadToVBAPExtent(spreadXYZ, aperture, divergence);
                    
                    if (iabReturnCode != kIABNoError)
                    {
                        return iabReturnCode;
                    }
                }
                
                // Pass IAB object rendering parameters to IABInterior class for conversion
                // into VBAP extended sources.
                //
                std::vector<IABVBAP::vbapRendererExtendedSource> extendedSources;
                iabReturnCode = iabInterior_.MapExtendedSourceToVBAPExtendedSources(oAzimuth, oElevation, oRadius, aperture, divergence, extendedSources);
                if (iabReturnCode != kIABNoError)
                {
                    return iabReturnCode;
                }
                
                // Update speaker and channel variables to actual value matching config
                //
                for (uint32_t i = 0; i < extendedSources.size(); i++)
                {
                    extendedSources[i].renderedSpeakerGains_.resize(speakerCount_);
                    std::fill(extendedSources[i].renderedSpeakerGains_.begin(), extendedSources[i].renderedSpeakerGains_.end(), 0.0f);
                    
                    extendedSources[i].renderedChannelGains_.resize(static_cast<uint32_t>(numRendererOutputChannels_));
                    std::fill(extendedSources[i].renderedChannelGains_.begin(), extendedSources[i].renderedChannelGains_.end(), 0.0f);
                }
                
                // Set vbap object's extended sources
                iVbapObject->extendedSources_ = extendedSources;
                
                // Set vbap object gain
                
                IABGain objectIABGain;
                // Retrieve objectIABGain from sub-block
                iIABObjectSubBlock.GetObjectGain(objectIABGain);
                // Retrieve linear gain scale factor
                float objectGain = objectIABGain.getIABGain();
                // Set it to iVbapObject
                iVbapObject->SetGain(objectGain);
                
                // VBAP-render object. This is necessary only when subBlockPanExist == true
                // 
                vbapReturnCode = vbapRenderer_->RenderObject(iVbapObject);
                if (vbapReturnCode != IABVBAP::kVBAPNoError)
                {
                    return kIABRendererVBAPRenderingError;
                }
            }   // if (snapSpeakerIndex != -1)
            
            // Post object rendering, also check if zone 9 control is active
            // Note that SDK 1.0 does not support Zone19
            
            IABObjectZoneGain9 zoneGain9;
            iIABObjectSubBlock.GetObjectZoneGains9(zoneGain9);

            if (iabObjectZone9_)
            {
                // Yes, zone control is on, apply zone 9 gains to modify iVbapObject->channelGains_
                iabReturnCode = iabObjectZone9_->ProcessZoneGains(zoneGain9, iVbapObject->channelGains_);
                
                if (kIABNoError != iabReturnCode)
                {
                    return iabReturnCode;
                }
            }
        }   // if (subBlockPanExist)

        
		// Apply channel gains
		// Adding smooth enable/disable through enableSmoothing_ flag, replacing hard-locked
		// "true". The enableSmoothing_ is in turn "controlled" through render configuration file command
		// "c smooth 1|0".
		//   - enableSmoothing_ flag persists through all frames of a program
		gainsProceReturnCode = channelGainsProcessor_->ApplyChannelGains(iVbapObject->id_
			, iAssetSamples
			, iOutputSampleBufferCount
			, oOutputChannels
			, iOutputChannelCount
			, false									// No init to output channel buffers
			, iVbapObject->channelGains_
			, enableSmoothing_
			);

		if (gainsProceReturnCode != IABGAINSPROC::kGainsProcNoError)
		{
			return kIABRendererApplyChannelGainsError;
		}

        oRenderedOutputSampleCount = iOutputSampleBufferCount;
        
        return kIABNoError;

    }

    // IABRenderer::IsObjectActivatedForRendering() implementation
    bool IABRenderer::IsObjectActivatedForRendering(const IABObjectDefinition* iIABObject) const
    {
        // Get and check conditional
        uint1_t conditionalObject = 0;
        iIABObject->GetConditionalObject(conditionalObject);
        
        if (conditionalObject == 0)
        {
            // Unconditional/default - activated.
            return true;
        }
        else
        {
            // It's a conditional. Check object use case
            IABUseCaseType objectUseCase = kIABUseCase_NoUseCase;
            iIABObject->GetObjectUseCase(objectUseCase);
            
            if (objectUseCase == kIABUseCase_Always)
            {
                // If always use - activated
                return true;
            }
            else if ((objectUseCase == targetUseCase_) && (objectUseCase != kIABUseCase_NoUseCase))
            {
                // If object use case matches the target use case - activated
                return true;
            }
            else
            {
                // Otherwise - not activated
                return false;
            }
        }
    }
    
	// IABRenderer::RenderIABBed() implementation
	iabError IABRenderer::RenderIABBed(const IABBedDefinitionInterface& iIABBed
                                       , IABSampleType **oOutputChannels
                                       , IABRenderedOutputChannelCountType iOutputChannelCount
                                       , IABRenderedOutputSampleCountType iOutputSampleBufferCount
                                       , IABRenderedOutputSampleCountType &oRenderedOutputSampleCount)
    {
		iabError iabReturnCode = kIABNoError;
		oRenderedOutputSampleCount = 0;

		// Save meta ID to parentMetaID_.
		iIABBed.GetMetadataID(parentMetaID_);
        
		// Conditional Bed logic implementation
		//

		// Step 1 check: Is this bed (itself) activated?
		// If not, exit right away. Otherwise, continue
		if (!IsBedActivatedForRendering(dynamic_cast<const IABBedDefinition*>(&iIABBed)))
		{
			oRenderedOutputSampleCount = iOutputSampleBufferCount;
			return iabReturnCode;
		}

		// Step 2 check: "this" is activated, but is there a sub-lement (if present)
		// that is also activated?
		// If yes, the activated child/sub-element is rendered, replacing the parent
		//
		IABElementCountType numSubElements = 0;
		iIABBed.GetSubElementCount(numSubElements);					// Call never return error, no check.

		// Sub-elements present, check if one of them is activated.
		// (Developer note: ST2098-2 (page 25, section 10.3.2): "... of the child elements of a 
		// BedDefinition, at most one BedDefinition or BedRemap child element is activated."
		//
		if (numSubElements != 0)
		{
			// Get sub-elements 
			std::vector<IABElement*> bedSubElements;
			IABElementIDType elementID;

			iIABBed.GetSubElements(bedSubElements);

			// Loop through sub-elements
			// Check if any of the pointers is a NULL or not a valid sub-element type
			for (std::vector<IABElement*>::iterator iter = bedSubElements.begin(); iter != bedSubElements.end(); iter++)
			{
				if (NULL == *iter)
				{
					// Early exit if containing NULL
					return kIABRendererBedDefinitionError;
				}

				// Get and check ID
				(*iter)->GetElementID(elementID);

				if (kIABElementID_BedDefinition == elementID)
				{
					// Sub-element of BedDefinition type. If this is activated, this should be rendered
					// in place of parent (this).
					if ( IsBedActivatedForRendering(dynamic_cast<IABBedDefinition*>(*iter)) )
					{
						// Render the activated sub-element bed instead
						// (Recursive call)
						//
						iabReturnCode = RenderIABBed(*(dynamic_cast<IABBedDefinitionInterface*>(*iter))
													, oOutputChannels
													, iOutputChannelCount
													, iOutputSampleBufferCount
													, oRenderedOutputSampleCount);

						// Exit, return error code as-is.
						return iabReturnCode;
					}
				}
				else if (kIABElementID_BedRemap == elementID)
				{
					// Sub-element of BedRemap type. If this is activated, this should be rendered
					// by RenderIABBedRemap(), in combination with parent bed
					if (IsBedRemapActivatedForRendering(dynamic_cast<IABBedRemap*>(*iter)))
					{
						iabReturnCode = RenderIABBedRemap(*(dynamic_cast<IABBedRemapInterface*>(*iter))
													, iIABBed
													, oOutputChannels
													, iOutputChannelCount
													, iOutputSampleBufferCount
													, oRenderedOutputSampleCount);

						// Exit, return error code as-is.
						return iabReturnCode;
					}
				}
				else;	// Ignore other types of sub-elements.
			}
		}

		// Step check 3. No further check. If conditional bed processing flow reaches this
		// point, the bed (this) meets all conditions for rendering directly.
		// Continue to render this bed.

        // Bed definition is active, render each bed channel
		//
		IABChannelCountType channelCount = 0;
		iIABBed.GetChannelCount(channelCount);
        std::vector<IABChannel*> bedChannels;
		iIABBed.GetBedChannels(bedChannels);

		// Check size and parameter congruency, and at least 1 channel is present
		if ( (channelCount == 0) || (bedChannels.size() != channelCount) )
		{
			return kIABRendererBedDefinitionError;
		}
        
        std::vector<IABChannel*>::const_iterator iterBedChannel;
        IABRenderedOutputSampleCountType renderedOutputSampleCount = 0;

        for (iterBedChannel = bedChannels.begin(); iterBedChannel != bedChannels.end(); iterBedChannel++)
        {
            // Ensure pointer is not NULL
            if (*iterBedChannel == NULL)
            {
                return kIABRendererBedDefinitionError;
            }
            
            IABAudioDataIDType audioDataID;
            
            (*iterBedChannel)->GetAudioDataID(audioDataID);
            
            if (audioDataID == 0)
            {
                // For IAB, when audioData ID = 0, then the bed channel has no audio for this frame and can be skipped
                continue;
            }
            
            // Update working audio sample buffer (sampleBufferFloat_) with bed channel audio samples
            iabReturnCode = UpdateAudioSampleBuffer(audioDataID);
            if (kIABNoError != iabReturnCode)
            {
                // audioData ID not found or no valid sample pointer
                return iabReturnCode;
            }
            
            IABSampleType *pAssetSamples = sampleBufferFloat_;

			iabReturnCode = RenderIABChannel(**iterBedChannel
				, pAssetSamples
				, oOutputChannels
				, iOutputChannelCount
				, iOutputSampleBufferCount
				, renderedOutputSampleCount);
            
			if (kIABNoError != iabReturnCode)
			{
				return iabReturnCode;
			}
			
            if (renderedOutputSampleCount != iOutputSampleBufferCount)
            {
                return kIABRendererBedDefinitionError;
            }
        }
        
        oRenderedOutputSampleCount = iOutputSampleBufferCount;

        return kIABNoError;
    }

	// IABRenderer::IsBedActivatedForRendering() implementation
	bool IABRenderer::IsBedActivatedForRendering(const IABBedDefinition* iIABBed) const
	{
		// Get and check conditional
		uint1_t conditionalBed = 0;
		iIABBed->GetConditionalBed(conditionalBed);

		if (conditionalBed == 0)
		{
			// Unconditional/default - activated.
			return true;
		}
		else
		{
			// It's a conditional. Check bed use case
			IABUseCaseType bedUseCase = kIABUseCase_NoUseCase;
			iIABBed->GetBedUseCase(bedUseCase);

			if (bedUseCase == kIABUseCase_Always)
			{
				// If always use - activated
				return true;
			}
			else if ((bedUseCase == targetUseCase_) && (bedUseCase != kIABUseCase_NoUseCase))
			{
				// If bed use case matches the target use case - activated
				return true;
			}
			else
			{
				// Otherwise - not activated
				return false;
			}
		}
	}

	// IABRenderer::IsBedRemapActivatedForRendering() implementation
	bool IABRenderer::IsBedRemapActivatedForRendering(const IABBedRemap* iIABBedRemap) const
	{
		// It's a conditional. Check bed use case
		IABUseCaseType remapUseCase = kIABUseCase_NoUseCase;
		iIABBedRemap->GetRemapUseCase(remapUseCase);

		if (remapUseCase == kIABUseCase_Always)
		{
			// If always use - activated (This is logically true, but practically may not exist.)
			return true;
		}
		else if ((remapUseCase == targetUseCase_) && (remapUseCase != kIABUseCase_NoUseCase))
		{
			// If remap use case matches the target use case - activated
			return true;
		}
		else
		{
			// Otherwise - not activated
			return false;
		}
	}

	// IABRenderer::RenderIABChannel() implementation
	iabError IABRenderer::RenderIABChannel(const IABChannelInterface& iIABChannel
                                           , IABSampleType *iAssetSamples
                                           , IABSampleType **oOutputChannels
                                           , IABRenderedOutputChannelCountType iOutputChannelCount
                                           , IABRenderedOutputSampleCountType iOutputSampleBufferCount
                                           , IABRenderedOutputSampleCountType &oRenderedOutputSampleCount)
    {
        oRenderedOutputSampleCount = 0;
        
        IABGain channelGain;
        iIABChannel.GetChannelGain(channelGain);

        float gain = channelGain.getIABGain();

        if (gain == 0.0f)
        {
            // zero channel gain, no further processing required.
            // set to same as iOutputSampleBufferCount fo now, so it is not seen as an error by the caller
            oRenderedOutputSampleCount = iOutputSampleBufferCount;
            return kIABNoError;
        }

        IABChannelIDType channelID;
        IABSampleType *ptrInputSamples = iAssetSamples;

        // Use bed channel ID to find the corresponding config file speaker label
        std::map<IABChannelIDType, IABRendererBedChannelInfo>::const_iterator iterBedChannelMap;
        iIABChannel.GetChannelID(channelID);
		iterBedChannelMap = IABConfigTables::bedChannelInfoMap.find(channelID);
        
        if (iterBedChannelMap == IABConfigTables::bedChannelInfoMap.end())
        {
            // No such channel ID in IABConfigTables::bedChannelInfoMap
            return kIABRendererBedChannelError;
        }

        // Map entry found, use the channel's speaker label to check if it is in the
        // config file speaker list
		
		// First, from physical URI'ed speakers, in precedence
        std::map<std::string, int32_t>::iterator iterChannelIndexMap = physicalURIedBedSpeakerOutputIndexMap_.find(iterBedChannelMap->second.speakerURI_);
		
		// Then as next-step backup, from all URI'ed speakers, including virtual speakers that have valid, non-empty URIs defined.
		// Effectively, this "find" will locate both physical AND virtual ones. The physical ones overlap with the first "find" above.
		// When the first, physical ones are not found, this will help to "find" virtual ones, if present
		std::map<std::string, int32_t>::iterator iterVirtualSpeakerIndexMap = totalURIedSpeakerToTotalSpeakerListMap_.find(iterBedChannelMap->second.speakerURI_);

        if (iterChannelIndexMap != physicalURIedBedSpeakerOutputIndexMap_.end())
        {
            // Yes, it's in the list, ensure index is within range
            if (iterChannelIndexMap->second >= iOutputChannelCount)
            {
                return kIABRendererBedChannelError;
            }
            
            // Add asset to the buffer
            IABSampleType *ptrChannelBuffer = oOutputChannels[iterChannelIndexMap->second];
            
            if (gain == 1.0f)
            {
                for (uint32_t i = 0; i < iOutputSampleBufferCount; i++)
                {
                    // Add asset to the target channel buffer
                    *ptrChannelBuffer++ += (*ptrInputSamples++);
                }
            }
            else
            {
                for (uint32_t i = 0; i < iOutputSampleBufferCount; i++)
                {
                    // Apply gain and add asset to the target channel buffer
                    *ptrChannelBuffer++ += (*ptrInputSamples++) * gain;
                }
            }
            
            oRenderedOutputSampleCount = iOutputSampleBufferCount;
            return kIABNoError;
        }
		else if (iterVirtualSpeakerIndexMap != totalURIedSpeakerToTotalSpeakerListMap_.end())
		{
			// The Bed channel maps to a virtual speaker in config file. And their URIs match.
			// Need to direct routing, and apply downmix coeffs to physical channels.

			// Get downmix map for the virtual bed channel
			const std::vector<RenderUtils::DownmixValue> downmixMap = (totalSpeakerList_->at(iterVirtualSpeakerIndexMap->second)).getNormalizedDownmixValues();

			// Aggregate channel gain with map coefficients
			std::vector<RenderUtils::DownmixValue> aggregatedDownmixMap;
			uint32_t sizeDownmixMap = static_cast<uint32_t>(downmixMap.size());

			for (uint32_t i = 0; i < sizeDownmixMap; i++)
			{
				aggregatedDownmixMap.push_back(RenderUtils::DownmixValue(downmixMap[i].ch_, (downmixMap[i].coefficient_ * gain)));
			}

			// Mix bed channel samples into mapped physical output channels, as listed in aggregatedDownmixMap
			for (uint32_t i = 0; i < sizeDownmixMap; i++)
			{
				float downmixChannelGain = aggregatedDownmixMap[i].coefficient_;

				ptrInputSamples = iAssetSamples;														// Reset input sample pointer for each downmix output speaker

				// Need to map .ch_ (speaker index) to the actual output index. The latter in in the range of 
				// physical outputs for physical speaker (while the former is indexed with inclusion of virtual).
				if (speakerIndexToOutputIndexMap_.find(aggregatedDownmixMap[i].ch_) == speakerIndexToOutputIndexMap_.end())
				{
					return kIABRendererDownmixChannelError;
				}

				IABSampleType *ptrOutputChannelBuffer = oOutputChannels[speakerIndexToOutputIndexMap_.at(aggregatedDownmixMap[i].ch_)];	// pointer to downmix output channel

				for (uint32_t i = 0; i < iOutputSampleBufferCount; i++)
				{
					// Apply gain and add asset to the target channel buffer
					*ptrOutputChannelBuffer++ += (*ptrInputSamples++) * downmixChannelGain;
				}
			}

			oRenderedOutputSampleCount = iOutputSampleBufferCount;
			return kIABNoError;
		}
		else if (iterBedChannelMap->second.speakerURI_ != speakerURILFE)
        {
            // No, the target layout does not contain this bed channel and it is not LFE
            // Render the bed channel as a point source object with the speaker's VBAP coordinates
            return (
				RenderIABChannelAsObject(channelID
				, gain
				, iAssetSamples
				, oOutputChannels
				, iOutputChannelCount
				, iOutputSampleBufferCount
				, oRenderedOutputSampleCount)
				);
        }
        else
        {
            // Keep track of warnings, but don't return error
            warnings_[kIABRendererNoLFEInConfigForBedLFEWarning] = 1;

            // set to same as iOutputSampleBufferCount fo now, so it is not seen as an error by the caller
            oRenderedOutputSampleCount = iOutputSampleBufferCount;

            return kIABNoError;
        }
    }

	// IABRenderer::RenderIABChannelAsObject() implementation
	iabError IABRenderer::RenderIABChannelAsObject(IABChannelIDType iChannelID
                                                   , float iChannelGain
                                                   , IABSampleType *iAssetSamples
                                                   , IABSampleType **oOutputChannels
                                                   , IABRenderedOutputChannelCountType iOutputChannelCount
                                                   , IABRenderedOutputSampleCountType iOutputSampleBufferCount
                                                   , IABRenderedOutputSampleCountType &oRenderedOutputSampleCount)
    {
        IABVBAP::vbapError vbapReturnCode = IABVBAP::kVBAPNoError;
		IABGAINSPROC::gainsProcError gainsProceReturnCode = IABGAINSPROC::kGainsProcNoError;

		oRenderedOutputSampleCount = 0;

        if (!vbapObject_)
        {
            // working vbapObject has not been instantiated
            return kIABRendererBedChannelError;
        }

        // Reset working object data structure to default state before updating
        ResetVBAPObject();

		// VBAP object ID is generated by combining iChannelID, bed parentMetaID_, and
		// then adding 0xff000000, for improved uniqueness
		uint32_t vbapIDforChannel = iChannelID + (parentMetaID_ << 8) + 0xff000000;

        // Set VBAP object ID
        vbapObject_->SetId(vbapIDforChannel);
        
        // Get bed channel VBAP coordinates from the map to set source position
        std::map<IABChannelIDType, IABRendererBedChannelInfo>::const_iterator iter;
        iter = IABConfigTables::bedChannelInfoMap.find(iChannelID);
        if (iter == IABConfigTables::bedChannelInfoMap.end())
        {
            return kIABRendererBedChannelError;
        }

		// To set extended sources for vbapObject
		std::vector<IABVBAP::vbapRendererExtendedSource> extendedSources;

		// Channel position based rendering. Position already on dome.
		// Extent parameters set to 0 (default).
		//
        IABVBAP::vbapRendererExtendedSource extendedSource(speakerCount_, numRendererOutputChannels_);
        extendedSource.SetPosition((*iter).second.speakerVBAPCoordinates_);
		// Set gain for extended source to (default) 1.0
        extendedSource.SetGain(1.0f);

		// Add to extendedSources
		extendedSources.push_back(extendedSource);

		// Set vbap object's extended sources
		vbapObject_->extendedSources_ = extendedSources;
		// Set iChannelID as vbap object gain
		vbapObject_->SetGain(iChannelGain);

		// VBAP-render object.
		// 
		vbapReturnCode = vbapRenderer_->RenderObject(vbapObject_);
		if (vbapReturnCode != IABVBAP::kVBAPNoError)
		{
			return kIABRendererVBAPRenderingError;
		}

		// Apply channel gains
		// Adding smooth enable/disable through enableSmoothing_ flag, replacing hard-locked
		// "true". The enableSmoothing_ is in turn "controlled" through render configuration file command
		// "c smooth 1|0".
		//   - enableSmoothing_ flag persists through all frames of a program
		gainsProceReturnCode = channelGainsProcessor_->ApplyChannelGains(vbapObject_->id_
			, iAssetSamples
			, iOutputSampleBufferCount
			, oOutputChannels
			, iOutputChannelCount
			, false									// No init to output channel buffers
			, vbapObject_->channelGains_
			, enableSmoothing_
			);

		if (gainsProceReturnCode != IABGAINSPROC::kGainsProcNoError)
		{
			return kIABRendererApplyChannelGainsError;
		}

		oRenderedOutputSampleCount = iOutputSampleBufferCount;

        return kIABNoError;
    }

	// IABRenderer::RenderIABBedRemap() implementation
	iabError IABRenderer::RenderIABBedRemap(const IABBedRemapInterface& iIABBedRemap
                                            , const IABBedDefinitionInterface& iParentBed
                                            , IABSampleType **oOutputChannels
                                            , IABRenderedOutputChannelCountType iOutputChannelCount
                                            , IABRenderedOutputSampleCountType iOutputSampleBufferCount
                                            , IABRenderedOutputSampleCountType &oRenderedOutputSampleCount)
    {
		iabError iabReturnCode = kIABNoError;

		// Private method serving BedDefinition rendering only. Checking done at parent level.

		IABUseCaseType remapUseCase = kIABUseCase_NoUseCase;
		iIABBedRemap.GetRemapUseCase(remapUseCase);

		IABChannelCountType sourceChannelCount = 0;
		iIABBedRemap.GetSourceChannels(sourceChannelCount);

		IABChannelCountType destinationChannelCount = 0;
		iIABBedRemap.GetDestinationChannels(destinationChannelCount);

		// double check remap case
		if (remapUseCase != targetUseCase_ && remapUseCase != kIABUseCase_Always)
		{
			return kIABRendererBedRemapError;
		}

		// Retrieve source channels from parent bed for checking
		std::vector<IABChannel*> sourceChannels;
		iParentBed.GetBedChannels(sourceChannels);

		if ( (sourceChannelCount == 0) 
			|| (sourceChannels.size() != sourceChannelCount)
			|| (destinationChannelCount == 0)
			|| (iOutputSampleBufferCount == 0) )
		{
			return kIABRendererBedRemapError;
		}

		// Decode/unpack all source channel assets before remap sub block processing
		// (Audio samples (DLC or PCM) is decoded/unpacked at frame-atomic level, and not at sub block level.)
		// 

		// To allow for this, buffers holding decoded source channel assets must be allocated
		// (Dev note: optimization posibility to IABRenderer class to avoid alloc-dealloc?. 
		// Challenge: Source channel number depending on source bed and not pre-known.
		// Pre-alloc buffers for all supported/legal SMPTE channels.)

		// Buffers holding decoded channel PCM samples
		float *sourceChannelPCMBuffer = new float[sourceChannelCount * iOutputSampleBufferCount];

		// Init to 0.0f
		std::fill(sourceChannelPCMBuffer
			, (sourceChannelPCMBuffer + sourceChannelCount * iOutputSampleBufferCount)
			, 0.0f);

		// Array of pointers for individual source channel samples
		float **sourceBufferPointers = new float*[sourceChannelCount];
		IABAudioDataIDType audioDataID = 0;

		// Pre-fetch source channel gains/scale for later use during remap processing
		IABGain sourceChannelGain;
		float *sourceChannelScales = new float[sourceChannelCount];
        
        // Init to 0.0f
        std::fill(sourceChannelScales
                  , (sourceChannelScales + sourceChannelCount)
                  , 0.0f);

		// Init and PCM decoding
		// 1) Init sourceBufferPointers to beginning of each decoded source channel
		// 2) Decode PCM for each source channel
		// 3) Pre-fetch source channel gains and fill sourceChannelScales
		//
		for (uint32_t i = 0; i < sourceChannelCount; i++)
		{
			//  Set PCM beginning pointer for channel i 
			sourceBufferPointers[i] = sourceChannelPCMBuffer + i * iOutputSampleBufferCount;

			// Check sourceChannels[i] pointer
			if (sourceChannels[i] == NULL)
			{
				// Error condition, delete buffers
				delete[] sourceChannelPCMBuffer;
				delete[] sourceBufferPointers;
				delete[] sourceChannelScales;

				return kIABRendererBedRemapError;
			}

            // Pre-fetch/retrieve source channel gain and convert to scales
            sourceChannels[i]->GetChannelGain(sourceChannelGain);
            sourceChannelScales[i] = sourceChannelGain.getIABGain();
            
			sourceChannels[i]->GetAudioDataID(audioDataID);
			if (audioDataID == 0)
			{
				// AudioID of 0: no DLC/PCM element, ie. source audio contains silence only
				// Corresponding source PCM sample values remain 0.0f, as init'ed by std::fill() call above
				continue;
			}

			// Decode source channel audio samples and save to holding buffer to be used as source 
			// samples by remap processing below.
			iabReturnCode = UpdateAudioSampleBuffer(audioDataID, sourceBufferPointers[i]);
			if (kIABNoError != iabReturnCode)
			{
				// DLC audio ID not found or no valid sample pointer
				// Error condition, delete buffers
				delete[] sourceChannelPCMBuffer;
				delete[] sourceBufferPointers;
				delete[] sourceChannelScales;

				return iabReturnCode;
			}
		}

		// *** Get Remap sub blocks
		std::vector<IABBedRemapSubBlock*> remapSubBlocks;
		iIABBedRemap.GetRemapSubBlocks(remapSubBlocks);

		uint8_t numRemapSubBlocks = 0;
		iIABBedRemap.GetNumRemapSubBlocks(numRemapSubBlocks);

		// Check sub block number vs size
		if ( (numRemapSubBlocks == 0)
			|| (remapSubBlocks.size() != numRemapSubBlocks)
			|| (numRemapSubBlocks != numPanSubBlocks_))
		{
			// Error condition, delete buffers
			delete[] sourceChannelPCMBuffer;
			delete[] sourceBufferPointers;
			delete[] sourceChannelScales;

			return kIABRendererBedRemapError;
		}

		// Process "remap sub blocks" 1 by 1
		//
		std::vector<IABRemapCoeff*> remapCoeffArray;
		uint1_t remapInfoExist = 0;
		uint16_t numSource = 0;
		uint16_t numDestination = 0;

		// For remapped channels that do not find a match in speaker list, a temp buffer
		// is needed to hold remapped samples for further render-as-object.
		// "render-as-object" is on subblock basis
        // Allocate buffer to support maximum subblock sample count of all supported
        // sample rate, frame rate combinations
		float *tempRemappedPCMBuffer = new float[kIABMaxSubblockSampleCount];

		// Variable for saving returned samples-rendered count
		IABRenderedOutputSampleCountType returnedSampleCount = 0;

		// *** Looping through remap sub blocks
		for (uint32_t n = 0; n < numRemapSubBlocks; n++)
		{
            // number of samples per remap sub-block
            uint32_t subBlockSampleCount = subBlockSampleCount_[n];

			// Check RemapInfoExists - always exist for the first sub block
			remapSubBlocks[n]->GetRemapInfoExists(remapInfoExist);

			if (remapInfoExist != 0)
			{
				// Get remap coefficients for the block
				// remapInfoExist for remapSubBlocks[0] always true. Other sub blocks depending on 
				// remapInfoExist parameter parsed from stream.
				// Note, if not updated, coeffs from previous block carries forward. This
				// is the intended behavior.
				remapSubBlocks[n]->GetRemapCoeffArray(remapCoeffArray);
			}

			// Extra check on remapCoeffArray, the size need to match numDestination
			numDestination = static_cast<uint16_t>(remapCoeffArray.size());
			if (numDestination != destinationChannelCount)
			{
				// Error condition, delete buffers
				delete[] sourceChannelPCMBuffer;
				delete[] sourceBufferPointers;
				delete[] sourceChannelScales;
				delete[] tempRemappedPCMBuffer;

				return kIABRendererBedRemapError;
			}

			// Apply remap coefficients
			IABChannelIDType destinationChannelID;
			std::map<IABChannelIDType, IABRendererBedChannelInfo>::const_iterator iterDestChannelMap;
			IABGain remapGain;
			float remapScale = 0.0f;

			// Loop through destination/output channels
			for (uint32_t i = 0; i < destinationChannelCount; i++)
			{
				destinationChannelID = remapCoeffArray[i]->getDestinationChannelID();

				// Is this destination channel ID in IAB spec?
				iterDestChannelMap = IABConfigTables::bedChannelInfoMap.find(destinationChannelID);

				if (iterDestChannelMap == IABConfigTables::bedChannelInfoMap.end())
				{
					// No such channel ID in IABConfigTables::bedChannelInfoMap
					// Error condition, delete buffers
					delete[] sourceChannelPCMBuffer;
					delete[] sourceBufferPointers;
					delete[] sourceChannelScales;
					delete[] tempRemappedPCMBuffer;

					return kIABRendererBedRemapError;
				}

				// Is this destination channel ID in config file speaker list?
				// (use the channel's speaker label to check)
				// First, from physical URI'ed speakers, in precedence
				std::map<std::string, int32_t>::iterator iterChannelIndexMap =
					physicalURIedBedSpeakerOutputIndexMap_.find(iterDestChannelMap->second.speakerURI_);

				// Then as next-step backup, from all URI'ed speakers, including virtual speakers that have valid, non-empty URIs defined.
				// Effectively, this "find" will locate both physical AND virtual ones. The physical ones overlap with the first "find" above.
				// When the first, physical ones are not found, this will help to "find" virtual ones, if present
				std::map<std::string, int32_t>::iterator iterVirtualSpeakerIndexMap = 
					totalURIedSpeakerToTotalSpeakerListMap_.find(iterDestChannelMap->second.speakerURI_);

				// Three possibilities for remap processing below.
				// 1) Destination channel ID in the target physical speaker list - directly remap
				// 2) Destination channel ID in the target virtual speaker list - directly remap to virtual and then downmix
				// 3) Destination channel ID NOT in any target speaker list - remap first
				//    & render-as-object to target
				//
				if (iterChannelIndexMap != physicalURIedBedSpeakerOutputIndexMap_.end())
				{
					// In config speaker list - ensure index is within range
					if (iterChannelIndexMap->second >= iOutputChannelCount)
					{
						// Error condition, delete buffers
						delete[] sourceChannelPCMBuffer;
						delete[] sourceBufferPointers;
						delete[] sourceChannelScales;
						delete[] tempRemappedPCMBuffer;

						return kIABRendererBedRemapError;
					}

					// destination channel buffer. Note to shift by n sub-blocks
					IABSampleType *destChannelBuffer = 
						oOutputChannels[iterChannelIndexMap->second] + subBlockSampleStartOffset_[n];

					// Extra check on numSource in remapCoeffArray[i]
					numSource = remapCoeffArray[i]->getRemapSourceNumber();
					if (numSource != sourceChannelCount)
					{
						// Error condition, delete buffers
						delete[] sourceChannelPCMBuffer;
						delete[] sourceBufferPointers;
						delete[] sourceChannelScales;
						delete[] tempRemappedPCMBuffer;

						return kIABRendererBedRemapError;
					}

					// Loop through source channels for 1 destination channel
					for (uint32_t j = 0; j < sourceChannelCount; j++)
					{
						// Get the j-source to i-destination remap coeff
						iabReturnCode = remapCoeffArray[i]->getRemapCoeff(remapGain, j);

						if (kIABNoError != iabReturnCode)
						{
							// Error condition, delete buffers
							delete[] sourceChannelPCMBuffer;
							delete[] sourceBufferPointers;
							delete[] sourceChannelScales;
							delete[] tempRemappedPCMBuffer;

							return iabReturnCode;
						}

						// Get the j-source to i-destination remap scaler
						remapScale = remapGain.getIABGain();

						// jth-source channel buffer. Note to shift by n sub-blocks
						IABSampleType *srcChannelBuffer = sourceBufferPointers[j] + subBlockSampleStartOffset_[n];

						// Apply both remap scale and source channel scale for jth source, in tandem 
						// and accumulate remapped PCM to output
						float combinedScale = remapScale * sourceChannelScales[j];

						if (combinedScale == 0.0f)
						{
							continue;
						}

						for (uint32_t k = 0; k < subBlockSampleCount; k++)
						{
							destChannelBuffer[k] += srcChannelBuffer[k] * combinedScale;
						}
					}
				}
				else if (iterVirtualSpeakerIndexMap != totalURIedSpeakerToTotalSpeakerListMap_.end())
				{
					// The destination channel maps to a virtual speaker in config file. And their URIs match.
					// Direct remap, and apply downmix coeffs to output to physical channels.

					// Get downmix map for the virtual destination speaker to physical speakers
					const std::vector<RenderUtils::DownmixValue> downmixMap = (totalSpeakerList_->at(iterVirtualSpeakerIndexMap->second)).getNormalizedDownmixValues();
					uint32_t sizeDownmixMap = static_cast<uint32_t>(downmixMap.size());

					// For each of the downmixed physical speakers (for the URIed virtual speaker), need to
					// apply a combined gain scale factor to source channel samples (before sending them as output 
					// to a downmix physical speaker)
					// 1. Source channel gain,
					// 2. Remap gain, and
					// 3. Downmix coeff (this one from configuration file)
					float combinedScale = 1.0f;

					// Mix Remap source channel samples into mapped physical output channels
					// Note, number of downmix physical speakers is equal to sizeDownmixMap.
					for (uint32_t m = 0; m < sizeDownmixMap; m++)
					{
						// Need to map .ch_ (speaker index) to the actual output index. The latter is in the range for 
						// physical speaker output indices(while the former is indexed all speakers including virtual).
						if (speakerIndexToOutputIndexMap_.find(downmixMap[m].ch_) == speakerIndexToOutputIndexMap_.end())
						{
							// Not found. An error condition, delete buffers
							delete[] sourceChannelPCMBuffer;
							delete[] sourceBufferPointers;
							delete[] sourceChannelScales;
							delete[] tempRemappedPCMBuffer;

							return kIABRendererDownmixChannelError;
						}

						// Pointer to downmix physical output buffer. Note to shift by n sub-blocks
						IABSampleType *destChannelBuffer = oOutputChannels[speakerIndexToOutputIndexMap_.at(downmixMap[m].ch_)] 
							+ subBlockSampleStartOffset_[n];

						// Loop through source channel for the mth physical speaker of the virtual destination speaker
						for (uint32_t j = 0; j < sourceChannelCount; j++)
						{
							// Reset combined gains with mth downmix coefficient
							// Need to reset this for each of the source channels, like "init"
							combinedScale = downmixMap[m].coefficient_;

							// Get the j-source to i-destination remap coeff (the ith destination is a virtual in this case.)
							iabReturnCode = remapCoeffArray[i]->getRemapCoeff(remapGain, j);

							if (kIABNoError != iabReturnCode)
							{
								// Error condition, delete buffers
								delete[] sourceChannelPCMBuffer;
								delete[] sourceBufferPointers;
								delete[] sourceChannelScales;
								delete[] tempRemappedPCMBuffer;

								return iabReturnCode;
							}

							// Get the j-source to i-destination remap scaler
							remapScale = remapGain.getIABGain();

							// jth-source channel buffer. Note to shift by n sub-blocks
							IABSampleType *srcChannelBuffer = sourceBufferPointers[j] + subBlockSampleStartOffset_[n];

							// Comnine both remap scale and source channel scale for jth source, with downmix coeff. 
							combinedScale *= (remapScale * sourceChannelScales[j]);

							if (combinedScale == 0.0f)
							{
								continue;
							}

							// Accumulate remapped PCM to the mth downmix physical speaker output
							for (uint32_t k = 0; k < subBlockSampleCount; k++)
							{
								destChannelBuffer[k] += srcChannelBuffer[k] * combinedScale;
							}
						}    // END of source channel j loop "for (uint32_t j = 0; j < sourceChannelCount; j++)"
					}        // END of downmix coeff array m loop "for (uint32_t m = 0; m < sizeDownmixMap; i++)"
				}
				else if (iterDestChannelMap->second.speakerURI_ != speakerURILFE)
				{
					// Not in config speaker list - so need additional render-as-object
					// processing after remap
					// (Excluding LFE from render-as-object processing)

					// Remap first to tempRemappedPCMBuffer - to be used as input
					// for render-as-object

					// Init tempRemappedPCMBuffer to 0.0f
					std::fill(tempRemappedPCMBuffer, (tempRemappedPCMBuffer + kIABMaxSubblockSampleCount), 0.0f);

					// Loop through source channels
					for (uint32_t j = 0; j < sourceChannelCount; j++)
					{
						// Get the j-source to i-destination remap coeff
						iabReturnCode = remapCoeffArray[i]->getRemapCoeff(remapGain, j);

						if (kIABNoError != iabReturnCode)
						{
							// Error condition, delete buffers
							delete[] sourceChannelPCMBuffer;
							delete[] sourceBufferPointers;
							delete[] sourceChannelScales;
							delete[] tempRemappedPCMBuffer;

							return iabReturnCode;
						}

						// Get the j-source to i-destination remap scaler
						remapScale = remapGain.getIABGain();

						// jth-source channel buffer. Note to shift by n sub-blocks
						IABSampleType *srcChannelBuffer = sourceBufferPointers[j] + subBlockSampleStartOffset_[n];

						// Apply remap scale and source channel scale for jth source, in tandem.
						// Mapped PCM output to be sent (copied/overwritten) to tempRemappedPCMBuffer
						float combinedScale = remapScale * sourceChannelScales[j];

						if (combinedScale == 0.0f)
						{
							continue;
						}

						for (uint32_t k = 0; k < subBlockSampleCount; k++)
						{
							tempRemappedPCMBuffer[k] += srcChannelBuffer[k] * combinedScale;
						}
					}

					// Remap done for the distination ID. As it is not in speaker list,
					// samples in tempRemappedPCMBuffer is further rendered as object, 

					// Set up output buffer pointers for sub-block, per remap subblock index n
					// ie. to shift each output pointer by n sub-blocks of subBlockSampleCount
					for (uint32_t m = 0; m < iOutputChannelCount; m++)
					{
						outputBufferPointers_[m] = oOutputChannels[m] + subBlockSampleStartOffset_[n];
					}

					// Render-as-object
					// Note source gains are processed during remapping. Unity gain "1.0f" is used below.
					iabReturnCode = RenderIABChannelAsObject(destinationChannelID
						, 1.0f
						, tempRemappedPCMBuffer
						, outputBufferPointers_
						, iOutputChannelCount
						, subBlockSampleCount
						, returnedSampleCount);

					if (kIABNoError != iabReturnCode)
					{
						// Error condition, delete buffers
						delete[] sourceChannelPCMBuffer;
						delete[] sourceBufferPointers;
						delete[] sourceChannelScales;
						delete[] tempRemappedPCMBuffer;

						return iabReturnCode;
					}
				}
				else
				{
					// Keep track of warnings, but don't return error
					warnings_[kIABRendererNoLFEInConfigForRemapLFEWarning] = 1;
				}    // End/closing "if (iterChannelIndexMap != physicalURIedBedSpeakerOutputIndexMap_.end())"
			}        // End/closing "for (uint32_t i = 0; i < destinationChannelCount; i++)"
		}            // End/closing "for (uint32_t n = 0; n < numRemapSubBlocks; n++)"

		// Processing reaching this point, processed output samples, sub-block samples, etc
		// all agree. Set oRenderedOutputSampleCount
		oRenderedOutputSampleCount = iOutputSampleBufferCount;

		// Clean up buffers
		delete[] sourceChannelPCMBuffer;
		delete[] sourceBufferPointers;
		delete[] sourceChannelScales;
		delete[] tempRemappedPCMBuffer;

		return kIABNoError;
	}

    iabError IABRenderer::RenderIABObjectZone19(const IABObjectZoneDefinition19Interface& iIABZone19
                                                , IABSampleType **oOutputChannels
                                                , IABRenderedOutputChannelCountType iOutputChannelCount
                                                , IABRenderedOutputSampleCountType iOutputSampleBufferCount
                                                , IABRenderedOutputSampleCountType &oRenderedOutputSampleCount)
    {
    	// Not implemented in v1.x
        return kIABNotImplementedError;
    }

    iabError IABRenderer::RenderIABZone19SubBlock(const IABZone19SubBlockInterface& iIABZone19SubBlock
                                                  , IABSampleType **oOutputChannels
                                                  , IABRenderedOutputChannelCountType iOutputChannelCount
                                                  , IABRenderedOutputSampleCountType iOutputSampleBufferCount
                                                  , IABRenderedOutputSampleCountType &oRenderedOutputSampleCount)
    {
		// Not implemented in v1.x
		return kIABNotImplementedError;
    }
    
    // IABRenderer::UpdateAudioSampleBuffer() implementation
	iabError IABRenderer::UpdateAudioSampleBuffer(IABAudioDataIDType iAudioDataID, IABSampleType* iOutputSampleBuffer)
    {
		iabError errorCode = kIABNoError;

        if (iAudioDataID == 0)
        {
            return kIABBadArgumentsError;
        }
        
        if ((sampleBufferInt_ == NULL) || (iOutputSampleBuffer == NULL))
        {
            // working integer and float buffers have not been instantiated
            return kIABRendererNotInitialisedError;
        }

        std::vector<IABElement*> frameSubElements;
        std::vector<IABElement*>::const_iterator iterFSE;
        IABAudioDataIDType audioDataID;
        bool sampleUpdated = false;
        
        // Search through frame sub-elements to find audio data element
        iabFrameToRender_->GetSubElements(frameSubElements);
        IABAudioDataDLC *dlcElement = NULL;
        IABAudioDataPCM *pcmElement = NULL;
        bool foundAudioElement = false;
        
        for (iterFSE = frameSubElements.begin(); iterFSE != frameSubElements.end(); iterFSE++)
        {
            // Check iAudioDataID against IABAudioDataDLC element
            dlcElement = dynamic_cast<IABAudioDataDLC*>(*iterFSE);
            if (dlcElement)
            {
                // found DLC element
                dlcElement->GetAudioDataID(audioDataID);
                
                // Check to see if the audioData ID matches the target
                if (audioDataID == iAudioDataID)
                {
                    // Decode to int32_t PCM samples
                    IABSampleRateType dlcSampleRate;
                    dlcElement->GetDLCSampleRate(dlcSampleRate);

                    if (dlcSampleRate == kIABSampleRate_96000Hz && render96kTo48k_)
                    {
                        // Force decoding sampling rate to 48k, from a 96k DLC element
                        errorCode = dlcElement->DecodeDLCToMonoPCM(sampleBufferInt_, numSamplePerRendererOutputChannel_, kIABSampleRate_48000Hz);
                    }
                    else
                    {
                        // Use the inherent sampling rate of DLC element for decoding
                        errorCode = dlcElement->DecodeDLCToMonoPCM(sampleBufferInt_, numSamplePerRendererOutputChannel_, dlcSampleRate);
                    }

                    if (kIABNoError != errorCode)
                    {
                        return errorCode;
                    }
                    
                    foundAudioElement = true;
                }
            }
            else
            {
                // Check iAudioDataID against IABAudioDataPCM element
                pcmElement = dynamic_cast<IABAudioDataPCM*>(*iterFSE);
                if (pcmElement)
                {
                    pcmElement->GetAudioDataID(audioDataID);
                    
                    // Check to see if the audioData ID matches the target
                    if (audioDataID == iAudioDataID)
                    {
                        uint32_t sampleCount = pcmElement->GetPCMSampleCount();
                        if (sampleCount != numSamplePerRendererOutputChannel_)
                        {
                            return kIABRendererSampleCountMismatchError;
                        }
                        
                        // Get audio samples
                        errorCode = pcmElement->UnpackPCMToMonoSamples(sampleBufferInt_, numSamplePerRendererOutputChannel_);
                        
                        if (kIABNoError != errorCode)
                        {
                            return errorCode;
                        }
                        
                        foundAudioElement = true;
                    }                    
                }
            }
            
            if (foundAudioElement)
            {
                float scaleFactor = kInt32BitMaxValue;  // scale to 32-bit signed integer range
                int32_t *decodedSample = sampleBufferInt_;
                IABSampleType *convertedSample = iOutputSampleBuffer;
                
                for (uint32_t i = 0; i < numSamplePerRendererOutputChannel_; i++)
                {
                    *convertedSample++ = static_cast<IABSampleType>(*decodedSample++ / scaleFactor);
                }
                
                sampleUpdated = true;
                break;
            }
        }

        if (sampleUpdated)
        {
            return kIABNoError;
        }
        else
        {
            // audio ID not found
            return kIABBadArgumentsError;
        }
    }

	// IABRenderer::UpdateAudioSampleBuffer() implementation
	iabError IABRenderer::UpdateAudioSampleBuffer(IABAudioDataIDType iAudioDataID)
	{
			return UpdateAudioSampleBuffer(iAudioDataID, sampleBufferFloat_);
	}
    
    // IABRenderer::ResetVBAPObject() implementation
    iabError IABRenderer::ResetVBAPObject()
    {
        if (!vbapObject_)
        {
            return kIABRendererNotInitialisedError;
        }
        
        vbapObject_->objectGain_ = 1.0f;
        vbapObject_->id_ = 0;
        vbapObject_->vbapNormGains_ = 0.0f;
        vbapObject_->extendedSources_.clear();
        
        std::vector<float>::iterator iter;
        
        for (iter = vbapObject_->channelGains_.begin(); iter != vbapObject_->channelGains_.end(); iter++)
        {
            // reset channel gains
            *iter = 0.0f;
        }
        
        return kIABNoError;
    }

	struct candidateSpeaker
	{
		candidateSpeaker(int32_t index, float posX, float posY, float posZ)
		{
			index_ = index;
			posX_ = posX;
			posY_ = posY;
			posZ_ = posZ;
		}

		int32_t index_;
		float posX_;
		float posY_;
		float posZ_;
	};

    // IABRenderer::FindSnapSpeakerIndex() implementation
    int32_t IABRenderer::FindSnapSpeakerIndex(CartesianPosInUnitCube iObjectPosition, float iSnapTolerance)
    {
        // Structure to save speaker info, used for resolving speaker selection when object position has
        // an equal displacement of smaller than snap tolerance, to more than 1 speakers.

        std::vector<candidateSpeaker> snapCandidates;
        
        int32_t speakerIndex = -1;
        float lastMax = 2.0f;   // initialise to 2.0 so that it will be replaced by the first match which is always <= 1.0
        float diffMax = 0.0f;
        float speakerX, speakerY, speakerZ;
        float objectPosX, objectPosY, objectPosZ;
        iObjectPosition.getIABObjectPosition(objectPosX, objectPosY, objectPosZ);
        
        std::map<int32_t, CartesianPosInUnitCube>::iterator iter;
        for (iter = vbapSpeakerChannelIABPositionMap_.begin(); iter != vbapSpeakerChannelIABPositionMap_.end(); iter++)
        {
            iter->second.getIABObjectPosition(speakerX, speakerY, speakerZ);
            diffMax = fabs(speakerX - objectPosX);
            diffMax = fmaxf(fabs(speakerY - objectPosY), diffMax);
            diffMax = fmaxf(fabs(speakerZ - objectPosZ), diffMax);

			// compare with current minimum if falling within iSnapTolerance range.
            if (diffMax < iSnapTolerance)
            {
                // If a candidate with closer coordinate is found, clear list, update lastMax and index.
                if (diffMax < lastMax)
                {
                    snapCandidates.clear();
                    lastMax = diffMax;
                    speakerIndex = iter->first;

					// Save it to vector for resolving multiple minimums.
					snapCandidates.push_back(candidateSpeaker(iter->first, speakerX, speakerY, speakerZ));
                }
                else if (diffMax == lastMax)
                {
					// Save multiple minimums to vector.
					snapCandidates.push_back(candidateSpeaker(iter->first, speakerX, speakerY, speakerZ));
				}
				else
				{
					// greater than current minimum, continue
					continue;
				}
            }
        }

        // Resolve multiple minimums using extra minimum absolute distance method.
		// (an improvement to previous random selection method.)
        if (snapCandidates.size() > 1)
        {
            float diffX = 0.0f;
            float diffY = 0.0f;
            float diffZ = 0.0f;
            double squaredDistance = 0.0f;      // Working variable for computing object distance from current speaker
            double lastClosestDistance = 3.0f;  // Tracks closest speaker distance, initialised to worst case:  sum of square of each direction (1 + 1 + 1)

            std::vector<candidateSpeaker>::iterator iterCandidate;
            for (iterCandidate = snapCandidates.begin(); iterCandidate != snapCandidates.end(); iterCandidate++)
            {
                diffX = fabs((*iterCandidate).posX_ - objectPosX);
                diffY = fabs((*iterCandidate).posY_ - objectPosY);
                diffZ = fabs((*iterCandidate).posZ_ - objectPosZ);
                
                // Calculate squared distance
                squaredDistance = diffX * diffX + diffY * diffY + diffZ * diffZ;
                
                // Compare to previous closest speaker and update if this speaker is closer
                if (squaredDistance < lastClosestDistance)
                {
                    lastClosestDistance = squaredDistance;
                    speakerIndex = (*iterCandidate).index_;
                }
            }
        }
        
        return speakerIndex;
    }

    // IABRenderer::IsSupported() implementation
    bool IABRenderer::IsSupported(IABFrameRateType iFrameRate, IABSampleRateType iSampleRate)
    {
        // ** Per requirement, IAB render v1.1 allows the following sample rate and frame rate combinations only:
        // 48kHz: 24fps, 48fps, 60fps, 120fps, 23.97fps.  (23.97fps is for IABAudioDataPCM only, not IABAudioDataDLC)
        // 96KHz: 24fps, 48fps
        //

		// add support for 2 more frame rates, 25fps & 30fps, at 48kHz sample rate only
		// These are added to support IABNearfieldRenderer v1.0 requirement. Tested with IAB NFR-CTP v1.0.
		// Code refactored for improved readability & maintainability
		bool isEnabledInCoreRenderer = false;							// default to false until set true

		if (iSampleRate == kIABSampleRate_48000Hz)
		{
			switch (iFrameRate)
			{
				// Enabled frame rates, for 48kHz sample rate
			case kIABFrameRate_24FPS:									// Core SDK 1.0, tested with core-CTP 1.0
			case kIABFrameRate_25FPS:									// IAB NFR v1.0, tested with IAB NFR-CTP 1.0
			case kIABFrameRate_30FPS:									// IAB NFR v1.0, tested with IAB NFR-CTP 1.0
			case kIABFrameRate_48FPS:									// Core SDK 1.0, tested with core-CTP 1.0
			case kIABFrameRate_60FPS:									// Core SDK 1.0, tested with core-CTP 1.0
			case kIABFrameRate_120FPS:									// Core SDK 1.0, tested with core-CTP 1.0
			case kIABFrameRate_23_976FPS:								// IAB NFR v1.0, tested with IAB NFR-CTP 1.0
				isEnabledInCoreRenderer = true;
				break;

				// Blocked/disabled frame rates, for 48kHz sample rate
			case kIABFrameRate_50FPS:
			case kIABFrameRate_96FPS:
			case kIABFrameRate_100FPS:
			default:
				// Disabled / blocked by default
				break;
			}
		}
		else if (iSampleRate == kIABSampleRate_96000Hz)
		{
			switch (iFrameRate)
			{
				// Enabled frame rates, for 96kHz sample rate
			case kIABFrameRate_24FPS:									// Core SDK 1.0, tested with core-CTP 1.0
			case kIABFrameRate_48FPS:									// Core SDK 1.0, tested with core-CTP 1.0

				isEnabledInCoreRenderer = true;
				break;

				// Blocked/disabled frame rates, for 96kHz sample rate
			case kIABFrameRate_25FPS:
			case kIABFrameRate_30FPS:
			case kIABFrameRate_50FPS:
			case kIABFrameRate_60FPS:
			case kIABFrameRate_96FPS:
			case kIABFrameRate_100FPS:
			case kIABFrameRate_120FPS:
			case kIABFrameRate_23_976FPS:
			default:
				// Disabled / blocked by default
				break;
			}
		}
		else;															// Disabled / blocked by default

		return isEnabledInCoreRenderer;
    }
    
} // namespace ImmersiveAudioBitstream
} // namespace SMPTE
