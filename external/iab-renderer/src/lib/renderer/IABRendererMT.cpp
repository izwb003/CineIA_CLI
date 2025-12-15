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
 * IABRendererMT.cpp
 *
 * @file
 */

#if __linux__ || __APPLE__

#include <assert.h>
#include <stack>
#include <vector>
#include <stdlib.h>
#include <math.h>
#if __linux__
#include <sys/sysinfo.h>
#elif __APPLE__
#include <unistd.h>
#endif

#include "IABDataTypes.h"
#include "common/IABElements.h"
#include "IABRendererMT.h"
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

#define     MAX_THREADPOOL_SIZE     8					// Maximum threadpool size. 
#define     MIN_THREADPOOL_SIZE     1					// Minimum threadpool size. 

#define     MAX_VBAP_CACHE_SIZE     250					// VBAP cache size, at which point to reset. 

#define     MAX_OUTPUT_CHANNELS     100                 // TODO: check what this might be



namespace SMPTE
{
namespace ImmersiveAudioBitstream
{

// Worker function for thread pool approach
//
void *MTRenderThreadWorker( void *iParam )
{
	ThreadWorkerFunctionParam* myThreadParam = static_cast<ThreadWorkerFunctionParam*> (iParam);
	IABRendererMT* rendererMT = myThreadParam->rendererMT_;
	std::queue<QueueJobParam>* jobQueue = rendererMT->jobQueue();

	// Engines persist throughout lifetime
	IABAudioAssetDecoder* myAssetDecoder = myThreadParam->threadAssetDecoder_;
	IABObjectRenderer* myObjectRenderer = myThreadParam->threadObjectRenderer_;
	IABBedRenderer* myBedRenderer = myThreadParam->threadBedRenderer_;

	QueueJobParam job;

	// Loop to end of program rendering
	while ( true )
	{
		// *****  MT critical section  *****
		// lock mutex for access job queue
        rendererMT->lockJobQueueMutex();

		// In worker threads, if job queu is empty, wait on condition
		// Use while instead of if to overcome supurious signal
		while (jobQueue->empty() && !rendererMT->doTerminate())
		{
			// Wait ...
            rendererMT->waitForJobQueueEntries();
			if (rendererMT->doTerminate())
			{
				break;
			}
		}

		if (rendererMT->doTerminate())
		{
            rendererMT->unlockJobQueueMutex();
			break;
		}

		// Otherwise (ie. not empty), continue and get a job
		job = jobQueue->front();				// Claim / get a job
		jobQueue->pop();						// Claimed, pop it from queue.
	
		// unlock
        rendererMT->unlockJobQueueMutex();
		// *****  End critical section  *****
	
		iabError iabReturnCode = kIABNoError;

		// Now let's do the claimed job
		//
		if (job.elementType_ == kIABElementID_AudioDataDLC)
		{
			// *** Decode DLC

			// Capture error, but current unhandled.
			// (Ming note: how to handle errors from thread worker is left as future improvement.)
			iabReturnCode = myAssetDecoder->DecodeIABAsset(
				job.assetDecodeParam_.iIABAudioDLC_
				, job.assetDecodeParam_.iOutputSampleBuffer
				);
		}
		else if (job.elementType_ == kIABElementID_AudioDataPCM)
		{
			// *** Unpack PCM
			
			// Capture error, but current unhandled.
			// (Ming note: how to handle errors from thread worker is left as future improvement.)
			iabReturnCode = myAssetDecoder->DecodeIABAsset(
				job.assetDecodeParam_.iIABAudioPCM_
				, job.assetDecodeParam_.iOutputSampleBuffer
				);
		}
		else if (job.elementType_ == kIABElementID_ObjectDefinition)
		{
			// *** Render object
			
			// Capture error, but current unhandled.
			// (Ming note: how to handle errors from thread worker is left as future improvement.)
			iabReturnCode = myObjectRenderer->RenderIABObject(
				*(job.objectRenderParam_.iIABObject_)
				, job.objectRenderParam_.oOutputChannels_
				, job.objectRenderParam_.iOutputChannelCount_
				, job.objectRenderParam_.iOutputSampleBufferCount_
			);
		}
		else if (job.elementType_ == kIABElementID_BedDefinition)
		{
			// *** Render bed
			
			// Capture error, but current unhandled.
			// (Ming note: how to handle errors from thread worker is left as future improvement.)
			iabReturnCode = myBedRenderer->RenderIABBed(
				*(job.bedRenderParam_.iIABBed_)
				, job.bedRenderParam_.oOutputChannels_
				, job.bedRenderParam_.iOutputChannelCount_
				, job.bedRenderParam_.iOutputSampleBufferCount_
			);
		}
		else;	// Ignore any other element types
	
        // Decrement job count, record any error that occurred
        rendererMT->decrementJobCount(iabReturnCode);

	}
	return NULL;
}

} // namespace ImmersiveAudioBitstream
} // namespace SMPTE



// Uncomment this line to enable IABRendererMT to issue errors or warning to std::err
//#define ENABLE_MTRENDERER_ERROR_REPORTING

namespace SMPTE
{
namespace ImmersiveAudioBitstream
{
	static const int32_t kIABDecorrTailingFrames = 2;

	// IsObjectActivatedForRendering() implementation
	bool IsObjectActivatedForRendering(const IABObjectDefinition* iIABObject, IABUseCaseType iTargetUseCase)
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
			else if ((objectUseCase == iTargetUseCase) && (objectUseCase != kIABUseCase_NoUseCase))
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

	// IsBedActivatedForRendering() implementation
	bool IsBedActivatedForRendering(const IABBedDefinition* iIABBed, IABUseCaseType iTargetUseCase)
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
			else if ((bedUseCase == iTargetUseCase) && (bedUseCase != kIABUseCase_NoUseCase))
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

	// IsBedRemapActivatedForRendering() implementation
	bool IsBedRemapActivatedForRendering(const IABBedRemap* iIABBedRemap, IABUseCaseType iTargetUseCase)
	{
		// It's a conditional. Check bed use case
		IABUseCaseType remapUseCase = kIABUseCase_NoUseCase;
		iIABBedRemap->GetRemapUseCase(remapUseCase);

		if (remapUseCase == kIABUseCase_Always)
		{
			// If always use - activated (This is logically true, but practically may not exist.)
			return true;
		}
		else if ((remapUseCase == iTargetUseCase) && (remapUseCase != kIABUseCase_NoUseCase))
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

	// IsSupported() implementation
	bool IsSupported(IABFrameRateType iFrameRate, IABSampleRateType iSampleRate)
	{
		// ** Per requirement, IAB render v1.1 allows the following sample rate and frame rate combinations only:
		// 48kHz: 24fps, 48fps, 60fps, 120fps, 23.97fps.  (23.97fps is for IABAudioDataPCM only, not IABAudioDataDLC)
		// 96KHz: 24fps, 48fps
		//

		// Add support for 2 more frame rates, 25fps & 30fps, at 48kHz sample rate only
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

    /****************************************************************************
    *
    * IABRendererMT
    *
    *****************************************************************************/

    // Create IABRendererMT instance
    IABRendererMTInterface* IABRendererMTInterface::Create(RenderUtils::IRendererConfiguration &iConfig, uint32_t iThreadPoolSize)
    {
        IABRendererMT* iabRenderer = NULL;
        iabRenderer = new IABRendererMT(iConfig, iThreadPoolSize);
        return iabRenderer;
    }
    
    // Deletes an IABRendererMT instance
    void IABRendererMTInterface::Delete(IABRendererMTInterface* iInstance)
    {
        delete iInstance;
    }

	// Constructor
	IABRendererMT::IABRendererMT(RenderUtils::IRendererConfiguration &iConfig, uint32_t iThreadPoolSize)
    {
		targetUseCase_ = kIABUseCase_NoUseCase;
        numRendererOutputChannels_ = 0;
		render96kTo48k_ = true;						// Default to true for SDK v1.x

		enableSmoothing_ = true;					// Sample smoothing is enabled by default
		enableDecorrelation_ = true;				// Object decorrelation per stream metadata is enabled by default

		decorrOutputBuffer_ = NULL;
		decorrOutputChannelPointers_ = NULL;

		assetSampleBuffer_ = NULL;
		assetSampleBufferPointers_ = NULL;

		objectRenderersAreInited_ = false;
		bedRenderersAreInited_ = false;
		assetDecodersAreInited_ = false;
		doTerminate_ = false;
		errorCode_ = kIABNoError;
		warningCode_ = kIABNoError;

        // Create the required nunber of
        // output channel mutexes
        perChOutputMutex.resize(kMaxOutputChannels);

		if (iThreadPoolSize == 0)
		{
			threadPoolSize_ = MIN_THREADPOOL_SIZE;					// Min threadpool size set to 1. (1 + frame rendering thread, so effectively 2 already. Not recommended.)
		}
		else if (iThreadPoolSize > MAX_THREADPOOL_SIZE )
		{
			threadPoolSize_ = MAX_THREADPOOL_SIZE;					// Max threadpool size set to 20.
		}
		else
		{
			threadPoolSize_ = iThreadPoolSize;		// as specified.
		}

		SetUp(iConfig);
	}

	// Destructor
	IABRendererMT::~IABRendererMT()
    {
		// Stop all running threads
		jobQueueMutex.lock();
		doTerminate_ = true;
		jobQueueHasEntries.broadcast();
		jobQueueMutex.unlock();

		for (uint32_t i = 0; i < threadPoolSize_; i++)
		{
			void* status;
			if (0 != pthread_join(threads_[i], &status))
			{
				// error
			}
		}

		if (iabDecorrelation_)
		{
			delete iabDecorrelation_;
		}

		if (vectDSP_)
		{
			delete vectDSP_;
		}
        
		delete[] decorrOutputBuffer_;
		delete[] decorrOutputChannelPointers_;

		delete[] assetSampleBuffer_;
		delete[] assetSampleBufferPointers_;

		// New, MT

		for (std::vector<IABAudioAssetDecoder*>::iterator iterAssetDec = iabAssetDecoders_.begin(); iterAssetDec != iabAssetDecoders_.end(); iterAssetDec++)
		{
			delete *iterAssetDec;
		}
		iabAssetDecoders_.clear();

		for (std::vector<AssetDecoderMTWorkerParam*>::iterator iterAssetParam = assetWorkerParams_.begin(); iterAssetParam != assetWorkerParams_.end(); iterAssetParam++)
		{
			delete *iterAssetParam;
		}
		assetWorkerParams_.clear();

		for (std::vector<IABObjectRenderer*>::iterator iterObjRender = iabObjectRenderers_.begin(); iterObjRender != iabObjectRenderers_.end(); iterObjRender++)
		{
			delete *iterObjRender;
		}
		iabObjectRenderers_.clear();

		for (std::vector<ObjectRendererMTWorkerParam*>::iterator iterObjParam = objectWorkerParams_.begin(); iterObjParam != objectWorkerParams_.end(); iterObjParam++)
		{
			delete *iterObjParam;
		}
		objectWorkerParams_.clear();

		for (std::vector<IABBedRenderer*>::iterator iterBedRender = iabBedRenderers_.begin(); iterBedRender != iabBedRenderers_.end(); iterBedRender++)
		{
			delete *iterBedRender;
		}
		iabBedRenderers_.clear();

		for (std::vector<BedRendererMTWorkerParam*>::iterator iterBedParam = bedWorkerParams_.begin(); iterBedParam != bedWorkerParams_.end(); iterBedParam++)
		{
			delete *iterBedParam;
		}
		bedWorkerParams_.clear();

		for (std::vector<ThreadWorkerFunctionParam*>::iterator iterThreadFuncParam = threadFunctionParameters_.begin(); iterThreadFuncParam != threadFunctionParameters_.end(); iterThreadFuncParam++)
		{
			delete *iterThreadFuncParam;
		}
		threadFunctionParameters_.clear();
	
	}

	// IABRendererMT::SetUp() implementation
	void IABRendererMT::SetUp(RenderUtils::IRendererConfiguration &iConfig)
	{
        // ===========================================================
        // Set up IAB renderer with passed in renderer configuration

		// For collecting parameters that are necessary for setting up ObjectRenderer objects.
		ObjectRendererParam objectRendererParam;

		// For collecting parameters that are necessary for setting up BedRenderer objects.
		BedRendererParam bedRendererParam;

		// For collecting parameters that are necessary for setting up asset decoder objects.
		AssetDecoderParam assetDecoderParam;

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
        // IAB configuration files now support virtual speakers.
        
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
		// this function, "void IABRendererMT::SetUp()".
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

		// Allocate buffers for holding decoded asset samples from DLC and/or PCM audio data elements
		//

		// kIABMaxAudioDataElementsInFrame48000Hz = 128, Max number of asset element per frame
		assetSampleBuffer_ = new IABSampleType[kIABMaxAudioDataElementsInFrame48000Hz * kIABMaxFrameSampleCount];
		assetSampleBufferPointers_ = new IABSampleType*[kIABMaxAudioDataElementsInFrame48000Hz];

		// Set up deccode asset buffer pointer array
		for (uint32_t i = 0; i < kIABMaxAudioDataElementsInFrame48000Hz; i++)
		{
			assetSampleBufferPointers_[i] = assetSampleBuffer_ + i * kIABMaxFrameSampleCount;
		}

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

		// *** MT set up

		threads_.resize(threadPoolSize_);

		// Create threadPoolSize_ instances
		for (uint32_t i = 0; i < threadPoolSize_; i++)
		{
			// asset decoder pool
			iabAssetDecoders_.push_back(new IABAudioAssetDecoder);
			assetWorkerParams_.push_back(new AssetDecoderMTWorkerParam);

			// object renderer pool
			iabObjectRenderers_.push_back(new IABObjectRenderer(gainsHistoryMutex, perChOutputMutex));
			objectWorkerParams_.push_back(new ObjectRendererMTWorkerParam);

			// bed renderer pool
			iabBedRenderers_.push_back(new IABBedRenderer(perChOutputMutex));
			bedWorkerParams_.push_back(new BedRendererMTWorkerParam);

			// thread function parameters pool
			threadFunctionParameters_.push_back(new ThreadWorkerFunctionParam);
		}


		// *** Init objectRendererParam for setting up member object renderer
		//
		objectRendererParam.numRendererOutputChannels_ = numRendererOutputChannels_;
		objectRendererParam.targetUseCase_ = targetUseCase_;
		objectRendererParam.render96kTo48k_ = render96kTo48k_;
		objectRendererParam.frameEntityGainHistory_ = &frameEntityGainHistory_;

		// Also init objectSubBlockRendererParam for setting up the objectSubBlockRenderer contained in object renderer
		objectRendererParam.objectSubBlockRendererParam_.numRendererOutputChannels_ = numRendererOutputChannels_;
		objectRendererParam.objectSubBlockRendererParam_.speakerCount_ = speakerCount_;
		objectRendererParam.objectSubBlockRendererParam_.renderConfig_ = &iConfig;
		objectRendererParam.objectSubBlockRendererParam_.enableSmoothing_ = enableSmoothing_;

		// *** Init bedRendererParam for setting up member bed renderer
		//
		bedRendererParam.numRendererOutputChannels_ = numRendererOutputChannels_;
		bedRendererParam.targetUseCase_ = targetUseCase_;
		bedRendererParam.render96kTo48k_ = render96kTo48k_;

		// Also init bedChannelRendererParam for setting up the IABBedChannelRenderer contained in bed renderer
		// lhs is a pointer to map.
		bedRendererParam.bedChannelRendererParam_.physicalURIedBedSpeakerOutputIndexMap_ = &physicalURIedBedSpeakerOutputIndexMap_;
		bedRendererParam.bedChannelRendererParam_.speakerIndexToOutputIndexMap_ = &speakerIndexToOutputIndexMap_;
		bedRendererParam.bedChannelRendererParam_.totalURIedSpeakerToTotalSpeakerListMap_ = &totalURIedSpeakerToTotalSpeakerListMap_;
		// both sides are pointers
		bedRendererParam.bedChannelRendererParam_.totalSpeakerList_ = totalSpeakerList_;

		bedRendererParam.bedChannelRendererParam_.numRendererOutputChannels_ = numRendererOutputChannels_;
		bedRendererParam.bedChannelRendererParam_.speakerCount_ = speakerCount_;
		bedRendererParam.bedChannelRendererParam_.renderConfig_ = &iConfig;

		// And init bedRemapRendererParam for setting up the IABBedRemapRenderer contained in bed renderer
		// lhs is a pointer to map.
		bedRendererParam.bedRemapRendererParam_.physicalURIedBedSpeakerOutputIndexMap_ = &physicalURIedBedSpeakerOutputIndexMap_;
		bedRendererParam.bedRemapRendererParam_.speakerIndexToOutputIndexMap_ = &speakerIndexToOutputIndexMap_;
		bedRendererParam.bedRemapRendererParam_.totalURIedSpeakerToTotalSpeakerListMap_ = &totalURIedSpeakerToTotalSpeakerListMap_;
		// both sides are pointers
		bedRendererParam.bedRemapRendererParam_.totalSpeakerList_ = totalSpeakerList_;

		bedRendererParam.bedRemapRendererParam_.numRendererOutputChannels_ = numRendererOutputChannels_;
		bedRendererParam.bedRemapRendererParam_.speakerCount_ = speakerCount_;
		bedRendererParam.bedRemapRendererParam_.renderConfig_ = &iConfig;

		// *** Init assetDecoderParam for setting up member asset decoder
		//
		assetDecoderParam.render96kTo48k_ = render96kTo48k_;

		// *** Initialize pool of instances, and worker params with Config related parameters
		// (This is only partial initialization. Other parameters depend on pasred frames, which
		// will be used, once only during rendering first frame, to complete initialiation.)
		//
		for (uint32_t i = 0; i < threadPoolSize_; i++)
		{
			iabAssetDecoders_[i]->SetUp(assetDecoderParam);
			iabObjectRenderers_[i]->SetUp(objectRendererParam);
			objectWorkerParams_[i]->iOutputChannelCount_ = numRendererOutputChannels_;
			iabBedRenderers_[i]->SetUp(bedRendererParam);
			bedWorkerParams_[i]->iOutputChannelCount_ = numRendererOutputChannels_;

			// For queue based approach, processing engines are pre-setup for each threads
			// and persist through lifetime of rendering a programs
			threadFunctionParameters_[i]->threadAssetDecoder_ = iabAssetDecoders_[i];
			threadFunctionParameters_[i]->threadObjectRenderer_ = iabObjectRenderers_[i];
			threadFunctionParameters_[i]->threadBedRenderer_ = iabBedRenderers_[i];
			threadFunctionParameters_[i]->rendererMT_ = this;
		}

		// Also set up configuration-related parameters to jobParameterCarrier_ that last unchanged 
		// throughout instance life cycle (ie. rendering frames from the same program).
		//
		jobParameterCarrier_.objectRenderParam_.iOutputChannelCount_ = numRendererOutputChannels_;
		jobParameterCarrier_.bedRenderParam_.iOutputChannelCount_ = numRendererOutputChannels_;

		// Create threadpool (containing threadPoolSize_ of threads)
		for (uint32_t i = 0; i < threadPoolSize_; i++)
		{
			pthread_create(&(threads_[i]), NULL, MTRenderThreadWorker, (void*) threadFunctionParameters_[i]);
		}
	}

	// IABRendererMT::GetOutputChannelCount() implementation
	IABRenderedOutputChannelCountType IABRendererMT::GetOutputChannelCount() const
    {
        return numRendererOutputChannels_;
    }
    
	// IABRendererMT::GetMaxOutputSampleCount() implementation
	IABRenderedOutputSampleCountType IABRendererMT::GetMaxOutputSampleCount() const
    {
        return kIABMaxFrameSampleCount;
    }

	// IABRendererMT::GetProcessorCoreNumber() implementation
	uint32_t IABRendererMT::GetProcessorCoreNumber()
	{
#if __linux__
		return get_nprocs();
#elif __APPLE__
		return static_cast<uint32_t>(sysconf(_SC_NPROCESSORS_ONLN));
#else
		return 0;
#endif
	}

	// IABRendererMT::RenderIABFrame() implementation
	iabError IABRendererMT::RenderIABFrame(const IABFrameInterface& iIABFrame
                                         , IABSampleType **oOutputChannels
                                         , IABRenderedOutputChannelCountType iOutputChannelCount
                                         , IABRenderedOutputSampleCountType iOutputSampleBufferCount)
    {
        bool hasDecorrObjects = false;
        iabError ec = kIABNoError;	// Copy of error code from threaded job
        warningCode_ = kIABNoError;	// Initialize warning for every frame

		// For collecting frame parameters that are necessary for setting up ObjectRenderer objects, etc.
		FrameParam frameParam;

		// Get frame rate of iIABFrame
        iIABFrame.GetFrameRate(frameRate_);

		// Get sample rate of iIABFrame
		iIABFrame.GetSampleRate(sampleRate_);
        
		// Report error if the frame sample rate is anything other than 48kHz or 96kHz
		if ( (sampleRate_ != kIABSampleRate_48000Hz) && (sampleRate_ != kIABSampleRate_96000Hz) )
        {
            return kIABRendererUnsupportedSampleRateError;
        }

		// Check to see if frame rate and sample rate combination is supporteoOutputChannels
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
			
        // Check input parameters
        if ((iOutputChannelCount != numRendererOutputChannels_) ||
            (iOutputSampleBufferCount != frameSampleCount_) ||
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

		// Update gains cache at beginning of rendering an IAB Frame
		// Update on cross-frame past gains history must be enabled for smoothing to process correctly.
		// (though smoothing can be disabled through configuration file loaded.)
		//
		UpdateFrameGainsHistory();

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
            return kIABNoError;
		}

		// Asset decoders to be initialized with minimal frame parameters here before decoding/unpacking
		//
		if (!assetDecodersAreInited_)
		{
			frameParam.frameSampleCount_ = frameSampleCount_;

			// Initialize frame parameters to iabAssetDecoders_ pool, to complete asset decoder init.
			// This is done once only for IABRendererMT instance life cycle.
			//
			for (uint32_t i = 0; i < threadPoolSize_; i++)
			{
				iabAssetDecoders_[i]->SetFrameParameter(frameParam);
			}

			// Mark flag
			assetDecodersAreInited_ = true;
		}
		else;
		
		// *****************************************
		// init frameParam for setting up member object/bed renderers with frame-related parameters
		// Need to do this only once, assuming frame params remain the same across the entire program

		// For object renderers
		//
		if (!objectRenderersAreInited_)
		{
			frameParam.iabFrameToRender_ = iabFrameToRender_;
			frameParam.frameRate_ = frameRate_;
			frameParam.frameSampleCount_ = frameSampleCount_;
			frameParam.numPanSubBlocks_ = numPanSubBlocks_;
			frameParam.frameAudioDataIDToAssetPointerMap_ = &frameAudioDataIDToAssetPointerMap_;

			// Initialize frame parameters to iabObjectRenderers_ pool, to complete object renderers init.
			// This is done once only for IABRendererMT instance lifetime.
			//
			for (uint32_t i = 0; i < threadPoolSize_; i++)
			{
				iabObjectRenderers_[i]->SetFrameParameter(frameParam);
			}

			// Mark flag
			objectRenderersAreInited_ = true;
		}

		// For bed renderers
		//
		if (!bedRenderersAreInited_)
		{
			frameParam.iabFrameToRender_ = iabFrameToRender_;
			frameParam.frameRate_ = frameRate_;
			frameParam.frameSampleCount_ = frameSampleCount_;
			frameParam.numPanSubBlocks_ = numPanSubBlocks_;
			frameParam.frameAudioDataIDToAssetPointerMap_ = &frameAudioDataIDToAssetPointerMap_;

			// Initialize frame parameters to iabBedRenderers_ pool, to complete bed renderers init.
			// This is done once only for IABRendererMT instance lifetime.
			//
			for (uint32_t i = 0; i < threadPoolSize_; i++)
			{
				iabBedRenderers_[i]->SetFrameParameter(frameParam);
			}

			// Mark flag
			bedRenderersAreInited_ = true;
		}

		// *** Set up worker params with frame-related parameters.
		// Further params such as iIABObject_, etc will be assigned per
		// individual threads.
		//
		for (uint32_t i = 0; i < threadPoolSize_; i++)
		{
			objectWorkerParams_[i]->iOutputSampleBufferCount_ = frameSampleCount_;
			bedWorkerParams_[i]->iOutputSampleBufferCount_ = frameSampleCount_;
			bedWorkerParams_[i]->oOutputChannels_ = oOutputChannels;
		}

		// *** End of params initialization

		// Check on VBAP cache sizes from all MT object renderers, and reset if necessary.
		// (This is to ensure caches do not grow out of control to take up too much resources.)

		uint32_t largestVBAPCacheSize = 0;
		uint32_t cacheSize = 0;

		// Note that there is no mutex protection for this part. None of the VBAP engines are active at this point.
		for (uint32_t i = 0; i < threadPoolSize_; i++)
		{
			cacheSize = iabObjectRenderers_[i]->GetSubblockVBAPCacheSize();

			if (largestVBAPCacheSize < cacheSize)
			{
				largestVBAPCacheSize = cacheSize;
			}
		}

		if (largestVBAPCacheSize > MAX_VBAP_CACHE_SIZE)
		{
			for (uint32_t i = 0; i < threadPoolSize_; i++)
			{
				iabObjectRenderers_[i]->ClearSubblockVBAPCache();
			}
		}

		// *** Go through frame sub-elements and add jobs to job queue
		//    1. First batch: asset only, DLC and PCM. Make sure decoding is complete before 2nd batch
		//    2. Second batch: objects and beds. Make sure rendering is complete before decorrelation
		//
		if (!renderJobQueue_.empty())
		{
			// Queue no empty to start with, error.
			return kIABRendererGeneralError;
		}

		// Variable to save queue size for checking
		size_t currentQueueSize = 0;

		// Initiate frame-related parameters to jobParameterCarrier_ that are common to object and beds.
		// (Even it is done every frame here, it is not expected to change throughout IABRendererMT instance 
		// lifetime, ie. rendering frames from the same program.)
		//
		jobParameterCarrier_.objectRenderParam_.iOutputSampleBufferCount_ = frameSampleCount_;
		jobParameterCarrier_.bedRenderParam_.iOutputSampleBufferCount_ = frameSampleCount_;

		// Create asset decoding queue for frame
		//
		frameAudioDataIDToAssetPointerMap_.clear();
		IABElementIDType elementID;
		uint32_t assetCount = 0;								// Counting number of asset elements and make it does not exceed 128.

		// *****  MT critical section  *****

		// lock mutex for adding entries (asset DLC and PCM elements only) to job queue
		jobQueueMutex.lock();

		for (IABElementCountType i = 0; i < subElementCount; i++)
		{
			// Only process the asset if the audio data ID is non-zero
			IABAudioDataIDType elementAudioDataID = 0;

			frameSubElements[i]->GetElementID(elementID);

			if (kIABElementID_AudioDataDLC == elementID)
			{
				// Indicate DLC element (important)
				jobParameterCarrier_.elementType_ = kIABElementID_AudioDataDLC;

				// Set up DLC params
				jobParameterCarrier_.assetDecodeParam_.iIABAudioDLC_ = (dynamic_cast<IABAudioDataDLC*>(frameSubElements[i]));	// DLC to be decoded
				jobParameterCarrier_.assetDecodeParam_.iIABAudioDLC_->GetAudioDataID(elementAudioDataID);
			}
			else if (kIABElementID_AudioDataPCM == elementID)
			{
				// Indicate PCM element (important)
				jobParameterCarrier_.elementType_ = kIABElementID_AudioDataPCM;

				// Set up PCM params
				jobParameterCarrier_.assetDecodeParam_.iIABAudioPCM_ = (dynamic_cast<IABAudioDataPCM*>(frameSubElements[i]));	// PCM to unpack
				jobParameterCarrier_.assetDecodeParam_.iIABAudioPCM_->GetAudioDataID(elementAudioDataID);
			}
			else;		// Not adding any other elements to queues at this stage. Ensure asset are completely decoded.

			if (elementAudioDataID != 0)
			{
				// Check if kIABMaxAudioDataElementsInFrame48000Hz (128) has been reached ...
				if (assetCount == kIABMaxAudioDataElementsInFrame48000Hz)
				{
					// unlock before exit
					jobQueueMutex.unlock();

					return kIABRendererAssetNumberExceedsMax;
				}

				jobParameterCarrier_.assetDecodeParam_.iOutputSampleBuffer = assetSampleBufferPointers_[assetCount];			// Where to output decoded PCM samples

				// Push to job queue
				renderJobQueue_.push(jobParameterCarrier_);

				// Add to map entry (one of threads to decode assets later)
				frameAudioDataIDToAssetPointerMap_.insert(std::pair<IABAudioDataIDType, IABSampleType*>(elementAudioDataID, assetSampleBufferPointers_[assetCount]));

				// Increment assetCount
				assetCount++;
			}
		}

		// Get queue size after adding asset entries
		currentQueueSize = renderJobQueue_.size();

		// lock mutex jobCountMutex for access jobCount
		jobCountMutex.lock();
		// set number of jobs
		jobCount = currentQueueSize;
		// unlock jobCountMutex. We are back to jobQueueMutex only.
		jobCountMutex.unlock();

		// unlock jobQueueMutex to let the threads execute
		jobQueueMutex.unlock();

		// Signal all worker threads to wake up (only if there are asset decoding/unpackingjobs in the queue).
		jobQueueHasEntries.broadcast();

		// lock mutex jobCountMutex for access jobCount
		jobCountMutex.lock();
		// Check jobCount. Either it's decremented to 0, or we'll wait on jobCompletion
		// Use while to overcome supurious signal
		while (jobCount > 0)
		{
			// Wait for jobCompletion
			jobCompletion.wait(jobCountMutex);
		}

		// With jobCountMutex locked, get error from any processed job
		ec = errorCode_;

		// unlock jobCountMutex
		jobCountMutex.unlock();

		// Handle any error
		if (kIABNoError != ec)
		{
			return ec;
		}

		// Resume to prepare next batch of queue.

		// lock mutex for adding objects and beds to job queue
		jobQueueMutex.lock();

		// Ading objects and beds to queue, here
		//
		for (IABElementCountType i = 0; i < subElementCount; i++)
		{
			frameSubElements[i]->GetElementID(elementID);

			if (kIABElementID_ObjectDefinition == elementID)
			{
				IABObjectDefinition* elementToRender = dynamic_cast<IABObjectDefinition*>(frameSubElements[i]);

				// Indicate object element (important)
				jobParameterCarrier_.elementType_ = kIABElementID_ObjectDefinition;

				// Set up object params
				jobParameterCarrier_.objectRenderParam_.iIABObject_ = elementToRender;					// object to be rendered

				IABMetadataIDType objectMetaID;
				elementToRender->GetMetadataID(objectMetaID);

				// Adding object decorr support for binary On/Off mode only.
				// - "No decorr": Off (ObjectDecorCoefPrefix = 0x0)
				// - "Max decorr" : On (ObjectDecorCoefPrefix = 0x1)
				// - Not supported : (ObjectDecorCoefPrefix = 0x2), effectively "No decorr"
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
					jobParameterCarrier_.objectRenderParam_.oOutputChannels_ = decorrOutputChannelPointers_;

					// Set flag hasDecorrObjects to true, to indicate presence of decorr object(s).
					hasDecorrObjects = true;
				}
				else
				{
					// For all other cases, Decorr == OFF.
					// Route rendered output as normal.
					//
					jobParameterCarrier_.objectRenderParam_.oOutputChannels_ = oOutputChannels;
				}

				// Push to job queue
				renderJobQueue_.push(jobParameterCarrier_);
			}
			else if (kIABElementID_BedDefinition == elementID)
			{
				// Indicate bed element (important)
				jobParameterCarrier_.elementType_ = kIABElementID_BedDefinition;

				// Set up bed params
				jobParameterCarrier_.bedRenderParam_.iIABBed_ = (dynamic_cast<IABBedDefinition*>(frameSubElements[i]));				// bed to be rendered
				jobParameterCarrier_.bedRenderParam_.oOutputChannels_ = oOutputChannels;

				// Push to job queue
				renderJobQueue_.push(jobParameterCarrier_);
			}
			else;		// Not adding anything else
		}

		// New batch of queue ready - the rendering queue.
		// Get queue size
		currentQueueSize = renderJobQueue_.size();

		// lock mutex jobCountMutex for access jobCount
		jobCountMutex.lock();
		// set number of jobs
		jobCount = currentQueueSize;
		// unlock jobCountMutex. We are back to jobQueueMutex only.
		jobCountMutex.unlock();

		// unlock jobQueueMutex to let the threads execute
		jobQueueMutex.unlock();

		// Signal all worker threads to wake up (only if there are asset decoding/unpackingjobs in the queue).
		jobQueueHasEntries.broadcast();

		// lock mutex jobCountMutex for access jobCount
		jobCountMutex.lock();
		// Check jobCount. Either it's decremented to 0, or we'll wait on jobCompletion
		// Use while to overcome supurious signal
		while (jobCount > 0)
		{
			// Wait for jobCompletion
			jobCompletion.wait(jobCountMutex);
		}

		// With jobCountMutex locked, get error from any processed job
		ec = errorCode_;

		// unlock jobCountMutex
		jobCountMutex.unlock();

		if (kIABNoError != ec) {
			return ec;
		}

		// Resume to prepare next batch of queue.

		// Done rendering. Ready to move on to decorrelation in this thread.
		//

		// **** Process object decorrelation here
		//

		// Does the frame contain decorr objects?
		//
		if (hasDecorrObjects)
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
                                                       , frameSampleCount_);

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
                             , frameSampleCount_);
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

		if (kIABNoError != warningCode_)
		{
			// Issue warning
			return warningCode_;
		}

        return kIABNoError;
    }

	void IABRendererMT::decrementJobCount(iabError errorCode)
	{
		// lock mutex jobCountMutex for access jobCount
		jobCountMutex.lock();
		jobCount--;

		if (kIABNoError != errorCode)
		{
			switch (errorCode)
			{
				case kIABRendererNoLFEInConfigForBedLFEWarning:
				case kIABRendererNoLFEInConfigForRemapLFEWarning:
				case kIABRendererEmptyObjectZoneWarning:
					// Keep warnings separate
					warningCode_ = errorCode;
					break;
				default:
					errorCode_ = errorCode;
					break;
			}
		}

		// Check if jobCount has reached 0. If yes, we have just completed the last job.
		if (jobCount == 0)
		{
			// Signal to frame rendering thread (ie. queue creator) that the batch of jobs in queue is completed.
			jobCompletion.signal();
		}

		// proceed to unlock the 2nd mutex. We are back to jobQueueMutex only.
		jobCountMutex.unlock();
	}

	// IABRendererMT::UpdateFrameGainsHistory() implementation
	void IABRendererMT::UpdateFrameGainsHistory()
	{
		// *****  MT critical section  *****
		// lock mutex for gains history
		gainsHistoryMutex.lock();

		// reset object gain history
		for (std::map<uint32_t, IABGAINSPROC::EntityPastChannelGains>::iterator iter = frameEntityGainHistory_.begin(); iter != frameEntityGainHistory_.end();)
		{
			if (!(*iter).second.touched_)
			{
				std::map<uint32_t, IABGAINSPROC::EntityPastChannelGains>::iterator tmpIter = iter;
				++iter;

				frameEntityGainHistory_.erase(tmpIter);
			}
			else
			{
				(*iter).second.touched_ = false;
				++iter;
			}
		}

		// unlock
		gainsHistoryMutex.unlock();
		// *****  End critical section  *****
	}

	// IABRendererMT::ResetFrameGainsHistory() implementation
	void IABRendererMT::ResetFrameGainsHistory()
	{
		// *****  MT critical section  *****
		// lock mutex for gains history
		gainsHistoryMutex.lock();

		// reset smoothing gain histories
		frameEntityGainHistory_.clear();

		// unlock
		gainsHistoryMutex.unlock();
		// *****  End critical section  *****
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


	/****************************************************************************
	* IABObjectSubBlockRenderer
	*****************************************************************************/

	// Default constructor
	IABObjectSubBlockRenderer::IABObjectSubBlockRenderer(std::vector<IABMutex>& perChOutputMutex)
    :perChOutputMutex_(perChOutputMutex)
	{
		speakerCount_ = 0;
		numRendererOutputChannels_ = 0;
		enableSmoothing_ = true;					// Sample smoothing is enabled by default

		vbapRenderer_ = NULL;
		channelGainsProcessor_ = NULL;
		iabObjectZone9_ = NULL;
	}

	// Destructor
	IABObjectSubBlockRenderer::~IABObjectSubBlockRenderer()
	{
		if (vbapRenderer_)
		{
			delete vbapRenderer_;
		}

		if (channelGainsProcessor_)
		{
			delete channelGainsProcessor_;
		}

		if (iabObjectZone9_)
		{
			delete iabObjectZone9_;
		}
	}

	// IABObjectSubBlockRenderer::SetUp() implementation
	iabError IABObjectSubBlockRenderer::SetUp(ObjectSubBlockRendererParam &iObjectSubBlockRendererParam)
	{
		// Number of output channels for the current target config. 
		// This number euqals to # of physical speakers. An output must be generated from renderer for physical speakers.
		numRendererOutputChannels_ = iObjectSubBlockRendererParam.numRendererOutputChannels_;

		// Number of "speakers" for the current target config.
		// Despite the terminology inherited from render-config days, this number equals to "physical + virtual" speakers.
		// So speakerCount_ >= numRendererOutputChannels_.
		speakerCount_ = iObjectSubBlockRendererParam.speakerCount_;

		// Create and set up VBAP renderer
		vbapRenderer_ = new IABVBAP::VBAPRenderer();
		vbapRenderer_->InitWithConfig(iObjectSubBlockRendererParam.renderConfig_);

		// Create a channel gain processor (engine)
		// This is for applying channel gains to asset sample to generate channel output.
		// (For multi-threading, consider implementing these directly in object subblock renderer
		// instead of separate class...)
		channelGainsProcessor_ = new IABGAINSPROC::ChannelGainsProcessorMT(perChOutputMutex_);

		// Smoothing ON/OFF flag (render config based)
		enableSmoothing_ = iObjectSubBlockRendererParam.enableSmoothing_;

		// Initialize map table vbapSpeakerToIABPositionMap_
		InitVBAPSpeakerToIABPositionMap(*(iObjectSubBlockRendererParam.renderConfig_));

		// Create and set up object zone 9 processor
		iabObjectZone9_ = new IABObjectZone9(*(iObjectSubBlockRendererParam.renderConfig_));

#ifdef ENABLE_MTRENDERER_ERROR_REPORTING
        
        if (!iabObjectZone9_ || !iabObjectZone9_->isInitialised())
        {
            std::cerr << "Warning: Unable to properly initialise object zone 9 with the renderer configuration.\n";
            std::cerr << "Object zone support disabled for target configuration.\n";
        }
        
#endif

		return kIABNoError;
	}

	// IABObjectSubBlockRenderer::GetVBAPCacheSize() implementation
	uint32_t IABObjectSubBlockRenderer::GetVBAPCacheSize()
	{
		return vbapRenderer_->GetVBAPCacheSize();
	}

	// IABObjectSubBlockRenderer::ClearVBAPCache() implementation
	void IABObjectSubBlockRenderer::ClearVBAPCache()
	{
		if (vbapRenderer_)
		{
			// Clear VBAPRenderer extendedsource cache
			vbapRenderer_->ResetPreviouslyRendered();
		}
	}

	// IABObjectSubBlockRenderer::InitVBAPSpeakerToIABPositionMap() implementation
	iabError IABObjectSubBlockRenderer::InitVBAPSpeakerToIABPositionMap(RenderUtils::IRendererConfiguration &iConfig)
	{
		// Clear if to be init'ed
		vbapSpeakerToIABPositionMap_.clear();

		// Speaker data structures from the renderer configuration, Physical speakers only (for snap)
		const std::vector<RenderUtils::RenderSpeaker>*	physicalSpeakerList;

		// Get speaker list from configuration
		iConfig.GetPhysicalSpeakers(physicalSpeakerList);

		// Convert config file speaker VBAP coordinates to IAB unit cube coordinates to support object snapping
		// Converted coordinates are stored in vbapSpeakerToIABPositionMap_
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
		for (iter = (*physicalSpeakerList).begin(); iter != (*physicalSpeakerList).end(); iter++)
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
					vbapSpeakerToIABPositionMap_.clear();
					break;
				}

				iabReturnCode = IABPosition.setIABObjectPosition(speakerIABPosX, speakerIABPosY, speakerIABPosZ);

				if (kIABNoError != iabReturnCode)
				{
					vbapSpeakerToIABPositionMap_.clear();
					break;
				}

				vbapSpeakerToIABPositionMap_[iter->outputIndex_] = IABPosition;
			}
		}

		return iabReturnCode;
	}

	// IABObjectSubBlockRenderer::RenderIABObjectSubBlock() implementation
	iabError IABObjectSubBlockRenderer::RenderIABObjectSubBlock(const IABObjectSubBlockInterface& iIABObjectSubBlock
		, IABGAINSPROC::EntityPastChannelGains& ioChannelGains
		, IABVBAP::vbapRendererObject *iVbapObject
		, IABSampleType *iAssetSamples
		, IABSampleType **oOutputChannels
		, IABRenderedOutputChannelCountType iOutputChannelCount
		, IABRenderedOutputSampleCountType iOutputSampleBufferCount)
	{
		// Check input parameters
		if (!iVbapObject ||
			!iAssetSamples ||
			(iOutputChannelCount == 0) ||
			(iOutputSampleBufferCount == 0))
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

			// Spread has priority over snap: Disable snap when spread is > 0
			if (!objectHasSpread)
			{
				// Check if object snap should be activated
				IABObjectSnap objectSnap;
				iIABObjectSubBlock.GetObjectSnap(objectSnap);

				if (objectSnap.objectSnapPresent_)
				{
					// Convert quantized value to float range [0.0, 1.0]
					float snapTolerance = static_cast<float>((objectSnap.objectSnapTolerance_) / 4095.0f);

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
				std::vector<IABVBAP::vbapRendererExtendedSource>& extendedSources = iVbapObject->extendedSources_;
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

				// Set vbap object gain
				//
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
		gainsProceReturnCode = channelGainsProcessor_->ApplyChannelGains(iVbapObject->id_
			, ioChannelGains
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

		return kIABNoError;
	}

	// IABObjectSubBlockRenderer::FindSnapSpeakerIndex() implementation
	int32_t IABObjectSubBlockRenderer::FindSnapSpeakerIndex(CartesianPosInUnitCube iObjectPosition, float iSnapTolerance)
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
		for (iter = vbapSpeakerToIABPositionMap_.begin(); iter != vbapSpeakerToIABPositionMap_.end(); iter++)
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

	/****************************************************************************
	* IABObjectRenderer
	*****************************************************************************/

	// Default constructor
	IABObjectRenderer::IABObjectRenderer(IABMutex& gainsHistoryMutex, std::vector<IABMutex>& perChOutputMutex)
    :subBlockRenderer_(perChOutputMutex),
    gainsHistoryMutex_(gainsHistoryMutex)
	{
		targetUseCase_ = kIABUseCase_NoUseCase;
		numRendererOutputChannels_ = 0;
		vbapObject_ = NULL;
		outputBufferPointers_ = NULL;
		render96kTo48k_ = true;						// Default to true for SDK v1.x
	}

	// Destructor
	IABObjectRenderer::~IABObjectRenderer()
	{
		if (vbapObject_)
		{
			delete vbapObject_;
		}

		delete[] outputBufferPointers_;
	}

	// IABObjectRenderer::SetUp() implementation
	iabError IABObjectRenderer::SetUp(ObjectRendererParam &iObjectRendererParam)
	{
		// UseCase for the current target config 
		targetUseCase_ = iObjectRendererParam.targetUseCase_;

		// Number of output channels for the current target config 
		numRendererOutputChannels_ = iObjectRendererParam.numRendererOutputChannels_;

		// Flag to enable/disable 96k IAB stream rendering to 48k output.
		render96kTo48k_ = iObjectRendererParam.render96kTo48k_;

		// Frame gains history.
		frameEntityGainHistory_ = iObjectRendererParam.frameEntityGainHistory_;

		// Create a vbap object, an object for modelling IAB rendering parameters into
		// an object that VBAPRenderer can render on.
		vbapObject_ = new IABVBAP::vbapRendererObject(numRendererOutputChannels_);

		// Allocate work pointer buffer for storing pointers to output sample buffers.
		// Size = number of output channels.
		outputBufferPointers_ = new IABSampleType*[numRendererOutputChannels_];

		// Also set up object subblock renderer
		subBlockRenderer_.SetUp(iObjectRendererParam.objectSubBlockRendererParam_);

		return kIABNoError;
	}

	// IABObjectRenderer::GetSubblockVBAPCacheSize() implementation
	uint32_t IABObjectRenderer::GetSubblockVBAPCacheSize()
	{
		return subBlockRenderer_.GetVBAPCacheSize();
	}

	// IABObjectRenderer::ClearSubblockVBAPCache() implementation
	void IABObjectRenderer::ClearSubblockVBAPCache()
	{
		subBlockRenderer_.ClearVBAPCache();
	}

	// IABObjectRenderer::SetFrameParameter() implementation
	iabError IABObjectRenderer::SetFrameParameter(FrameParam &iFrameParam)
	{
		// Point to the current IAB frame being rendered 
		iabFrameToRender_ = iFrameParam.iabFrameToRender_;

		// Pointer frame asset map.
		frameAudioDataIDToAssetPointerMap_ = iFrameParam.frameAudioDataIDToAssetPointerMap_;

		// Frame rate for current IAB frame
		frameRate_ = iFrameParam.frameRate_;

		// frameSampleCount_
		frameSampleCount_ = iFrameParam.frameSampleCount_;

		// Number of object panning subblocks
		numPanSubBlocks_ = iFrameParam.numPanSubBlocks_;

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
				subBlockSampleStartOffset_[i] = subBlockSampleStartOffset_[i - 1] + kSubblockSize_23_97FPS_48kHz[i - 1];
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
				subBlockSampleStartOffset_[i] = subBlockSampleStartOffset_[i - 1] + subBlockSampleCount;
			}
		}

		return kIABNoError;
	}

	// IABObjectRenderer::RenderIABObject() implementation
	iabError IABObjectRenderer::RenderIABObject(const IABObjectDefinition& iIABObject
		, IABSampleType **oOutputChannels
		, IABRenderedOutputChannelCountType iOutputChannelCount
		, IABRenderedOutputSampleCountType iOutputSampleBufferCount)
	{
		iabError iabReturnCode = kIABNoError;

		// Check input parameters
		if ((iOutputChannelCount == 0) ||
			(iOutputSampleBufferCount == 0) ||
			(iOutputSampleBufferCount != frameSampleCount_))
		{
			return kIABBadArgumentsError;
		}

		// Conditional Object logic implementation
		//

		// Step 1 check: Is this object (itself) activated?
		// If not, exit right away. Otherwise, continue
		if (!IsObjectActivatedForRendering(&iIABObject, targetUseCase_))
		{
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
					if (IsObjectActivatedForRendering(dynamic_cast<IABObjectDefinition*>(*iter), targetUseCase_))
					{
						// Render the activated sub-element object instead
						// (Recursive call)
						//
						iabReturnCode = RenderIABObject(*(dynamic_cast<IABObjectDefinition*>(*iter))
							, oOutputChannels
							, iOutputChannelCount
							, iOutputSampleBufferCount);

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
				else;	// Ignore other types of sub-elements.
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
			// For IAB, when audioData ID = 0, then the object has no audio for this frame and can be skipped
			return kIABNoError;
		}

		// Get start channel gains from inside object rendering

		// Start gains from previous frame
		IABGAINSPROC::EntityPastChannelGains inStartOutEndChannelGains;
		
		// Resize to number of output channels per target config
		inStartOutEndChannelGains.channelGains_.resize(numRendererOutputChannels_);

		// Retrieve channel gains for this object for possible smoothing processing.
		GetStartGainsInFrameGainsHistory(objectMetaID, inStartOutEndChannelGains);

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

		// Retrieve object asset
		IABSampleType *sampleBufferFloat = frameAudioDataIDToAssetPointerMap_->at(audioDataID);

		// Sub block input and output PCM buffer pointers
		IABSampleType *inputAssetSamples = NULL;

		// Render each panblock
		for (uint32_t i = 0; i < numSubBlocks; i++)
		{
			subBlockSampleCount = subBlockSampleCount_[i];

			// Update PCM input and output buffer pointers per sub block index
			inputAssetSamples = sampleBufferFloat + subBlockSampleStartOffset_[i];

			for (uint32_t j = 0; j < iOutputChannelCount; j++)
			{
				outputBufferPointers_[j] = oOutputChannels[j] + subBlockSampleStartOffset_[i];
			}

			// Render a sub block
			const IABObjectSubBlock* subblockToRender = dynamic_cast<const IABObjectSubBlock*>(objectPanSubBlocks[i]);

			subBlockRenderer_.RenderIABObjectSubBlock(*subblockToRender
				, inStartOutEndChannelGains
				, vbapObject_
				, inputAssetSamples
				, outputBufferPointers_
				, iOutputChannelCount
				, subBlockSampleCount);
		}

		// *****  MT critical section  *****
		// lock mutex for gains history
		gainsHistoryMutex_.lock();

		// Save post-rendering inStartOutEndChannelGains back to history, for next frame processing.
		(*frameEntityGainHistory_)[objectMetaID] = inStartOutEndChannelGains;

		// unlock
		gainsHistoryMutex_.unlock();
		// *****  End critical section  *****

		return kIABNoError;
	}

	// IABObjectRenderer::ResetVBAPObject() implementation
	iabError IABObjectRenderer::ResetVBAPObject()
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

	// IABObjectRenderer::GetStartGainsInFrameGainsHistory() implementation
	iabError IABObjectRenderer::GetStartGainsInFrameGainsHistory(uint32_t iObjectID, IABGAINSPROC::EntityPastChannelGains& oStartGains)
	{
		// *****  MT critical section  *****
		// lock mutex for gains history
		gainsHistoryMutex_.lock();

		// Retrieve current channel gains from internally save history, per iObjectID
		// If not found, add a new entry to history
		//
		if (frameEntityGainHistory_->find(iObjectID) == frameEntityGainHistory_->end())
		{
			IABGAINSPROC::EntityPastChannelGains& newEntryInHistory = (*frameEntityGainHistory_)[iObjectID];

			if (newEntryInHistory.channelGains_.size() != numRendererOutputChannels_)
			{
				newEntryInHistory.channelGains_.resize(numRendererOutputChannels_);
			}
		}

		// Copy whatever is found back to ioStartGains ioStartGains, as output
		// Dev note, if this is a new entry, the output remains the same as input. As a result, it effectively 
		// disables ensuing smoothing, as start gains == target gains.
		oStartGains = (*frameEntityGainHistory_)[iObjectID];

		// unlock
		gainsHistoryMutex_.unlock();
		// *****  End critical section  *****

		return kIABNoError;
	}

	/****************************************************************************
	* IABBedChannelRenderer
	*****************************************************************************/

	// Default constructor
	IABBedChannelRenderer::IABBedChannelRenderer(std::vector<IABMutex>& perChOutputMutex)
    :perChOutputMutex_(perChOutputMutex)
	{
		containerBedMetaID_ = 0;
		speakerCount_ = 0;
		numRendererOutputChannels_ = 0;

		vbapRenderer_ = NULL;
		channelGainsProcessor_ = NULL;
		vbapObject_ = NULL;
	}

	// Destructor
	IABBedChannelRenderer::~IABBedChannelRenderer()
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
	}

	// IABBedChannelRenderer::SetUp() implementation
	iabError IABBedChannelRenderer::SetUp(BedChannelRendererParam &iBedChannelRendererParam)
	{
		// Total list including virtual
		totalSpeakerList_ = iBedChannelRendererParam.totalSpeakerList_;

		// Physical speakers to output buffer indices map.
		physicalURIedBedSpeakerOutputIndexMap_ = iBedChannelRendererParam.physicalURIedBedSpeakerOutputIndexMap_;

		// Map containing URI'ed speakers to indices in totalSpeakerList_.
		totalURIedSpeakerToTotalSpeakerListMap_ = iBedChannelRendererParam.totalURIedSpeakerToTotalSpeakerListMap_;

		// This maps speaker index (of totalSpeakerList_) to output buffer index
		speakerIndexToOutputIndexMap_ = iBedChannelRendererParam.speakerIndexToOutputIndexMap_;

		// Number of output channels for the current target config. 
		// This number euqals to # of physical speakers. An output must be generated from renderer for physical speakers.
		numRendererOutputChannels_ = iBedChannelRendererParam.numRendererOutputChannels_;

		// Number of "speakers" for the current target config.
		// Despite the terminology inherited from render-config days, this number equals to "physical + virtual" speakers.
		// So speakerCount_ >= numRendererOutputChannels_.
		speakerCount_ = iBedChannelRendererParam.speakerCount_;

		// Create a vbap object, an object for modelling IAB rendering parameters into
		// an object that VBAPRenderer can render on.
		vbapObject_ = new IABVBAP::vbapRendererObject(numRendererOutputChannels_);

		// Create and set up VBAP renderer
		vbapRenderer_ = new IABVBAP::VBAPRenderer();
		vbapRenderer_->InitWithConfig(iBedChannelRendererParam.renderConfig_);

		// Create a channel gain processor (engine)
		// This is for applying channel gains to asset sample to generate channel output.
		// (For multi-threading, consider implementing these directly in bed channel renderer
		// instead of separate class...)
		channelGainsProcessor_ = new IABGAINSPROC::ChannelGainsProcessorMT(perChOutputMutex_);

		return kIABNoError;
	}

	// IABBedChannelRenderer::ClearVBAPCache() implementation
	void IABBedChannelRenderer::ClearVBAPCache()
	{
		if (vbapRenderer_)
		{
			// Clear VBAPRenderer extendedsource cache
			vbapRenderer_->ResetPreviouslyRendered();
		}
	}

	// IABBedChannelRenderer::SetContainerBedID() implementation
	void IABBedChannelRenderer::SetContainerBedID(uint32_t iBedMetaID)
	{
		containerBedMetaID_ = iBedMetaID;
	}

	// IABBedChannelRenderer::RenderIABChannel() implementation
	iabError IABBedChannelRenderer::RenderIABChannel(const IABChannelInterface& iIABChannel
		, IABSampleType *iAssetSamples
		, IABSampleType **oOutputChannels
		, IABRenderedOutputChannelCountType iOutputChannelCount
		, IABRenderedOutputSampleCountType iOutputSampleBufferCount)
	{
		IABGain channelGain;
		iIABChannel.GetChannelGain(channelGain);

		float gain = channelGain.getIABGain();

		if (gain == 0.0f)
		{
			// zero channel gain, skip, no further processing required.
			return kIABNoError;
		}

		IABChannelIDType channelID;
		IABSampleType *ptrInputSamples = iAssetSamples;

		// Use bed channel ID to find the corresponding config file speaker label
		IABBedChannelInfoMap::const_iterator iterBedChannelMap;
		iIABChannel.GetChannelID(channelID);
		iterBedChannelMap = IABConfigTables::bedChannelInfoMap.find(channelID);

		if (iterBedChannelMap == IABConfigTables::bedChannelInfoMap.end())
		{
			// No such channel ID in IABConfigTables::bedChannelInfoMap
			// Error: Channel ID not defined in ST2098-2.
			return kIABRendererBedChannelError;
		}

		// Map entry found, use the channel's speaker label to check if it is in the
		// config file speaker list

		// First, from physical URI'ed speakers, in precedence
		std::map<std::string, int32_t>::iterator iterChannelIndexMap = physicalURIedBedSpeakerOutputIndexMap_->find(iterBedChannelMap->second.speakerURI_);

		// Then as next-step backup, from all URI'ed speakers, including virtual speakers that have valid, non-empty URIs defined.
		// Effectively, this "find" will locate both physical AND virtual ones. The physical ones overlap with the first "find" above.
		// When the first, physical ones are not found, this will help to "find" virtual ones, if present
		std::map<std::string, int32_t>::iterator iterVirtualSpeakerIndexMap = totalURIedSpeakerToTotalSpeakerListMap_->find(iterBedChannelMap->second.speakerURI_);

		if (iterChannelIndexMap != physicalURIedBedSpeakerOutputIndexMap_->end())
		{
			// Yes, it's in the list, ensure index is within range
			if (iterChannelIndexMap->second >= iOutputChannelCount)
			{
				return kIABRendererBedChannelError;
			}

			// Add asset to the buffer
			IABSampleType *ptrChannelBuffer = oOutputChannels[iterChannelIndexMap->second];

			// *****  MT critical section  *****
			int chNum = iterChannelIndexMap->second;
			// lock mutex for output buffer
            perChOutputMutex_[chNum].lock();

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

			// unlock
            perChOutputMutex_[chNum].unlock();

			// *****  End critical section  *****

			return kIABNoError;
		}
		else if (iterVirtualSpeakerIndexMap != totalURIedSpeakerToTotalSpeakerListMap_->end())
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

				ptrInputSamples = iAssetSamples;								// Reset input sample pointer for each downmix output speaker

				// Need to map .ch_ (speaker index) to the actual output index. The latter in in the range of 
				// physical outputs for physical speaker (while the former is indexed with inclusion of virtual).
				if (speakerIndexToOutputIndexMap_->find(aggregatedDownmixMap[i].ch_) == speakerIndexToOutputIndexMap_->end())
				{
					return kIABRendererDownmixChannelError;
				}

				IABSampleType *ptrOutputChannelBuffer = oOutputChannels[speakerIndexToOutputIndexMap_->at(aggregatedDownmixMap[i].ch_)];	// pointer to downmix output channel


				int chNum = speakerIndexToOutputIndexMap_->at(aggregatedDownmixMap[i].ch_);
				// *****  MT critical section  *****

				// lock mutex for output buffer
                perChOutputMutex_[chNum].lock();

				for (uint32_t i = 0; i < iOutputSampleBufferCount; i++)
				{
					// Apply gain and add asset to the target channel buffer
					*ptrOutputChannelBuffer++ += (*ptrInputSamples++) * downmixChannelGain;
				}

				// unlock
                perChOutputMutex_[chNum].unlock();

				// *****  End critical section  *****
			}

			return kIABNoError;
		}
		else if (iterBedChannelMap->second.speakerURI_ != speakerURILFE)
		{
			// No, the target layout does not contain this bed channel and it is not LFE
			// Render the bed channel as a point source object with the speaker's VBAP coordinates

			IABVBAP::vbapError vbapReturnCode = IABVBAP::kVBAPNoError;
			IABGAINSPROC::gainsProcError gainsProceReturnCode = IABGAINSPROC::kGainsProcNoError;

			// Reset working object data structure to default state before updating
			vbapObject_->ResetState();

			// VBAP object requires an ID. Create one by combining channelID, containerBedMetaID_, and
			// then adding 0xff000000, for improved uniqueness
			uint32_t vbapIDforChannel = channelID + (containerBedMetaID_ << 8) + 0xff000000;

			// Set VBAP object ID
			vbapObject_->SetId(vbapIDforChannel);

			// Get bed channel VBAP coordinates from the map to set source position
			IABBedChannelInfoMap::const_iterator iter;
			iter = IABConfigTables::bedChannelInfoMap.find(channelID);
			if (iter == IABConfigTables::bedChannelInfoMap.end())
			{
				return kIABRendererBedChannelError;
			}

			// To set extended sources for vbapObject
			std::vector<IABVBAP::vbapRendererExtendedSource> & extendedSources = vbapObject_->extendedSources_;

			// Channel position based rendering. Position already on dome.
			// Extent parameters set to 0 (default).
			//
			IABVBAP::vbapRendererExtendedSource extendedSource(speakerCount_, numRendererOutputChannels_);
			extendedSource.SetPosition((*iter).second.speakerVBAPCoordinates_);
			// Set gain for extended source to (default) 1.0
			extendedSource.SetGain(1.0f);

			// Add to extendedSources
			extendedSources.push_back(extendedSource);

			// Set channel gain as vbap object gain
			vbapObject_->SetGain(gain);

			// VBAP-render object.
			// 
			vbapReturnCode = vbapRenderer_->RenderObject(vbapObject_);
			if (vbapReturnCode != IABVBAP::kVBAPNoError)
			{
				return kIABRendererVBAPRenderingError;
			}

			// Placeholder variable for API call.
			IABGAINSPROC::EntityPastChannelGains unusedGains;

			// Apply channel gains
			gainsProceReturnCode = channelGainsProcessor_->ApplyChannelGains(vbapObject_->id_
				, unusedGains
				, iAssetSamples
				, iOutputSampleBufferCount
				, oOutputChannels
				, iOutputChannelCount
				, false									// No init to output channel buffers
				, vbapObject_->channelGains_
				, false	);

			if (gainsProceReturnCode != IABGAINSPROC::kGainsProcNoError)
			{
				return kIABRendererApplyChannelGainsError;
			}

			return kIABNoError;
		}
		else
		{
			return kIABRendererNoLFEInConfigForBedLFEWarning;
		}
	}

	/****************************************************************************
	* IABBedRemapRenderer
	*****************************************************************************/

	// Default constructor
	IABBedRemapRenderer::IABBedRemapRenderer(std::vector<IABMutex>& perChOutputMutex)
    :perChOutputMutex_(perChOutputMutex)
	{
		speakerCount_ = 0;
		numRendererOutputChannels_ = 0;

		vbapRenderer_ = NULL;
		channelGainsProcessor_ = NULL;
		vbapObject_ = NULL;
		outputBufferPointers_ = NULL;
	}

	// Destructor
	IABBedRemapRenderer::~IABBedRemapRenderer()
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

		delete[] outputBufferPointers_;
	}

	// IABBedRemapRenderer::SetUp() implementation
	iabError IABBedRemapRenderer::SetUp(BedRemapRendererParam &iBedRemapRendererParam)
	{
		// Total list including virtual
		totalSpeakerList_ = iBedRemapRendererParam.totalSpeakerList_;

		// Physical speakers to output buffer indices map.
		physicalURIedBedSpeakerOutputIndexMap_ = iBedRemapRendererParam.physicalURIedBedSpeakerOutputIndexMap_;

		// Map containing URI'ed speakers to indices in totalSpeakerList_.
		totalURIedSpeakerToTotalSpeakerListMap_ = iBedRemapRendererParam.totalURIedSpeakerToTotalSpeakerListMap_;

		// This maps speaker index (of totalSpeakerList_) to output buffer index
		speakerIndexToOutputIndexMap_ = iBedRemapRendererParam.speakerIndexToOutputIndexMap_;

		// Number of output channels for the current target config. 
		// This number euqals to # of physical speakers. An output must be generated from renderer for physical speakers.
		numRendererOutputChannels_ = iBedRemapRendererParam.numRendererOutputChannels_;

		// Number of "speakers" for the current target config.
		// Despite the terminology inherited from render-config days, this number equals to "physical + virtual" speakers.
		// So speakerCount_ >= numRendererOutputChannels_.
		speakerCount_ = iBedRemapRendererParam.speakerCount_;

		// Create a vbap object, an object for modelling IAB rendering parameters into
		// an object that VBAPRenderer can render on.
		vbapObject_ = new IABVBAP::vbapRendererObject(numRendererOutputChannels_);

		// Allocate pointer buffer for storing pointers to output sample buffers.
		// Size = number of output channels.
		outputBufferPointers_ = new IABSampleType*[numRendererOutputChannels_];

		// Create and set up VBAP renderer
		vbapRenderer_ = new IABVBAP::VBAPRenderer();
		vbapRenderer_->InitWithConfig(iBedRemapRendererParam.renderConfig_);

		// Create a channel gain processor (engine)
		// This is for applying channel gains to asset sample to generate channel output.
		// (For multi-threading, consider implementing these directly in bed channel renderer
		// instead of separate class...)
		channelGainsProcessor_ = new IABGAINSPROC::ChannelGainsProcessorMT(perChOutputMutex_);

		return kIABNoError;
	}

	// IABBedRemapRenderer::ClearVBAPCache() implementation
	void IABBedRemapRenderer::ClearVBAPCache()
	{
		if (vbapRenderer_)
		{
			// Clear VBAPRenderer extendedsource cache
			vbapRenderer_->ResetPreviouslyRendered();
		}
	}

	// IABBedRemapRenderer::SetFrameParameter() implementation
	iabError IABBedRemapRenderer::SetFrameParameter(FrameParam &iFrameParam)
	{
		// Pointer frame asset map.
		frameAudioDataIDToAssetPointerMap_ = iFrameParam.frameAudioDataIDToAssetPointerMap_;

		// Frame rate for current IAB frame
		frameRate_ = iFrameParam.frameRate_;

		// frameSampleCount_
		frameSampleCount_ = iFrameParam.frameSampleCount_;

		// Number of object panning subblocks
		numPanSubBlocks_ = iFrameParam.numPanSubBlocks_;

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
				subBlockSampleStartOffset_[i] = subBlockSampleStartOffset_[i - 1] + kSubblockSize_23_97FPS_48kHz[i - 1];
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
				subBlockSampleStartOffset_[i] = subBlockSampleStartOffset_[i - 1] + subBlockSampleCount;
			}
		}

		return kIABNoError;
	}

	// IABBedRemapRenderer::RenderIABBedRemap() implementation
	iabError IABBedRemapRenderer::RenderIABBedRemap(const IABBedRemap& iIABBedRemap
		, const IABBedDefinition& iParentBed
		, IABSampleType **oOutputChannels
		, IABRenderedOutputChannelCountType iOutputChannelCount
		, IABRenderedOutputSampleCountType iOutputSampleBufferCount)
	{
		iabError iabReturnCode = kIABNoError;

		// Private method serving BedDefinition rendering only. Checking done at parent level.

		IABUseCaseType remapUseCase = kIABUseCase_NoUseCase;
		iIABBedRemap.GetRemapUseCase(remapUseCase);

		IABChannelCountType sourceChannelCount = 0;
		iIABBedRemap.GetSourceChannels(sourceChannelCount);

		IABChannelCountType destinationChannelCount = 0;
		iIABBedRemap.GetDestinationChannels(destinationChannelCount);

		// Retrieve source channels from parent bed for checking
		std::vector<IABChannel*> sourceChannels;
		iParentBed.GetBedChannels(sourceChannels);

		// Retrieve parent Bed meta ID. This will be used to compose a unique ID for a "destination channel object".
		uint32_t parentBedMetaID;
		iParentBed.GetMetadataID(parentBedMetaID);

		if ((sourceChannelCount == 0)
			|| (sourceChannels.size() != sourceChannelCount)
			|| (destinationChannelCount == 0)
			|| (iOutputSampleBufferCount == 0))
		{
			return kIABRendererBedRemapError;
		}

		// Array of pointers for individual source channel samples
		float **sourceBufferPointers = new float*[sourceChannelCount];
		IABAudioDataIDType audioDataID = 0;

		// Need a silence buffer as "filler" in case that any source channels have audio ID of 0.
		float *silencePCMBuffer = new float[iOutputSampleBufferCount];

		// Init to 0.0f
		std::fill(silencePCMBuffer, (silencePCMBuffer + iOutputSampleBufferCount), 0.0f);

		// Pre-fetch source channel gains/scale for later use during remap processing
		IABGain sourceChannelGain;
		float *sourceChannelScales = new float[sourceChannelCount];

		// Init to 0.0f
		std::fill(sourceChannelScales, (sourceChannelScales + sourceChannelCount), 0.0f);

		// Init and PCM decoding
		// 1) Init sourceBufferPointers to beginning of decoded PCM samples for each source channel
		// 2) Pre-fetch source channel gains and fill sourceChannelScales
		//
		for (uint32_t i = 0; i < sourceChannelCount; i++)
		{
			// Check sourceChannels[i] pointer
			if (sourceChannels[i] == NULL)
			{
				// Error condition, delete buffers
				delete[] sourceBufferPointers;
				delete[] silencePCMBuffer;
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
				sourceBufferPointers[i] = silencePCMBuffer;
				continue;
			}

			// Set up source channel asset pointer based on the audioDataID associated with source channel
			sourceBufferPointers[i] = frameAudioDataIDToAssetPointerMap_->at(audioDataID);
		}

		// *** Get Remap sub blocks
		std::vector<IABBedRemapSubBlock*> remapSubBlocks;
		iIABBedRemap.GetRemapSubBlocks(remapSubBlocks);

		uint8_t numRemapSubBlocks = 0;
		iIABBedRemap.GetNumRemapSubBlocks(numRemapSubBlocks);

		// Check sub block number vs size
		if ((numRemapSubBlocks == 0)
			|| (remapSubBlocks.size() != numRemapSubBlocks)
			|| (numRemapSubBlocks != numPanSubBlocks_))
		{
			// Error condition, delete buffers
			delete[] sourceBufferPointers;
			delete[] silencePCMBuffer;
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

		bool noLfeInConfig = false;

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
				delete[] sourceBufferPointers;
				delete[] silencePCMBuffer;
				delete[] sourceChannelScales;
				delete[] tempRemappedPCMBuffer;

				return kIABRendererBedRemapError;
			}

			// Apply remap coefficients
			IABChannelIDType destinationChannelID;
			IABBedChannelInfoMap::const_iterator iterDestChannelMap;
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
					delete[] sourceBufferPointers;
					delete[] silencePCMBuffer;
					delete[] sourceChannelScales;
					delete[] tempRemappedPCMBuffer;

					return kIABRendererBedRemapError;
				}

				// Is this destination channel ID in config file speaker list?
				// (use the channel's speaker label to check)
				// First, from physical URI'ed speakers, in precedence
				std::map<std::string, int32_t>::iterator iterChannelIndexMap =
					physicalURIedBedSpeakerOutputIndexMap_->find(iterDestChannelMap->second.speakerURI_);

				// Then as next-step backup, from all URI'ed speakers, including virtual speakers that have valid, non-empty URIs defined.
				// Effectively, this "find" will locate both physical AND virtual ones. The physical ones overlap with the first "find" above.
				// When the first, physical ones are not found, this will help to "find" virtual ones, if present
				std::map<std::string, int32_t>::iterator iterVirtualSpeakerIndexMap =
					totalURIedSpeakerToTotalSpeakerListMap_->find(iterDestChannelMap->second.speakerURI_);

				// Three possibilities for remap processing below.
				// 1) Destination channel ID in the target physical speaker list - directly remap
				// 2) Destination channel ID in the target virtual speaker list - directly remap to virtual and then downmix
				// 3) Destination channel ID NOT in any target speaker list - remap first
				//    & render-as-object to target
				//
				if (iterChannelIndexMap != physicalURIedBedSpeakerOutputIndexMap_->end())
				{
					// In config speaker list - ensure index is within range
					if (iterChannelIndexMap->second >= iOutputChannelCount)
					{
						// Error condition, delete buffers
						delete[] sourceBufferPointers;
						delete[] silencePCMBuffer;
						delete[] sourceChannelScales;
						delete[] tempRemappedPCMBuffer;

						return kIABRendererBedRemapError;
					}

					// destination channel buffer. Note to shift by n sub-blocks
					IABSampleType *destChannelBuffer =
						oOutputChannels[iterChannelIndexMap->second] + subBlockSampleStartOffset_[n];

					int chNum = iterChannelIndexMap->second;
					// Extra check on numSource in remapCoeffArray[i]
					numSource = remapCoeffArray[i]->getRemapSourceNumber();
					if (numSource != sourceChannelCount)
					{
						// Error condition, delete buffers
						delete[] sourceBufferPointers;
						delete[] silencePCMBuffer;
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
							delete[] sourceBufferPointers;
							delete[] silencePCMBuffer;
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

						// *****  MT critical section  *****

						// lock mutex for output buffer
                        perChOutputMutex_[chNum].lock();

						for (uint32_t k = 0; k < subBlockSampleCount; k++)
						{
							destChannelBuffer[k] += srcChannelBuffer[k] * combinedScale;
						}

						// unlock
                        perChOutputMutex_[chNum].unlock();

						// *****  End critical section  *****
					}
				}
				else if (iterVirtualSpeakerIndexMap != totalURIedSpeakerToTotalSpeakerListMap_->end())
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
						if (speakerIndexToOutputIndexMap_->find(downmixMap[m].ch_) == speakerIndexToOutputIndexMap_->end())
						{
							// Not found. An error condition, delete buffers
							delete[] sourceBufferPointers;
							delete[] silencePCMBuffer;
							delete[] sourceChannelScales;
							delete[] tempRemappedPCMBuffer;

							return kIABRendererDownmixChannelError;
						}

						// Pointer to downmix physical output buffer. Note to shift by n sub-blocks
						IABSampleType *destChannelBuffer = oOutputChannels[speakerIndexToOutputIndexMap_->at(downmixMap[m].ch_)]
							+ subBlockSampleStartOffset_[n];

						int chNum = speakerIndexToOutputIndexMap_->at(downmixMap[m].ch_);
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
								delete[] sourceBufferPointers;
								delete[] silencePCMBuffer;
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

							// *****  MT critical section  *****

							// lock mutex for output buffer
                            perChOutputMutex_[chNum].lock();

							// Accumulate remapped PCM to the mth downmix physical speaker output
							for (uint32_t k = 0; k < subBlockSampleCount; k++)
							{
								destChannelBuffer[k] += srcChannelBuffer[k] * combinedScale;
							}

							// unlock
                            perChOutputMutex_[chNum].unlock();

							// *****  End critical section  *****

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
							delete[] sourceBufferPointers;
							delete[] silencePCMBuffer;
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

					// Render the destination channel as a point source object with the speaker's VBAP coordinates

					IABVBAP::vbapError vbapReturnCode = IABVBAP::kVBAPNoError;
					IABGAINSPROC::gainsProcError gainsProceReturnCode = IABGAINSPROC::kGainsProcNoError;

					// Reset working object data structure to default state before updating
					vbapObject_->ResetState();

					// VBAP object requires an ID. Create one by combining destination channelID, parentBedMetaID, and
					// then adding 0xff000000, for improved uniqueness
					uint32_t vbapIDforChannel = destinationChannelID + (parentBedMetaID << 8) + 0xff000000;

					// Set VBAP object ID
					vbapObject_->SetId(vbapIDforChannel);

					// Get bed channel VBAP coordinates from the map to set source position
					IABBedChannelInfoMap::const_iterator iter;
					iter = IABConfigTables::bedChannelInfoMap.find(destinationChannelID);
					if (iter == IABConfigTables::bedChannelInfoMap.end())
					{
						return kIABRendererBedRemapError;
					}

					// To set extended sources for vbapObject
					std::vector<IABVBAP::vbapRendererExtendedSource> & extendedSources = vbapObject_->extendedSources_;

					// Channel position based rendering. Position already on dome.
					// Extent parameters set to 0 (default).
					//
					IABVBAP::vbapRendererExtendedSource extendedSource(speakerCount_, numRendererOutputChannels_);
					extendedSource.SetPosition((*iter).second.speakerVBAPCoordinates_);
					// Set gain for extended source to (default) 1.0
					extendedSource.SetGain(1.0f);

					// Add to extendedSources
					extendedSources.push_back(extendedSource);

					// Set vbap object gain
					// Note source gains are processed during remapping. Unity gain "1.0f" is used.
					vbapObject_->SetGain(1.0f);

					// VBAP-render object.
					// 
					vbapReturnCode = vbapRenderer_->RenderObject(vbapObject_);
					if (vbapReturnCode != IABVBAP::kVBAPNoError)
					{
						return kIABRendererVBAPRenderingError;
					}

					// Placeholder variable for API call.
					IABGAINSPROC::EntityPastChannelGains unusedGains;

					// Apply channel gains
					gainsProceReturnCode = channelGainsProcessor_->ApplyChannelGains(vbapObject_->id_
						, unusedGains
						, tempRemappedPCMBuffer
						, subBlockSampleCount
						, outputBufferPointers_
						, iOutputChannelCount
						, false									// No init to output channel buffers
						, vbapObject_->channelGains_
						, false									// lock to false to be consistent with other channel remap rendering paths.
                    );

					if (gainsProceReturnCode != IABGAINSPROC::kGainsProcNoError)
					{
						return kIABRendererApplyChannelGainsError;
					}
				}
				else
				{
					// Keep track of warnings
					noLfeInConfig = true;
				}    // End/closing "if (iterChannelIndexMap != physicalURIedBedSpeakerOutputIndexMap_.end())"
			}        // End/closing "for (uint32_t i = 0; i < destinationChannelCount; i++)"
		}            // End/closing "for (uint32_t n = 0; n < numRemapSubBlocks; n++)"

		// Processing reaching this point, processed output samples, sub-block samples, etc
		// all agree.

		// Clean up buffers
		delete[] sourceBufferPointers;
		delete[] silencePCMBuffer;
		delete[] sourceChannelScales;
		delete[] tempRemappedPCMBuffer;

		if (noLfeInConfig) {
			return kIABRendererNoLFEInConfigForRemapLFEWarning;
		}

		return kIABNoError;
	}

	/****************************************************************************
	* IABBedRenderer
	*****************************************************************************/

	// Default constructor
	IABBedRenderer::IABBedRenderer(std::vector<IABMutex>& perChOutputMutex)
    :channelRenderer_(perChOutputMutex),
    remapRenderer_(perChOutputMutex)
	{
	}

	// Destructor
	IABBedRenderer::~IABBedRenderer()
	{
	}

	// IABBedRenderer::SetUp() implementation
	iabError IABBedRenderer::SetUp(BedRendererParam &iBedRendererParam)
	{
		// UseCase for the current target config 
		targetUseCase_ = iBedRendererParam.targetUseCase_;

		// Number of output channels for the current target config 
		numRendererOutputChannels_ = iBedRendererParam.numRendererOutputChannels_;

		// Flag to enable/disable 96k IAB stream rendering to 48k output.
		render96kTo48k_ = iBedRendererParam.render96kTo48k_;

		// Set up bed channel renderer
		channelRenderer_.SetUp(iBedRendererParam.bedChannelRendererParam_);

		// Set up bed remap renderer
		remapRenderer_.SetUp(iBedRendererParam.bedRemapRendererParam_);

		return kIABNoError;
	}

	// IABBedRenderer::SetFrameParameter() implementation
	iabError IABBedRenderer::SetFrameParameter(FrameParam &iFrameParam)
	{
		// Pointer frame asset map.
		frameAudioDataIDToAssetPointerMap_ = iFrameParam.frameAudioDataIDToAssetPointerMap_;

		// Set up bed remap frame parameters
		remapRenderer_.SetFrameParameter(iFrameParam);

		// frameSampleCount_
		frameSampleCount_ = iFrameParam.frameSampleCount_;

		return kIABNoError;
	}

	// IABBedRenderer::RenderIABBed() implementation
	iabError IABBedRenderer::RenderIABBed(const IABBedDefinition& iIABBed
		, IABSampleType **oOutputChannels
		, IABRenderedOutputChannelCountType iOutputChannelCount
		, IABRenderedOutputSampleCountType iOutputSampleBufferCount)
	{
		iabError iabReturnCode = kIABNoError;

		// Warning if no LFE config, but input has LFE
		bool noLfeInConfig = false;
		// Warning if no LFE config for remap, input has LFE
		bool noLfeInConfigForRemap = false;

		// First, set meta ID to channelRenderer_, as it may need this ID to create unique ID for a "channel object".
		uint32_t bedMetaID;
		iIABBed.GetMetadataID(bedMetaID);
		channelRenderer_.SetContainerBedID(bedMetaID);

		// Conditional Bed logic implementation
		//

		// Step 1 check: Is this bed (itself) activated?
		// If not, exit right away. Otherwise, continue
		if (!IsBedActivatedForRendering(&iIABBed, targetUseCase_))
		{
			return iabReturnCode;
		}

		// Step 2 check: "this" is activated, but is there a sub-lement (if present)
		// that is also activated?
		// If yes, the activated child/sub-element is rendered, replacing the parent
		//
		IABElementCountType numSubElements = 0;
		iIABBed.GetSubElementCount(numSubElements);

		// Not using VBAP extendedsource cache. Clear before rendering the object
		// Actually ok to enable cache here. Unlikely for either to grow to considerable size
		// due to limited number of target channels.
		//
//		channelRenderer_.ClearVBAPCache();
//		remapRenderer_.ClearVBAPCache();

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
					if (IsBedActivatedForRendering(dynamic_cast<IABBedDefinition*>(*iter), targetUseCase_))
					{
						// Render the activated sub-element bed instead
						// (Recursive call)
						//
						iabReturnCode = RenderIABBed(*(dynamic_cast<IABBedDefinition*>(*iter))
							, oOutputChannels
							, iOutputChannelCount
							, iOutputSampleBufferCount);

						// Exit, return error code as-is.
						return iabReturnCode;
					}
				}
				else if (kIABElementID_BedRemap == elementID)
				{
					// Sub-element of BedRemap type. If this is activated, this should be rendered
					// by RenderIABBedRemap(), in combination with parent bed
					if (IsBedRemapActivatedForRendering(dynamic_cast<IABBedRemap*>(*iter), targetUseCase_))
					{
						iabReturnCode = remapRenderer_.RenderIABBedRemap(*(dynamic_cast<IABBedRemap*>(*iter))
							, iIABBed
							, oOutputChannels
							, iOutputChannelCount
							, iOutputSampleBufferCount);

						if (kIABRendererNoLFEInConfigForRemapLFEWarning == iabReturnCode)
						{
							// Record the warning, but continue
							noLfeInConfigForRemap = true;
							iabReturnCode = kIABNoError;
						}
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
		if ((channelCount == 0) || (bedChannels.size() != channelCount))
		{
			return kIABRendererBedDefinitionError;
		}

		std::vector<IABChannel*>::const_iterator iterBedChannel;

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

			// Retrieve bed channel asset
			IABSampleType *pAssetSamples = frameAudioDataIDToAssetPointerMap_->at(audioDataID);

			iabReturnCode = channelRenderer_.RenderIABChannel(**iterBedChannel
				, pAssetSamples
				, oOutputChannels
				, iOutputChannelCount
				, iOutputSampleBufferCount);

			if (kIABRendererNoLFEInConfigForBedLFEWarning == iabReturnCode)
			{
				// Record warning, but continue processing
				noLfeInConfig = true;
				iabReturnCode = kIABNoError;
			}
			if (kIABNoError != iabReturnCode)
			{
				return iabReturnCode;
			}
		}

		if (noLfeInConfig) {
			return kIABRendererNoLFEInConfigForBedLFEWarning;
		}
		if (noLfeInConfigForRemap) {
			return kIABRendererNoLFEInConfigForRemapLFEWarning;
		}

		return kIABNoError;
	}

	/****************************************************************************
	* IABAudioAssetDecoder
	*****************************************************************************/

	// Default constructor
	IABAudioAssetDecoder::IABAudioAssetDecoder()
	{
		render96kTo48k_ = false;
		frameSampleCount_ = 0;
	}

	// Destructor
	IABAudioAssetDecoder::~IABAudioAssetDecoder()
	{
	}

	// IABAudioAssetDecoder::SetUp() implementation
	iabError IABAudioAssetDecoder::SetUp(AssetDecoderParam &iAssetDecoderParam)
	{
		// Flag to render 96k to 48k 
		render96kTo48k_ = iAssetDecoderParam.render96kTo48k_;

		return kIABNoError;
	}

	// IABAudioAssetDecoder::SetFrameParameter() implementation
	iabError IABAudioAssetDecoder::SetFrameParameter(FrameParam &iFrameParam)
	{
		// frameSampleCount_
		frameSampleCount_ = iFrameParam.frameSampleCount_;

		return kIABNoError;
	}

	// IABAudioAssetDecoder::DecodeIABAsset() implementation (DLC)
	iabError IABAudioAssetDecoder::DecodeIABAsset(IABAudioDataDLC* iIABAudioDLC, IABSampleType* iOutputSampleBuffer)
	{
		iabError errorCode = kIABNoError;

		if (iOutputSampleBuffer == NULL)
		{
			return kIABRendererNotInitialisedError;
		}

		// Get sample rate for the DLC element
		IABSampleRateType dlcSampleRate;
		iIABAudioDLC->GetDLCSampleRate(dlcSampleRate);

		// Decode to int32_t PCM samples
		if (dlcSampleRate == kIABSampleRate_96000Hz && render96kTo48k_)
		{
			// Force decoding sampling rate to 48k, from a 96k DLC element
			errorCode = iIABAudioDLC->DecodeDLCToMonoPCMInternal(frameSampleCount_, kIABSampleRate_48000Hz);
		}
		else
		{
			// Use the inherent sampling rate of DLC element for decoding
			errorCode = iIABAudioDLC->DecodeDLCToMonoPCMInternal(frameSampleCount_, dlcSampleRate);
		}

		if (kIABNoError != errorCode)
		{
			return errorCode;
		}

		float scaleFactor = kInt32BitMaxValue;											// scale to 32-bit signed integer range
		int32_t *decodedSample = iIABAudioDLC->GetDecodedSampleBuffer();
		IABSampleType *convertedSample = iOutputSampleBuffer;

		for (uint32_t i = 0; i < frameSampleCount_; i++)
		{
			*convertedSample++ = static_cast<IABSampleType>(*decodedSample++ / scaleFactor);
		}

		return kIABNoError;
	}

	// IABAudioAssetDecoder::DecodeIABAsset() implementation (PCM)
	iabError IABAudioAssetDecoder::DecodeIABAsset(IABAudioDataPCM* iIABAudioPCM, IABSampleType* iOutputSampleBuffer)
	{
		iabError errorCode = kIABNoError;

		if (iOutputSampleBuffer == NULL)
		{
			return kIABRendererNotInitialisedError;
		}

		// Get frame sample count for checking
		uint32_t sampleCount = iIABAudioPCM->GetPCMSampleCount();
		if (sampleCount != frameSampleCount_)
		{
			return kIABRendererSampleCountMismatchError;
		}

		// Get audio samples
		errorCode = iIABAudioPCM->UnpackPCMToMonoSamplesInternal(frameSampleCount_);

		if (kIABNoError != errorCode)
		{
			return errorCode;
		}

		float scaleFactor = kInt32BitMaxValue;											// scale to 32-bit signed integer range
		int32_t *decodedSample = iIABAudioPCM->GetUnpackedSampleBuffer();
		IABSampleType *convertedSample = iOutputSampleBuffer;

		for (uint32_t i = 0; i < frameSampleCount_; i++)
		{
			*convertedSample++ = static_cast<IABSampleType>(*decodedSample++ / scaleFactor);
		}

		return kIABNoError;
	}


} // namespace ImmersiveAudioBitstream
} // namespace SMPTE


 // **************************************************************************
 // ChannelGainsProcessorMT class implementation
 //

namespace IABGAINSPROC
{
	// Constructor
	ChannelGainsProcessorMT::ChannelGainsProcessorMT(std::vector<SMPTE::ImmersiveAudioBitstream::IABMutex>& perChOutputMutex)
    :perChOutputMutex_(perChOutputMutex)
    {
		// Create VectDSP_ acceleration engine object/instance

#ifdef USE_MAC_ACCELERATE
		vectDSP_ = new CoreUtils::VectDSPMacAccelerate(MAX_RAMP_SAMPLES);
#else
		vectDSP_ = new CoreUtils::VectDSP();
#endif

		// Allocation to maximum size in sample (ie. MAX_RAMP_SAMPLES), as designed by algorithm
		// to cover all scenarios of realRampPeriod
		//
		smoothedGains_ = new float[MAX_RAMP_SAMPLES];

		// Allocate buffer for gainAppliedSamples_ for storing gain processed sample.
		// To maximum frame size, SMPTE::ImmersiveAudioBitstream::kIABMaxFrameSampleCount,
		// to accommodate all cases
		gainAppliedSamples_ = new float[SMPTE::ImmersiveAudioBitstream::kIABMaxFrameSampleCount];
	}

	// Destructor
	ChannelGainsProcessorMT::~ChannelGainsProcessorMT()
	{
		delete vectDSP_;
		delete[] smoothedGains_;
		delete[] gainAppliedSamples_;
	}

	// ChannelGainsProcessorMT::ApplyChannelGainst() implementation
	gainsProcError ChannelGainsProcessorMT::ApplyChannelGains(
		int32_t iObjectID
		, IABGAINSPROC::EntityPastChannelGains& ioStartEndGains
		, const float *iInputSamples
		, uint32_t iSampleCount
		, float **oOutputSamples
		, uint32_t iChannelCount
		, bool iInitializeOutputBuffers
		, const std::vector<float>& iTargetChannelGains
		, bool iEnableSmoothing
    )
	{
		// Check for null pointers, and size of iTargetChannelGains must agree with iChannelCount
		if (!iInputSamples
			|| !oOutputSamples
			|| (iSampleCount == 0)
			|| (iChannelCount == 0)
			|| (iTargetChannelGains.size() != iChannelCount))
		{
			return kGainsProcBadArgumentsError;
		}

		// If smoothing is enabled do extra checking
		if (iEnableSmoothing)
		{
			// size of iCurrentChannelGains must also agree with iChannelCount
			if (ioStartEndGains.channelGains_.size() != iChannelCount)
			{
				return kGainsProcBadArgumentsError;
			}

			// Compare iCurrentChannelGains with iTargetChannelGains
			// to decide if smoothing is really nedded
			//
			// We first assume that smoothing may not be needed...
			iEnableSmoothing = false;

			// PACT-1940: If we don't have channel gains for this
			// entity then don't perform smoothing
			if (ioStartEndGains.gainsValid_)
			{
				// loop through channel gains - compare start with target values...
				for (uint32_t i = 0; i < iChannelCount; i++)
				{
					// If difference found, revert back to smoothing
					if (iTargetChannelGains[i] != ioStartEndGains.channelGains_[i])
					{
						iEnableSmoothing = true;
						break;
					}
				}
			}
		}
		else
		{
			// ioStartEndGains is not expected to be accessed below when iEnableSmoothing == false, but
			// we perform a defensive resizing, just in case it's a placeholder/dummy variable on stack of 
			// calling function.
			ioStartEndGains.channelGains_.resize(iChannelCount);

			// Continue execution onward...
		}

		// Check output buffer pointer for each of output channels.
		// Initializing output buffer if requested
		for (uint32_t i = 0; i < iChannelCount; i++)
		{
			if (!oOutputSamples[i])
			{
				return kGainsProcBadArgumentsError;
			}

			// Reset output buffer samples to "0" only if (iInitializeOutputBuffers == true)
			if (iInitializeOutputBuffers)
			{
				memset(oOutputSamples[i], 0, sizeof(float) * iSampleCount);
			}
		}

		// Apply channel gains
		//
		if (iEnableSmoothing)												// Smoothing enabled
		{
			// Calculate gain changing ramp period, slope, etc.
			// rampPeriod contains an initilialized value per iSampleCount
			//
			uint32_t initRampPeriod = (MAX_RAMP_SAMPLES < iSampleCount) ? MAX_RAMP_SAMPLES : iSampleCount;

			// realRampPeriod is the final ramp period value that is created when capping slope. ie gain change rate.
			// By design, slope (rate of gain change per sample period must be capped
			// to reduce transients - the goal of any smoothing algorithm
			// MAX_SLOPE is an empirical design thredshold for 48kHz audio
			//
			uint32_t realRampPeriod = 0;

			float currentGain = 0.0f;
			float targetGain = 0.0f;
			float gainDiff = 0.0f;
			float slope = 1.0f;

			// Calculate ramp rate for each of output channels, based on corresponding gain changes
			for (uint32_t i = 0; i < iChannelCount; i++)
			{
				currentGain = ioStartEndGains.channelGains_[i];				// current gain value
				targetGain = iTargetChannelGains[i];						// target gain to be reached in rampPeriod samples
				gainDiff = targetGain - currentGain;						// gain difference covered in smoothing ramp
				slope = 0.0;												// init

				// Make sure there is no divide by 0
				if (initRampPeriod != 0)
				{
					slope = gainDiff / initRampPeriod;						// slope of ramp, ie. gain changes for every next sample. (This algorithm implements a linear change for linear scale factor.)
				}
				else
				{
					return kGainsProcDivisionByZeroError;
				}

				// Check slope against max limit. If it exceeds, cap it at max.
				// Revise ramp period based on set slope value and save it in realRampPeriod
				//
				if (slope > MAX_SLOPE)
				{
					// gain ramp up
					slope = MAX_SLOPE;
					realRampPeriod = RAMP_SAMPLE_MAX_SLOPE;
				}
				else if (slope < -MAX_SLOPE)
				{
					// gain ramp down
					slope = -MAX_SLOPE;
					realRampPeriod = RAMP_SAMPLE_MAX_SLOPE;
				}
				else if (slope == 0.0)
				{
					// No ramp needed
					realRampPeriod = 0;										// Don't use division if slope is 0. Value "0" means no ramp is needed.
				}
				else
				{
					// initRampPeriod already inside limit - simply use it.
					realRampPeriod = initRampPeriod;
				}

				// At this point, the resulting maximum value of realRampPeriod is MAX_RAMP_SAMPLES, currenlty 4800 samples.
				// the minimum value is either
				// 1) 1/MAX_SLOPE (which currently corresponds to 480 samples), or
				// 2) = iSampleCount, when (iSampleCount < 1/MAX_SLOPE), or
				// 3) = 0, when (slope == 0.0)
				// 

				// create smoothing ramp from slope
				//
				currentGain += slope;														// incrementing first gain one step beyond previously stored gain.
				vectDSP_->ramp(currentGain, targetGain, smoothedGains_, realRampPeriod);	// realRampPeriod is between 0 and 4800 samples.

				// adding constant portion after ramp if iSampleCount is longer than realRampPeriod (rarely happens)
				if (realRampPeriod < iSampleCount)
				{
					vectDSP_->fill(targetGain, smoothedGains_ + realRampPeriod, iSampleCount - realRampPeriod);
				}

				currentGain = smoothedGains_[iSampleCount - 1];

				// Multiply input with smoothedGains ramp, store result in gainAppliedSamples
				vectDSP_->mult(iInputSamples, smoothedGains_, gainAppliedSamples_, iSampleCount);

				// Find address of output samples for channel #[i]
				float *channelOutput = oOutputSamples[i];

				// *****  MT critical section  *****

				// lock mutex for output buffer
                perChOutputMutex_[i].lock();

				// Add (accummulate) gainAppliedSamples to output samples for channel.
				vectDSP_->add(channelOutput, gainAppliedSamples_, channelOutput, iSampleCount);

				// unlock
                perChOutputMutex_[i].unlock();

				// *****  End critical section  *****

				// Storing gain value that has been reached on channel by end of iSampleCount
				ioStartEndGains.channelGains_[i] = currentGain;
				ioStartEndGains.touched_ = true;
				ioStartEndGains.gainsValid_ = true;
			}
		}
		else
		{
			// Smoothing disabled. Apply gains in iTargetChannelGains uniformly

			// Apply gains for each output channels
			for (uint32_t i = 0; i < iChannelCount; i++)
			{
				// Fill smoothedGains array with identical gain value (no smoothing applied here.)
				vectDSP_->fill(iTargetChannelGains[i], smoothedGains_, iSampleCount);

				// Multiply input with smoothedGains, store result in gainAppliedSamples
				vectDSP_->mult(iInputSamples, smoothedGains_, gainAppliedSamples_, iSampleCount);

				// Find address of output samples for channel #[i]
				float *channelOutput = oOutputSamples[i];

				// *****  MT critical section  *****

				// lock mutex for output buffer
                perChOutputMutex_[i].lock();

				// Add (accummulate) gainAppliedSamples to output samples for channel.
				vectDSP_->add(channelOutput, gainAppliedSamples_, channelOutput, iSampleCount);

				// unlock
                perChOutputMutex_[i].unlock();

				// *****  End critical section  *****

				// Storing gain value that has been reached on channel by end of iSampleCount
				ioStartEndGains.channelGains_[i] = iTargetChannelGains[i];
				ioStartEndGains.touched_ = true;
				ioStartEndGains.gainsValid_ = true;
			}
		}

		return kGainsProcNoError;
	}
}  // namespace IABGAINSPROC

#endif // #if __linux__ || __APPLE__


