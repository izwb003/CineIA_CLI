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
 * IABRendererMT.h
 *
 * @file
 */


#ifndef __IABRENDERERMT_H__
#define	__IABRENDERERMT_H__

#if __linux__ || __APPLE__

#include <queue>
#include <pthread.h>

#include "common/IABElements.h"
#include "IABRendererAPI.h"
#include "renderer/VBAPRenderer/VBAPRenderer.h"
#include "renderer/IABInterior/IABInterior.h"
#include "renderer/ChannelGainsProcessor/ChannelGainsProcessor.h"
#include "IABConfigTables.h"
#include "renderer/IABObjectZones/IABObjectZones.h"
#include "renderer/IABDecorrelation/IABDecorrelation.h"


namespace SMPTE
{
    namespace ImmersiveAudioBitstream
    {
        
        /**
         *  Representations of the Mutex and Condition variable classes
         * for use ny multi threaded IAB renderer entities.
         *
         */
        class IABMutex
        {
            
        public:
            
            // Wrapper for pthread_cond_t
            class IABCondition
            {
            public:
                IABCondition()
                {
                    int retval = pthread_cond_init(&condVar_, NULL);
                    assert(retval == 0);
                }
                ~IABCondition()
                {
                    int retval = pthread_cond_destroy(&condVar_);
                    assert(retval == 0);
                }
                void signal()
                {
                    int retval = pthread_cond_signal(&condVar_);
                    assert(retval == 0);
                }
                void broadcast()
                {
                    int retval = pthread_cond_broadcast(&condVar_);
                    assert(retval == 0);
                }
                void wait(IABMutex& mutex)
                {
                    int retval = pthread_cond_wait(&condVar_, &mutex.mutex_);
                    assert(retval == 0);
                }
            private:
                pthread_cond_t condVar_;
            };
            
            IABMutex()
            {
                int retval = pthread_mutex_init(&mutex_, NULL);
                assert(retval == 0);
            }
            ~IABMutex()
            {
                int retval = pthread_mutex_destroy(&mutex_);
                assert(retval == 0);
            }
            void lock()
            {
                int retval = pthread_mutex_lock(&mutex_);
                assert(retval == 0);
            }
            void unlock()
            {
                int retval = pthread_mutex_unlock(&mutex_);
                assert(retval == 0);
            }
        private:
            pthread_mutex_t mutex_;
        };
    }
}

 // **************************************************************************
 // MT version of gain processor, without internal gain history save/restore
 //
 // The MT gain processor is a pure gains applicator.
 // Caller is responsible for implement gains history save/restore functionality.
 //
 // Currently, IABRendererMT::RenderIABFrame() and its members are the intended clients
 // of this class.
 //


namespace IABGAINSPROC
{
	/**
	*
	* MT Channel gains processor class, for applying channel gains.
	*
	* ChannelGainsProcessorMT class performs the same functions as ChannelGainsProcessor. The MT instance
	* performs channel gain applications without saving/restoring ID-mapped channel gains for cross-
	* frame smoothing processing.
	*
	* Caller is responsible for facilitating saving/restoring ID-mapped channel gains, outside of
	* this class.
	*
	* This change is to sever computation intensive channel gains application to PCM samples, from 
	* the need to access frame-wise shared data of gains history, hence minimizing potential
	* critical section lock/unlock.
	*
	*/
	class ChannelGainsProcessorMT
	{
	public:

		/// Constructor
		ChannelGainsProcessorMT(std::vector<SMPTE::ImmersiveAudioBitstream::IABMutex>& perChOutputMutex);

		/// Destructor
		~ChannelGainsProcessorMT();

		/**
		* This function is doing nothing. It's intended to overwrite 
		* ChannelGainsProcessor::UpdateGainsHistory() behavior.
		*
		*/
		void UpdateGainsHistory() {}

		/**
		* This function is doing nothing. It's intended to overwrite
		* ChannelGainsProcessor::ResetGainsHistory() behavior.
		*
		*/
		void ResetGainsHistory(void) {}

		/**
		* ApplyChannelGains() is a gain processing function. It does not own any PCM sample memory
		* internally. Instead, it simply applies gains in (iTargetChannelGains) to (*iInputSamples), 
		* and store output samples to (**oOutputsamples).
		*
		* Caller must ensure all buffers are properlly allocated and set up before calling the API
		* for required gain processing. Caller has the ownership to all buffers prior to, and after
		* calling ApplyChannelGains().
		*
		* - (iObjectID) is the ID of the object for which ApplyChannelGains() is used. For smoothing
		*   processing, client must supply previous saved channel gains in ioStartEndGains as the start 
		*   gains to ramp up (or down) to what is in (iTargetChannelGains).
		*
		* - (ioStartEndGains) carries the start channel gains for iObjectID. ApplyChannelGains() uses it
		*   as initial gain values for smoothing processing (ramp up or ramp down) towards what is carried
		*   in (iTargetChannelGains).
		*
		* - (iInputSamples) points to the start of input/source asset PCM samples, for 1 object or channel.
		*   The number of samples is (iSampleCount).
		*
		* - (iSampleCount) number of samples in each of the input or output buffers.
		*
		* - (oOutputsamples) contains an array of (iOutputChannelCount) pointers. Each points to the
		*   start of channel output buffer. There are (iOutputChannelCount) output channels. For each
		*   of output channels, (iSampleCount) output samples are generated.
		*
		* - (iChannelCount) number of output channels.
		*
		* - (iInitializeOutputBuffers), when true, buffers pointed from (oOutputsamples) are initilized to
		*   0.0f before gain processing. If false, no initialization is carried out. Gain-processed output
		*   samples are accummulated/aggregated/added into existent values in (oOutputsamples) buffers.
		*   Basically, this flag controls over-write or accumulate mode to output buffers.
		*
		* - (iTargetChannelGains) contains a set of (iOutputChannelCount) channel gains from VBAP renderer
		*   proceesing the object extent parameters of the current frame or subblocks. If smoothing is diabled,
		*   the target gains are applied directly and uniformly to (*iInputSamples) to generate 
		*   (iOutputChannelCount) channels of PCM output, each containing (iSampleCount) output samples.
		*   If, however, smoothing is enabled, a smoothing ramp-up/down is applied to generate gains from
		*   ioStartEndGains to iTargetChannelGains, before generated gains are applied.
		*
		* - (iEnableSmoothing), when true, ApplyChannelGains() applys gains using a smoothing algorithm,
		*   from current/start gains in ioStartEndGains input, to target/end gains in (iTargetChannelGains).
		*   Gains at the end of (iSampleCount) is saved back in ioStartEndGains as output. Client can save
		*   this output to frame gains history for iObjectID, for smoothing processing in next time segment.
		*   When (iEnableSmoothing is false, smoothing is disabled. Channel gains in (iTargetChannelGains)
		*   are applied uniformly. A copy of (iTargetChannelGains) is saved in ioStartEndGains as output. 
		*   Client can save this output to frame gains history for iObjectID, for smoothing processing in 
		*   next time segment.
		*
		* @param[in] iObjectID - object ID for which channel gains processing is carried out
		* @param[in] iInputSamples - pointer to input PCM samples
		* @param[in] iSampleCount - number of PCM samples, either input or each of output channel buffers
		* @param[out] oOutputSamples - pointer to an array of pointers, each corresponding to a channel output buffer
		* @param[in] iOutputChannelCount - number of output channels
		* @param[in] iInitializeOutputBuffers - when true, init to "0" on all channel output buffers
		* @param[in] iTargetChannelGains - channels gains to be applied, or target channel gains when smoothing is enabled
		* @param[in] iEnableSmoothing - when true, smoothing is enabled. When flase, disabled and iTargetChannelGains is applied uniformly.
		* @return \link gainsProcError \endlink if no errors. Otherwise error condition.
		*/
		gainsProcError ApplyChannelGains(
			int32_t iObjectID
			, IABGAINSPROC::EntityPastChannelGains& ioStartEndGains
			, const float *iInputSamples
			, uint32_t iSampleCount
			, float **oOutputSamples
			, uint32_t iChannelCount
			, bool iInitializeOutputBuffers
			, const std::vector<float>& iTargetChannelGains
			, bool iEnableSmoothing);

	private:

		// *** Private member functions
		// ***

		// VectDSP acceleration engine
		CoreUtils::VectDSPInterface *vectDSP_;

		// smoothedGains points to start of working gain buffer. The buffer stores sample-by-sample
		// gains after smoothing processing.
		float *smoothedGains_;

		// gainAppliedSamples points to start of working buffer for storing gain processed sample.
		float *gainAppliedSamples_;

		// vector of mutexes for synchronisation of channel output buffer access
		std::vector<SMPTE::ImmersiveAudioBitstream::IABMutex>& perChOutputMutex_;
	};

} // namespace IABGAINSPROC


// **************************************************************************
// MT IABRenderer classes
//

namespace SMPTE
{
namespace ImmersiveAudioBitstream
{
	
	/**
	* Sub-block sample count for 23.97fps frame rate and 48000Hz sampling rate.
	*
	* For fractional frame rate, number of samples is not equal for each of the sub-blocks.
	* This is unlike other integral frame rates.
	*/
	static const uint32_t kSubblockSize_23_97FPS_48kHz[8] = { 251, 250, 250, 250, 251, 250, 250, 250 };

    /**
     * Maximumm number of samples in a subblock. Currently set to maximum subblock size of
     * 96kHz, 23.97fps which has the maximum count out of all sample rate, frame rate combinations.
     */
	static const uint32_t kIABMaxSubblockSampleCount = 501;

	/**
	* Check if an ObjectDefinition element is activated (available) for rendering.
	* Note, "activated" does not necessarily mean "to be rendered". Other conditions
	* must be met. (See page 28 of published ST2098-2 specification.)
	*
	* @param[in] iIABBedRemap Pointer to an IABObjectDefinition element.
	* @returns bool true if activated, false if not.
	*
	*/
	bool IsObjectActivatedForRendering(const IABObjectDefinition* iIABObject, IABUseCaseType iTargetUseCase);

	/**
	* Check if a BedDefinition element is activated (available) for rendering.
	* Note, "activated" does not necessarily mean "to be rendered". Other conditions
	* must be met. (See page 25 of published ST2098-2 specification.)
	*
	* @param[in] iIABBedRemap Pointer to an IABBedDefinition element.
	* @returns bool true if activated, false if not.
	*
	*/
	bool IsBedActivatedForRendering(const IABBedDefinition* iIABBed, IABUseCaseType iTargetUseCase);

	/**
	* Check if a BedRemap element is activated (available) for rendering.
	* Generally in the case of BedRemap, "activated" does mean "to be used". This is unlike
	* BedDefinition element which may contain further sub-element.
	*
	* @param[in] iIABBedRemap Pointer to an IABBedRemap element.
	* @returns bool true if activated, false if not.
	*
	*/
	bool IsBedRemapActivatedForRendering(const IABBedRemap* iIABBedRemap, IABUseCaseType iTargetUseCase);

	/**
	* Checks to see if the sample rate and frame rate combination is supported for rendering.
	* 
	* !Important: This is where IABRendererMT support levels are defined for combinations of frame rates and
	* sample rates.
	*
	* @param[in] iFrameRate frame rate of the IAB frame to check..
	* @param[in] iSampleRate sample rate of the IAB frame to check.
	* @returns bool true if supported by IABRendererMT, false means "not supported".
	*
	*/
	bool IsSupported(IABFrameRateType iFrameRate, IABSampleRateType iSampleRate);

	// Typedef map linking channel index of each config file VBAP speaker to its position coordinate in the IAB unit cube
	typedef std::map<int32_t, CartesianPosInUnitCube> VBAPSpeakerToIABPositionMap;


	/**
	* Set of parameters from render-config, that are required in setting up IAB Object SubBlock Renderer
	*
	*/
	struct ObjectSubBlockRendererParam {

		// The VBAPRender member in object subblock renderer requires full config for initialization
		RenderUtils::IRendererConfiguration *renderConfig_;

		// Number of speakers in the renderer configuration
		uint32_t speakerCount_;

		// Flag to enable/disable cross-frame/subblock sample smoothing processing.
		bool     enableSmoothing_;

		// Number of audio output channels according to target config
		IABRenderedOutputChannelCountType   numRendererOutputChannels_;
	};

	/**
	* Set of parameters from render-config, that are required in setting up IAB Object Renderer
	*
	*/
	struct ObjectRendererParam {

		// Corresponding IAB use case matching soundfield line in the renderer configuration
		IABUseCaseType  targetUseCase_;

		// Number of audio output channels according to target config
		IABRenderedOutputChannelCountType   numRendererOutputChannels_;

		// Flag to enable/disable 96k IAB stream rendering to 48k output.
		bool     render96kTo48k_;

		// Cross-frame gains history.
		std::map<uint32_t, IABGAINSPROC::EntityPastChannelGains>* frameEntityGainHistory_;

		// Set of setup params for object subblock renderer.
		ObjectSubBlockRendererParam objectSubBlockRendererParam_;
	};

	/**
	* Set of parameters from IAB frame to be rendered, that are required in setting up IAB Object Renderer
	*
	*/
	struct FrameParam {
		// Frame size in sample for current frame
		IABRenderedOutputSampleCountType    frameSampleCount_;

		// Frame rate for current IAB frame
		IABFrameRateType frameRate_;

		// Number of object panning subblocks for current frame rate in input IAB Frame, per IAB specification
		uint8_t  numPanSubBlocks_;

		// Internal work pointer for referencing IAB frame to be rendered. Object set and owned by caller to RenderIABFrame(). 
		const IABFrame*     iabFrameToRender_;

		// Pointer to map for frame-wise audio data IDs to asset pointers. 
		// Passed from frame renderer to object or bed renderers.
		std::map<IABAudioDataIDType, IABSampleType*>*    frameAudioDataIDToAssetPointerMap_;
	};

	/**
	*
	* IAB Object Subblock Renderer class. For rendering an IAB object sub-block.
	*
	*/
	class IABObjectSubBlockRenderer
	{
	public:

		// Default Constructor
        IABObjectSubBlockRenderer(std::vector<IABMutex>& perChOutputMutex);

		// Destructor
		~IABObjectSubBlockRenderer();

		// Set up IABObjectSubBlockRenderer based on "iObjectSubBlockRendererParam".
		// Client must call SetUp() following creation of an IABObjectSubBlockRenderer object.
		// 
		iabError SetUp(ObjectSubBlockRendererParam &iObjectSubBlockRendererParam);

		// Get size of cached rendered extent source stored in VBAPRenderer member object.
		// 
		uint32_t GetVBAPCacheSize();

		// Clear cached rendered extent source stored in VBAPRenderer member object.
		// 
		void ClearVBAPCache();

		// Renders an IAB Object Sub Block(iIABObjectSubBlock) into output channels (ioOutputChannels).
		//
		// Note: Caller retains ownership to (*iIABObjectSubBlock) object after being rendered.
		//
		// Note: Rendered output samples are accumulated (NOT overwritten) to sample buffers 
		// pointed to by "oOutputChannels".
		// 
		iabError RenderIABObjectSubBlock(const IABObjectSubBlockInterface& iIABObjectSubBlock
			, IABGAINSPROC::EntityPastChannelGains& ioChannelGains
			, IABVBAP::vbapRendererObject *iVbapObject
			, IABSampleType *iAssetSamples
			, IABSampleType **oOutputChannels
			, IABRenderedOutputChannelCountType iOutputChannelCount
			, IABRenderedOutputSampleCountType iOutputSampleBufferCount);

	private:

		// *** Private member functions
		// ***

		// Create and initialize map table vbapSpeakerToIABPositionMap_
		iabError InitVBAPSpeakerToIABPositionMap(RenderUtils::IRendererConfiguration &iConfig);

		// Finds index of the config file speaker that satisfies the snap criteria
		// 1. Absolute diff between corresponding Speaker position X, Y, Z and object position X, Y, Z all <= snap tolerance AND
		// 2. Speaker closest to the object will be the snap target and its channel index within the config file is returned.
		// A return value of -1 indicates that no speaker satisfies the criteria, i.e. object snap is not activated.
		int32_t FindSnapSpeakerIndex(CartesianPosInUnitCube iObjectPosition, float iSnapTolerance);

		// *** Member variables
		// ***

		// Number of speakers in the renderer configuration
		// ( = numRendererOutputChannels_ + virual speakers)
		uint32_t speakerCount_;

		// Number of audio channels output by the renderer
		IABRenderedOutputChannelCountType   numRendererOutputChannels_;

		// Flag to enable/disable cross-frame/subblock sample smoothing processing.
		//  - Enable/disable as set by render config file "c smooth 1|0" flag, with default to "enabled".
		//  - No exposed getter/setter API
		bool     enableSmoothing_;

		// This map is used for checking if object position is within the snap tolerance.
		VBAPSpeakerToIABPositionMap vbapSpeakerToIABPositionMap_;

		// Pointer to instance of vbap renderer. Object created by, and owned by IABObjectRenderer instance.
		// IABObjectRenderer Destructor must delete object upon instance destruction.
		IABVBAP::VBAPRenderer     *vbapRenderer_;

		// Pointer to instance of ChannelGainsProcessorMT. Object created by, and owned by IABObjectRenderer instance.
		// IABObjectRenderer Destructor must delete object upon instance destruction.
		IABGAINSPROC::ChannelGainsProcessorMT     *channelGainsProcessor_;

		// Interior extended source to vbap extended source conversion class instance
		IABInterior iabInterior_;

		// Object zone9 control
		IABObjectZone9 *iabObjectZone9_;
        
        // Reference to the vector of mutexes required for multi threaded channel output
        std::vector<IABMutex>& perChOutputMutex_;
	};

	/**
	*
	* IAB Object Renderer class. For rendering an IAB object.
	*
	*/
	class IABObjectRenderer
	{
	public:

		// Default Constructor
        IABObjectRenderer(IABMutex& gainsHistoryMutex, std::vector<IABMutex>& perChOutputMutex);

		// Destructor
		~IABObjectRenderer();

		// Set up IABObjectRenderer based on "iObjectRendererParam".
		// Client must call SetUp() following creation of an IABObjectRenderer object.
		// 
		iabError SetUp(ObjectRendererParam &iObjectRendererParam);

		// Get size of cached rendered extent source in subBlockRenderer_ VBAP rendering engine.
		// 
		uint32_t GetSubblockVBAPCacheSize();

		// Clear cached rendered extent source in subBlockRenderer_ VBAP rendering engine.
		// 
		void ClearSubblockVBAPCache();

		// Set frame parameters to object renderer.
		// Called from frame rendering - once only if unchanged.
		// 
		iabError SetFrameParameter(FrameParam &iFrameParam);

		// Renders an IAB Object (iIABObject) into output channels (ioOutputChannels).
		// Note: Caller retains ownership to (*iIABObject) object after being rendered.
		//
		// Note: Rendered output samples are accumulated (NOT overwritten) to sample buffers 
		// pointed to by "oOutputChannels".
		// 
		iabError RenderIABObject(const IABObjectDefinition& iIABObject
			, IABSampleType **oOutputChannels
			, IABRenderedOutputChannelCountType iOutputChannelCount
			, IABRenderedOutputSampleCountType iOutputSampleBufferCount);

	private:

		// *** Private member functions
		// ***

		// Reset vbapObject_ to default state
		// This should be called before using it to render a new object
		iabError ResetVBAPObject();

		// *** Member variables
		// ***

		// Object SubBlock renderer
		IABObjectSubBlockRenderer subBlockRenderer_;

		// Corresponsing IAB use case matching soundfield line in the renderer configuration
		IABUseCaseType  targetUseCase_;

		// Number of audio channels output per config
		IABRenderedOutputChannelCountType numRendererOutputChannels_;

		// Pointer to a vbapRendererObject instance. This is a working data structure when rendering an object.
		// The instance must be reset to the default state with ResetVBAPObject() before it can be used for a new object.
		IABVBAP::vbapRendererObject *vbapObject_;

		// Internal work pointer for referencing parent IAB frame to be rendered.
		// It is NOT owned by Object Renderer. Caller owns it. Ensure we do NOT delete it even after use. 
		const IABFrame* iabFrameToRender_;

		// Frame rate for current IAB frame
		IABFrameRateType frameRate_;

		// Number of object panning subblocks for current frame rate in input IAB Frame, per IAB specification
		uint8_t numPanSubBlocks_;

		// Number of audio samples per channel output by the renderer
		IABRenderedOutputSampleCountType    frameSampleCount_;

		// Number of samples in each sub-block, per IAB specification.
		// 8 is maximum possible number of sub-blocks per specification.
		uint32_t subBlockSampleCount_[8];

		// Offset to point to start of sub-block samples in the frame sample buffer. 
		// 8 is maximum possible number of sub-blocks per specification.
		uint32_t subBlockSampleStartOffset_[8];

		// Flag to enable/disable 96k IAB stream rendering to 48k output.
		bool     render96kTo48k_;

		// Array of pointers to individual channels in the output buffer block.
		// When rendering an object definition, the renderer works on one subblock at a time and the pointers are updated
		// for each subblock to fill the correct segment of the output frame.
		// When rendering a bed channel as "object", no pointer update is required since the frame is rendered as a single block.
		IABSampleType                   **outputBufferPointers_;

		// Pointer to map for frame-wise audio data IDs to asset pointers. This is passed in from frame renderer.
		std::map<IABAudioDataIDType, IABSampleType*>*    frameAudioDataIDToAssetPointerMap_;

		// Object VBAP gain history. It is intialized to the map instance owned by frame renderer.
		// For MT, this is protected by a mutex.
		//
		std::map<uint32_t, IABGAINSPROC::EntityPastChannelGains>* frameEntityGainHistory_;

		// Method for finding "start" gains for object ID "iObjectID" from frameEntityGainHistory_.
		// If not found, a start gains of all 0.0f are used.
		iabError GetStartGainsInFrameGainsHistory(uint32_t iObjectID, IABGAINSPROC::EntityPastChannelGains& oStartGains);
        
        // Reference to the gaindHistoryMutex for synchronisation
        IABMutex& gainsHistoryMutex_;
	};

	/**
	* Set of parameters passed to object rendering worker function.
	*
	*/
	struct ObjectRendererMTWorkerParam {
		const IABObjectDefinition* iIABObject_;						// object to be rendered
		IABSampleType **oOutputChannels_;							// output buffers
		IABRenderedOutputChannelCountType iOutputChannelCount_;		// number of channels
		IABRenderedOutputSampleCountType iOutputSampleBufferCount_; // frame sample count
	};

	// *** Below for bed

	/**
	* Set of parameters from render-config, that are required in setting up IAB Bed Channel Renderer
	*
	*/
	struct BedChannelRendererParam {

		// Total list including virtual
		const std::vector<RenderUtils::RenderSpeaker>*	totalSpeakerList_;

		// Physical speakers to output buffer indices map.
		std::map<std::string, int32_t>*    physicalURIedBedSpeakerOutputIndexMap_;

		// Map containing URI'ed speakers to indices in totalSpeakerList_.
		std::map<std::string, int32_t>*    totalURIedSpeakerToTotalSpeakerListMap_;

		// This maps speaker index (of totalSpeakerList_) to output buffer index
		std::map<int32_t, int32_t>*        speakerIndexToOutputIndexMap_;

		// The VBAPRender (backup) in bed channel renderer requires full config for initialization
		RenderUtils::IRendererConfiguration *renderConfig_;

		// Number of speakers in the renderer configuration
		uint32_t speakerCount_;

		// Number of audio channels output by the renderer
		IABRenderedOutputChannelCountType   numRendererOutputChannels_;
	};

	/**
	* Set of parameters from render-config, that are required in setting up IAB Bed Remap Renderer
	*
	*/
	struct BedRemapRendererParam {

		// Total list including virtual
		const std::vector<RenderUtils::RenderSpeaker>*	totalSpeakerList_;

		// Physical speakers to output buffer indices map.
		std::map<std::string, int32_t>*    physicalURIedBedSpeakerOutputIndexMap_;

		// Map containing URI'ed speakers to indices in totalSpeakerList_.
		std::map<std::string, int32_t>*    totalURIedSpeakerToTotalSpeakerListMap_;

		// This maps speaker index (of totalSpeakerList_) to output buffer index
		std::map<int32_t, int32_t>*        speakerIndexToOutputIndexMap_;

		// The VBAPRender (backup) in bed remap renderer requires full config for initialization
		RenderUtils::IRendererConfiguration *renderConfig_;

		// Number of speakers in the renderer configuration
		uint32_t speakerCount_;

		// Number of audio channels output by the renderer
		IABRenderedOutputChannelCountType   numRendererOutputChannels_;
	};

	/**
	* Set of parameters from render-config, that are required in setting up IAB Bed Renderer
	*
	*/
	struct BedRendererParam {

		// Corresponding IAB use case matching soundfield line in the renderer configuration
		IABUseCaseType  targetUseCase_;

		// Number of audio output channels according to target config
		IABRenderedOutputChannelCountType   numRendererOutputChannels_;

		// Flag to enable/disable 96k IAB stream rendering to 48k output.
		bool     render96kTo48k_;

		// Set of setup params for bed channel renderer.
		BedChannelRendererParam bedChannelRendererParam_;

		// Set of setup params for bed remap renderer.
		BedRemapRendererParam bedRemapRendererParam_;
	};

	/**
	*
	* IAB Bed Channel Renderer class. For rendering a single channel.
	*
	*/
	class IABBedChannelRenderer
	{
	public:

		// Default Constructor
        IABBedChannelRenderer(std::vector<IABMutex>& perChOutputMutex);

		// Destructor
		~IABBedChannelRenderer();

		// Set up IABBedChannelRenderer based on "iBedChannelRendererParam".
		// Client must call SetUp() following creation of an IABBedChannelRenderer object.
		// 
		iabError SetUp(BedChannelRendererParam &iBedChannelRendererParam);

		// Clear cached rendered extent source stored in VBAPRenderer member object..
		// 
		void ClearVBAPCache();

		// Set container bed metadata ID
		void SetContainerBedID(uint32_t iBedMetaID);

		// Renders an IAB (Bed) Channel (iIABChannel) into output channels (oOutputChannels).
		//
		// When the speaker associated with the bed channel is present (through URI mapping) in the target 
		// layout, the bed channel will be sent directly into the output buffer associated with that speaker.
		// Otherwise, the bed channel will be rendered based on nominal spatial coordinates, as a point 
		// source. Output may appear in multiple speakers.
		//
		// Note: Caller retains ownership to (*iIABChannel) object after being rendered.
		//
		// Note: Rendered output samples are accumulated (NOT overwritten) to sample buffer 
		// pointed to by "oOutputChannel".
		// 
		iabError RenderIABChannel(const IABChannelInterface& iIABChannel
			, IABSampleType *iAssetSamples
			, IABSampleType **oOutputChannels
			, IABRenderedOutputChannelCountType iOutputChannelCount
			, IABRenderedOutputSampleCountType iOutputSampleBufferCount);

	private:

		// *** Private member functions
		// ***

		// *** Member variables
		// ***

		// Variable holding MetaID of Bed element that contains the channel
		uint32_t containerBedMetaID_;

		// Total list including virtual
		const std::vector<RenderUtils::RenderSpeaker>*	totalSpeakerList_;

		// This map lists each PHYSICAL speaker that has a URI (i.e. a bed channel) with its associated
		// position in the output buffer block.
		// Map key is the speaker's URI and map value is the associated position in the output buffer block
		// This map is used to find the associated buffer when rendering a bed channel directly to a mapped physical
		// speaker that is present in the config file.
		// (This map does NOT contain any virtual speakers that have URIs.)
		std::map<std::string, int32_t>*    physicalURIedBedSpeakerOutputIndexMap_;

		// This map contains all speakers that have URIs, including virtual speaker.
		std::map<std::string, int32_t>*    totalURIedSpeakerToTotalSpeakerListMap_;

		// This maps speaker index (of totalSpeakerList_) to output buffer index
		std::map<int32_t, int32_t>*        speakerIndexToOutputIndexMap_;

		// ========
		// Channel renderer also contains a VBAP renderer, as a backup renderer when direct routine (cRE)
		// is not possible (when target config does not contain the matching speaker).

		// Pointer to instance of vbap renderer. Object created by, and owned by IABBedChannelRenderer instance.
		// IABBedChannelRenderer Destructor must delete object upon instance destruction.
		IABVBAP::VBAPRenderer     *vbapRenderer_;

		// Pointer to instance of ChannelGainsProcessorMT. Object created by, and owned by IABBedChannelRenderer 
		// instance. IABBedChannelRenderer Destructor must delete object upon instance destruction.
		IABGAINSPROC::ChannelGainsProcessorMT     *channelGainsProcessor_;

		// Number of speakers in the renderer configuration
		// ( = numRendererOutputChannels_ + virual speakers)
		uint32_t speakerCount_;

		// Number of audio channels output by the renderer
		IABRenderedOutputChannelCountType   numRendererOutputChannels_;

		// Pointer to a vbapRendererObject instance. For non-cRE channel, a vbap object must be
		// constructed to eumulate a bed channel
		IABVBAP::vbapRendererObject *vbapObject_;
        
        // Per channel output mutex for thread synch
        std::vector<IABMutex>& perChOutputMutex_;

	};

	/**
	*
	* IAB Bed Remap Renderer class. For rendering a bed remap element.
	*
	* Bed remap can only be a sub-element of an IAB bed. Therefore, an instance/object of this
	* class can only exist for bed rendering, eg. as a member of IABBedRenderer class.
	*
	*/
	class IABBedRemapRenderer
	{
	public:

		// Default Constructor
		IABBedRemapRenderer(std::vector<IABMutex>& perChOutputMutex);

		// Destructor
		~IABBedRemapRenderer();

		// Set up IABBedRemapRenderer based on "iBedRemapRendererParam".
		// Client must call SetUp() following creation of an IABBedRemapRenderer object.
		// 
		iabError SetUp(BedRemapRendererParam &iBedRemapRendererParam);

		// Set frame parameters to bed remap renderer.
		// Called from frame rendering - once only if unchanged.
		// 
		iabError SetFrameParameter(FrameParam &iFrameParam);

		// Clear cached rendered extent source stored in VBAPRenderer member object..
		// 
		void ClearVBAPCache();

		// Renders an IAB Bed Remap (iIABBedRemap) into output channels (ioOutputChannels).
		//
		// Note: Caller retains ownership to (*iIABChannel) object after being rendered.
		//
		// Note: Rendered output samples are accumulated (NOT overwritten) to sample buffers 
		// pointed to by "oOutputChannels".
		// 
		iabError RenderIABBedRemap(const IABBedRemap& iIABBedRemap
			, const IABBedDefinition& iParentBed
			, IABSampleType **oOutputChannels
			, IABRenderedOutputChannelCountType iOutputChannelCount
			, IABRenderedOutputSampleCountType iOutputSampleBufferCount);

	private:

		// *** Private member functions
		// ***

		// *** Member variables
		// ***

		// Total list including virtual
		const std::vector<RenderUtils::RenderSpeaker>*	totalSpeakerList_;

		// This map lists each PHYSICAL speaker that has a URI (i.e. a bed channel) with its associated
		// position in the output buffer block.
		// Map key is the speaker's URI and map value is the associated position in the output buffer block
		// This map is used to find the associated buffer when rendering a bed channel directly to a mapped physical
		// speaker that is present in the config file.
		// (This map does NOT contain any virtual speakers that have URIs.)
		std::map<std::string, int32_t>*    physicalURIedBedSpeakerOutputIndexMap_;

		// This map contains all speakers that have URIs, including virtual speaker.
		std::map<std::string, int32_t>*    totalURIedSpeakerToTotalSpeakerListMap_;

		// This maps speaker index (of totalSpeakerList_) to output buffer index
		std::map<int32_t, int32_t>*        speakerIndexToOutputIndexMap_;

		// ========
		// BedRemap renderer also contains a VBAP renderer, as a backup renderer when a remap destination
		// channel does not find a matching speaker in target config.

		// Pointer to instance of vbap renderer. Object created by, and owned by IABBedRemapRenderer instance.
		// IABBedRemapRenderer Destructor must delete object upon instance destruction.
		IABVBAP::VBAPRenderer     *vbapRenderer_;

		// Pointer to instance of ChannelGainsProcessorMT.Object created by, and owned by IABBedRemapRenderer
		// instance. IABBedRemapRenderer Destructor must delete object upon instance destruction.
		IABGAINSPROC::ChannelGainsProcessorMT     *channelGainsProcessor_;

		// Number of speakers in the renderer configuration
		// ( = numRendererOutputChannels_ + virual speakers)
		uint32_t speakerCount_;

		// Number of audio channels output by the renderer
		IABRenderedOutputChannelCountType   numRendererOutputChannels_;

		// Pointer to a vbapRendererObject instance. For a destination channel that does not find a matching
		// speaker in target config, a vbap object must be constructed to eumulate the destination channel
		IABVBAP::vbapRendererObject *vbapObject_;

		// Frame rate for current IAB frame
		IABFrameRateType frameRate_;

		// Number of object panning subblocks for current frame rate in input IAB Frame, per IAB specification
		uint8_t numPanSubBlocks_;

		// Number of audio samples per channel output by the renderer
		IABRenderedOutputSampleCountType    frameSampleCount_;

		// Number of samples in each sub-block, per IAB specification.
		// 8 is maximum possible number of sub-blocks per specification.
		uint32_t subBlockSampleCount_[8];

		// Offset to point to start of sub-block samples in the frame sample buffer. 
		// 8 is maximum possible number of sub-blocks per specification.
		uint32_t subBlockSampleStartOffset_[8];

		// Array of pointers to individual channels in the output buffer block.
		// When rendering an bed remap, the renderer works on one remap subblock at a time and the pointers 
		// are updated for each remap subblock to fill the correct segment of the output frame.
		// This is unlike rendering a bed channel as "object", which does not have subblocks, and therefore
		// the entire frame is rendered as a single (albeit large) block.
		IABSampleType  **outputBufferPointers_;

		// Pointer to map for frame-wise audio data IDs to asset pointers. This is passed in from frame renderer.
		std::map<IABAudioDataIDType, IABSampleType*>*    frameAudioDataIDToAssetPointerMap_;
        
        // Reference to multi threaded channel write protection
        std::vector<IABMutex>& perChOutputMutex_;
	};

	/**
	*
	* IAB Bed Renderer class. For rendering an IAB bed.
	*
	*/
	class IABBedRenderer
	{
	public:

		// Default Constructor
        IABBedRenderer(std::vector<IABMutex>& perChOutputMutex);

		// Destructor
		~IABBedRenderer();

		// Set up IABBedRenderer based on "iBedRendererParam".
		// Client must call SetUp() following creation of an IABBedRenderer object.
		// 
		iabError SetUp(BedRendererParam &iBedRendererParam);

		// Set frame parameters to bed renderer.
		// Called from frame rendering - once only if unchanged.
		// 
		iabError SetFrameParameter(FrameParam &iFrameParam);

		// Renders an IAB Bed (iIABBed) into output channels (ioOutputChannels).
		// Note: Caller retains ownership to (*iIABBed) bed after being rendered.
		//
		// Note: Rendered output samples are accumulated (NOT overwritten) to sample buffers 
		// pointed to by "oOutputChannels".
		// 
		iabError RenderIABBed(const IABBedDefinition& iIABBed
			, IABSampleType **oOutputChannels
			, IABRenderedOutputChannelCountType iOutputChannelCount
			, IABRenderedOutputSampleCountType iOutputSampleBufferCount);

	private:

		// *** Member variables
		// ***

		// Bed Channel renderer
		IABBedChannelRenderer channelRenderer_;

		// Bed Remap renderer
		IABBedRemapRenderer remapRenderer_;

		// Corresponsing IAB use case matching soundfield line in the renderer configuration
		IABUseCaseType  targetUseCase_;

		// Number of audio channels output per config
		IABRenderedOutputChannelCountType numRendererOutputChannels_;

		// Number of audio samples per channel output by the renderer
		IABRenderedOutputSampleCountType    frameSampleCount_;

		// Flag to enable/disable 96k IAB stream rendering to 48k output.
		bool     render96kTo48k_;

		// Pointer to map for frame-wise audio data IDs to asset pointers. This is passed in from frame renderer.
		std::map<IABAudioDataIDType, IABSampleType*>*    frameAudioDataIDToAssetPointerMap_;
	};

	/**
	* Set of parameters passed to bed rendering worker function.
	*
	*/
	struct BedRendererMTWorkerParam {
		const IABBedDefinition* iIABBed_;							// bed to be rendered
		IABSampleType **oOutputChannels_;							// output buffers
		IABRenderedOutputChannelCountType iOutputChannelCount_;		// number of channels
		IABRenderedOutputSampleCountType iOutputSampleBufferCount_; // frame sample count
	};

	// *** Below for asset decoding

	/**
	* Set of parameters for setting up asset decoder
	*
	*/
	struct AssetDecoderParam {

		// Flag to enable/disable 96k IAB stream rendering to 48k output.
		bool     render96kTo48k_;
	};


	/**
	*
	* IAB audio asset decoder class. For decoding IAB DLC or PCM audio data elements.
	*
	*/
	class IABAudioAssetDecoder
	{
	public:

		// Default Constructor
		IABAudioAssetDecoder();

		// Destructor
		~IABAudioAssetDecoder();

		// Set up IABAudioAssetDecoder based on "iAssetDecoderParam".
		// Client must call SetUp() following creation of an IABAudioAssetDecoder object.
		// 
		iabError SetUp(AssetDecoderParam &iAssetDecoderParam);

		// Set frame parameters to asset decoder.
		// Called from frame rendering - once only if unchanged.
		// 
		iabError SetFrameParameter(FrameParam &iFrameParam);

		// Decodes an IAB DLC element into output PCM samples (ioOutputChannels).
		// Note: Caller retains ownership to (*iIABAudioDLC) object after decoding.
		//
		// Note: Output sample buffer "iOutputSampleBuffer" will be overwritten with decoded samples.
		// 
		iabError DecodeIABAsset(IABAudioDataDLC* iIABAudioDLC, IABSampleType* iOutputSampleBuffer);

		// Unpack from an IAB PCM element into output PCM samples (ioOutputChannels).
		// (An overloading method to DLC decoding).
		// Note: Caller retains ownership to (*iIABAudioPCM) object after unpacking.
		//
		// Note: Output sample buffer "iOutputSampleBuffer" will be overwritten with unpacked samples.
		// 
		iabError DecodeIABAsset(IABAudioDataPCM* iIABAudioPCM, IABSampleType* iOutputSampleBuffer);

	private:

		// *** Member variables
		// ***

		// Flag to enable/disable 96k IAB stream rendering to 48k output.
		bool     render96kTo48k_;

		// Number of asset samples per frame
		IABRenderedOutputSampleCountType    frameSampleCount_;
	};

	/**
	* Set of parameters passed to DLC decoder or PCM unpacker worker function.
	*
	*/
	struct AssetDecoderMTWorkerParam {
		IABAudioDataDLC* iIABAudioDLC_;								// DLC element to be decoded (either this, or PCM below)
		IABAudioDataPCM* iIABAudioPCM_;								// PCM element to be unpacked (either this, or DLC above)
		IABSampleType* iOutputSampleBuffer;							// output sample buffer
	};

	/**
	* Struct for job parameters in queue.
	*
	*/
	struct QueueJobParam {
		IABElementIDType elementType_;								// Indicate which of the 4 types of element are being worked on
		ObjectRendererMTWorkerParam objectRenderParam_;				// parameters for object rendering
		BedRendererMTWorkerParam bedRenderParam_;					// parameters for bed rendering
		AssetDecoderMTWorkerParam assetDecodeParam_;				// parameters for asset decoding
	};

	class IABRendererMT;  // forward declaration

	/**
	* Struct for passing parameters to thread worker function (mainly intialization
	* which include set up pointer to queue, setting up processing engines for each thread).
	*
	*/
	struct ThreadWorkerFunctionParam {
		IABRendererMT* rendererMT_;									// pointer to the multi-threaded renderer
		IABObjectRenderer* threadObjectRenderer_;					// pointer to object renderer instance for thread
		IABBedRenderer* threadBedRenderer_;							// pointer to bed renderer instance for thread
		IABAudioAssetDecoder* threadAssetDecoder_;					// pointer to asset decoder instance for thread
	};

    /**
     *
	 * IAB Renderer class to render an immersive audio bitstream.
     *
     */
    class IABRendererMT : public IABRendererMTInterface
    {
    public:

        // Constructor
        IABRendererMT(RenderUtils::IRendererConfiguration &iConfig, uint32_t iThreadPoolSize);
        
		// Destructor
        ~IABRendererMT();
        
        // Returns the number of audio channels output by the renderer.
        IABRenderedOutputChannelCountType GetOutputChannelCount() const;
        
        // Returns maximum number of audio samples per channel output by the renderer.
        IABRenderedOutputSampleCountType GetMaxOutputSampleCount() const;
        
		// Returns number of cores in host PC.
		// 
		uint32_t GetProcessorCoreNumber();

		// Renders an IAB frame (iIABFrame) into output channels (oOutputChannels).
		//
		// Note: Caller retains ownership to (*iIABFrame) object after being rendered.
        //
        // Note: Buffer initialization to "0" is carried out by function to all sample buffers 
        // pointed to by "oOutputChannels" array of pointers.
        // 
        iabError RenderIABFrame(const IABFrameInterface& iIABFrame
                                , IABSampleType **oOutputChannels
                                , IABRenderedOutputChannelCountType iOutputChannelCount
                                , IABRenderedOutputSampleCountType iOutputSampleBufferCount);

		// Returns the job queue (shared by all threads)
		std::queue<QueueJobParam>* jobQueue() { return &renderJobQueue_; }
        
        // Accessors to synchronisation entities for the
        // worker thread
        void lockJobQueueMutex() { jobQueueMutex.lock(); }
        void unlockJobQueueMutex() { jobQueueMutex.unlock(); }
        void waitForJobQueueEntries() { jobQueueHasEntries.wait(jobQueueMutex); }
        
        // Decrement jobCount, sets the error code from the completed job
        // !Important to do this at the end of job completion, not before, so that
        // signal to main thread can be sent at the right time for MT.
        void decrementJobCount(iabError errorCode);

		// True if renderer should terminate
		bool doTerminate() const { return doTerminate_; }

    private:

		// Set up IABRendererMT based on "iConfig" 
		// 
		void SetUp(RenderUtils::IRendererConfiguration &iConfig);

		// *** Member variables

		// Flag to enable/disable 96k IAB stream rendering to 48k output.
		bool     render96kTo48k_;

		// Flag to enable/disable cross-frame/subblock sample smoothing processing.
		//  - Enable/disable as set by render config file "c smooth 1|0" flag, with default to "enabled".
		//  - No exposed getter/setter API
		bool     enableSmoothing_;

		// Flag to enable/disable decorrelation as defined in IAB stream.
		// At v1.1, this impacts object decorrelation only
		//  - Enable/disable as set by render config file "c decorr 1|0" flag, with default to "enabled".
		//  - No exposed getter/setter API
		bool     enableDecorrelation_;

        // Number of audio channels output by the renderer
        IABRenderedOutputChannelCountType   numRendererOutputChannels_;
        
		// Speaker data structures from the renderer configuration
		// Physical
		const std::vector<RenderUtils::RenderSpeaker>*	physicalSpeakerList_;

		// Total list including virtual
		const std::vector<RenderUtils::RenderSpeaker>*	totalSpeakerList_;

		// Soundfield line in the renderer configuration
		std::string     targetSoundfield_;

		// Corresponsing IAB use case matching soundfield line in the renderer configuration
		IABUseCaseType  targetUseCase_;

		// Number of speakers in the renderer configuration
		uint32_t speakerCount_;

		// Frame rate for current IAB frame
		IABFrameRateType frameRate_;

		// Sample rate for current IAB frame
		IABSampleRateType sampleRate_;

		// Number of samples in frame, for current frame rate in input IAB Frame, per IAB specification
		IABRenderedOutputSampleCountType frameSampleCount_;

		// Number of object panning subblocks for current frame rate in input IAB Frame, per IAB specification
        uint8_t  numPanSubBlocks_;

		// Number of samples in each sub-block, per IAB specification.
		// 8 is maximum possible number of sub-blocks per specification.
		uint32_t subBlockSampleCount_[8];

		// Offset to point to start of sub-block samples in the frame sample buffer. 
		// 8 is maximum possible number of sub-blocks per specification.
		uint32_t subBlockSampleStartOffset_[8];

		// Internal work pointer for referencing IAB frame to be rendered. Object set and owned by caller to RenderIABFrame(). 
        const IABFrame*     iabFrameToRender_;
               
        // This map lists each PHYSICAL speaker that has a URI (i.e. a bed channel) with its associated
        // position in the output buffer block.
        // Map key is the speaker's URI and map value is the associated position in the output buffer block
        // This map is used to find the associated buffer when rendering a bed channel directly to a mapped physical
		// speaker that is present in the config file.
		// (This map does NOT contain any virtual speakers that have URIs.)
        std::map<std::string, int32_t>    physicalURIedBedSpeakerOutputIndexMap_;

		// This map contains all speakers that have URIs, including virtual speaker.
		std::map<std::string, int32_t>    totalURIedSpeakerToTotalSpeakerListMap_;

		// This maps speaker index (of totalSpeakerList_) to output buffer index
		std::map<int32_t, int32_t>        speakerIndexToOutputIndexMap_;

		// Map linking channel index of each config file VBAP speaker to its position coordinate in the IAB unit cube
        // This map is used for checking if object position is within the snap tolerance.
        std::map<int32_t, CartesianPosInUnitCube>   vbapSpeakerChannelIABPositionMap_;

		// *** Add support for object decorrelation in binary ON/OFF mode.
		//

		// IAB decorrelators. Object decorr at v1.1.
		IABDecorrelation *iabDecorrelation_;

		// Flag to indicate if iabDecorrelation_ is in reset state.
		bool decorrelationInReset_;

		// Counter for managing decorr tailing off period (hysteresis) from consecutive frames that contain 
		// objects with decorr == ON. For any frames with decorrTailingFramesCount_ > 0, decorrelation 
		// processing is carried out after rendering all objects inside RenderIABFrame(), as a post-normal-
		// rendering step. If decorrTailingFramesCount_ == 0, no decorr processing is needed.
		// The decorr processing tailing period (number of frames) can be controlled by applying a suitable
		// positive integer so as to gracefully tailing off "samples" from decorr APFs' internal delay lines. 
		int32_t decorrTailingFramesCount_;

		// PCM buffers for holding rendered output samples needing decorrelation processing.
		// These are added to total frame rendered output as last step before returning to caller.
		//

		// Work buffer for holding rendered output sample that need further decorrelation processing.
		// This buffer block contains all rendered output channels for decorr, arranged in a channel 
		// by channel basis, i.e. non-interleaving.
		// Note: Channel order is based on target configuration iConfig, with which the IABRendererMT instance
		// is created.
		//
		IABSampleType *decorrOutputBuffer_;

		// Array of output buffer pointers. Each pointer points to the start of a rendered output channel
		// that is to be decorrelated.
		IABSampleType  **decorrOutputChannelPointers_;

		// VectDSP acceleration engine for summing up C output with D output
		CoreUtils::VectDSPInterface *vectDSP_;

		// *** Variables to support IABRenderMT internal multi-threading
		//

		// PCM buffers for holding decoded audio assets from DLC/PCM.
		//
		IABSampleType *assetSampleBuffer_;

		// Array of asset sample buffer pointers. Each pointer points to the start of an audio asset.
		IABSampleType  **assetSampleBufferPointers_;

		// Maps an audio data ID to its decoded asset pointer.
		std::map<IABAudioDataIDType, IABSampleType*>    frameAudioDataIDToAssetPointerMap_;

		// Object VBAP gain history. Used to support preceding object channel gains, per object ID,
		// to support smoothing processing (when enabled).
		// Owned by this class. Can be accessed by member object renderers. Mutex protected.
		//
		std::map<uint32_t, IABGAINSPROC::EntityPastChannelGains>   frameEntityGainHistory_;

		// Private method for updating gains history frameEntityGainHistory_ for smoothing.
		// Specifically, it deletes unused entries in gains history and retains used entries.
		void UpdateFrameGainsHistory();

		// Reset gains history frameEntityGainHistory_ to empty.
		void ResetFrameGainsHistory();

		// **************************************************

		// *** MT IABRenderer job queue. 4 types of elements can go into this queue for each frame, in the
		// following sequence
		//   1. Audio data elements (assets) - DLC and PCM can be simutaneous if necessary (but practically only one type)
		//      (Asset decoding must be complete before adding objects and beds to queue)
		//   2. Objects and beds
		//
		std::queue<QueueJobParam> renderJobQueue_;

		// Job param carrier. "elementType_" member serves as the most important "key" that determines
		// which processing job the MTRenderThreadWorker will carry out.
		// (It never own any deep memories, but only as messenger.)
		QueueJobParam jobParameterCarrier_;

		// Parameters for setting up threads
		std::vector<ThreadWorkerFunctionParam*> threadFunctionParameters_;

		// Maximum number of threads to use. Default to 4 but can be set during IABRendererMT instance set up.
		uint32_t threadPoolSize_;

		// Pool of threads, containing threadPoolSize_ of thread IDs.
		std::vector<pthread_t> threads_;

		// Pool of asset decoders. Each thread has its own asset decoder for maximum confinement.
		std::vector<IABAudioAssetDecoder*>  iabAssetDecoders_;

		// Same number of struct instances to carry parameters to pass to object worker function.
		std::vector<AssetDecoderMTWorkerParam*>  assetWorkerParams_;

		// Pool of object renderers. Each thread has its own object renderer for maximum confinement.
		std::vector<IABObjectRenderer*>  iabObjectRenderers_;

		// Same number of struct instances to carry parameters to pass to asset decode worker function.
		std::vector<ObjectRendererMTWorkerParam*>  objectWorkerParams_;

		// Pool of bed renderers. Each thread has its own bed renderer for maximum confinement.
		std::vector<IABBedRenderer*>  iabBedRenderers_;

		// Same number of struct instances to carry parameters to pass to bed worker function.
		std::vector<BedRendererMTWorkerParam*>  bedWorkerParams_;

		// Flag on indicates init completion. Init once only, and complete at first frame.
		// This assumes that all frames of the same programe have identical frame parameters.
		bool objectRenderersAreInited_;

		// Flag on indicates init completion. Init once only, and complete at first frame.
		// This assumes that all frames of the same programe have identical frame parameters.
		bool bedRenderersAreInited_;

		// Flag on indicates init completion. Init once only, and complete at first frame.
		// This assumes that all frames of the same programe have identical asset FR&SR parameters.
		bool assetDecodersAreInited_;

		// Renderer and all of its threads should terminate
		bool doTerminate_;

		// Records the error code if any threaded job returns an error
		//
		// This will only be written/read when jobCountMutex is locked
		iabError errorCode_;

		// Records warnings if any threaded job returns a warning
		//
		// This is only written when jobCountMutex is locked or no threads are running
		// This is only read when no threads are running
		iabError warningCode_;

		// Thread synchronisation entities

		static const uint32_t kMaxOutputChannels = 100;

		// Mutex for writing to a channel's output buffer from all object and bed rendering.
		// Initialized in Setup()
        std::vector<IABMutex> perChOutputMutex;

		// Mutex for accessing cross-frame gains cache.
		IABMutex gainsHistoryMutex;

		// Mutex for accessing job queue.
		IABMutex jobQueueMutex;

		// Condition variable for job queue. 
		// jobQueueHasEntries flags entries are present in queue. This is a signal sent from main thread to all worker threads
		// ie. in broadcast mode
		IABMutex::IABCondition jobQueueHasEntries;

		// As the actual decoding, rendering job processing is done in thread worker outside the "jobQueueMutex" lock, need another 
		// mechanism to check and confirm the completion of actual work. Here, a "global" job count is used (an programming exception
		// in a way). This is accompanied with its own mutex and condition variable for maximum efficientcy.
		int32_t jobCount;

		// Mutex for accessing jobCount.
		IABMutex jobCountMutex;

		// Condition variable for signal actual completion of batch of jobs in queue. This is a signal sent from worker threads back 
		// to main. (in single signal mode)
		IABMutex::IABCondition jobCompletion;

	};

} // namespace ImmersiveAudioBitstream
} // namespace SMPTE

#endif // #if __linux__ || __APPLE__

#endif // __IABRENDERERMT_H__
