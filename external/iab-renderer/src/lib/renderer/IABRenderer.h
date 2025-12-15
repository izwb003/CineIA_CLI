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
 * IABRenderer.h
 *
 * @file
 */


#ifndef __IABRENDERER_H__
#define	__IABRENDERER_H__

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
     *
     * IAB Renderer class to render an immersive audio bitstream.
     *
     */
    
    class IABRenderer : public IABRendererInterface
    {
    public:

        // Constructor
        IABRenderer(RenderUtils::IRendererConfiguration &iConfig);
        
		// Note: DEV-INTERNAL
		// Constructor for development/internal/test usues.
		//
		// iFrameGainsCacheEnable = true, to enable rendering gains cache between 2 sccessive frames.
		// iFrameGainsCacheEnable = false, to disable gains cache between 2 sccessive frames.
		//
		IABRenderer(RenderUtils::IRendererConfiguration &iConfig, bool iFrameGainsCacheEnable);

		// Destructor
        ~IABRenderer();
        
        // Returns the number of audio channels output by the renderer.
        IABRenderedOutputChannelCountType GetOutputChannelCount() const;
        
        // Returns maximum number of audio samples per channel output by the renderer.
        IABRenderedOutputSampleCountType GetMaxOutputSampleCount() const;
        
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
                                , IABRenderedOutputSampleCountType iOutputSampleBufferCount
                                , IABRenderedOutputSampleCountType &oRenderedOutputSampleCount);

    private:

		// Set up IABRenderer based on "iConfig" 
		// 
		void SetUp(RenderUtils::IRendererConfiguration &iConfig);

		// Class methods for rendering IAB elements of types from Frame to Objects
		// Through the hierarchy
		//
		// Note: implementation level rolling out to first release
		//
		// All methods remain internal to IABRenderer, until reviewed and agreed for 
		// elevation to public, as per necessary.
        
        // Renders an IAB Object (iIABObject) into output channels (ioOutputChannels).
        //
        // Note: Rendered output samples are accumulated (NOT overwritten) to sample buffers 
        // pointed to by "oOutputChannels".
        // 
        iabError RenderIABObject(const IABObjectDefinitionInterface& iIABObject
                                 , IABSampleType **oOutputChannels
                                 , IABRenderedOutputChannelCountType iOutputChannelCount
                                 , IABRenderedOutputSampleCountType iOutputSampleBufferCount
                                 , IABRenderedOutputSampleCountType &oRenderedOutputSampleCount);

        // Renders an IAB Object Sub Block(iIABObjectSubBlock) into output channels (ioOutputChannels).
        //
        // Note: Rendered output samples are accumulated (NOT overwritten) to sample buffers 
        // pointed to by "oOutputChannels".
        // 
        iabError RenderIABObjectSubBlock(const IABObjectSubBlockInterface& iIABObjectSubBlock
                                         , IABVBAP::vbapRendererObject *iVbapObject
                                         , IABSampleType *iAssetSamples
                                         , IABSampleType **oOutputChannels
                                         , IABRenderedOutputChannelCountType iOutputChannelCount
                                         , IABRenderedOutputSampleCountType iOutputSampleBufferCount
                                         , IABRenderedOutputSampleCountType &oRenderedOutputSampleCount);

        // Check if an ObjectDefinition element is activated (available) for rendering.
        // Note, "activated" does not necessarily mean "to be rendered". Other conditions
        // must be met. (See page 28 of published ST2098-2 specification.)
        //
        bool IsObjectActivatedForRendering(const IABObjectDefinition* iIABObject) const;

        // Renders an IAB Bed (iIABBed) into output channels (ioOutputChannels).
        //
        // Note: Rendered output samples are accumulated (NOT overwritten) to sample buffers 
        // pointed to by "oOutputChannels".
        // 
        iabError RenderIABBed(const IABBedDefinitionInterface& iIABBed
                              , IABSampleType **oOutputChannels
                              , IABRenderedOutputChannelCountType iOutputChannelCount
                              , IABRenderedOutputSampleCountType iOutputSampleBufferCount
                              , IABRenderedOutputSampleCountType &oRenderedOutputSampleCount);

		// Check if a BedDefinition element is activated (available) for rendering.
		// Note, "activated" does not necessarily mean "to be rendered". Other conditions
		// must be met. (See page 25 of published ST2098-2 specification.)
		//
		bool IsBedActivatedForRendering(const IABBedDefinition* iIABBed) const;

		// Check if a BedRemap element is activated (available) for rendering.
		// Generally in the case of BedRemap, "activated" does mean "to be used". This is unlike
		// BedDefinition element which may contain further sub-element.
		//
		bool IsBedRemapActivatedForRendering(const IABBedRemap* iIABBedRemap) const;

		// Renders an IAB (Bed) Channel (iIABChannel) into output channels (oOutputChannels).
        // When the speaker associated with the bed channel is present in the target layout
        // the bed channel will be rendered into the output buffer associated with that speaker.
        // On the other hand, the bed channel will be rendered using RenderIABChannelAsObject,
        // based on spatial coordinates of that speaker. In the later case, the channel content
        // may be rendered into multiple output channels.
        // Note: Rendered output samples are accumulated (NOT overwritten) to sample buffer 
        // pointed to by "oOutputChannel".
        // 
        iabError RenderIABChannel(const IABChannelInterface& iIABChannel
                                  , IABSampleType *iAssetSamples
                                  , IABSampleType **oOutputChannels
                                  , IABRenderedOutputChannelCountType iOutputChannelCount
                                  , IABRenderedOutputSampleCountType iOutputSampleBufferCount
                                  , IABRenderedOutputSampleCountType &oRenderedOutputSampleCount);
        
        // Renders an IAB (Bed) Channel as an object into one or more output channels (oOutputChannels).
        // The method is used when the target layout does not contain the speaker associated with this channel.
        // Note: Rendered output samples are accumulated (NOT overwritten) to sample buffer
        // pointed to by "oOutputChannel".
        //
        iabError RenderIABChannelAsObject(IABChannelIDType iChannelID
                                          , float iChannelGain
                                          , IABSampleType *iAssetSamples
                                          , IABSampleType **oOutputChannels
                                          , IABRenderedOutputChannelCountType iOutputChannelCount
                                          , IABRenderedOutputSampleCountType iOutputSampleBufferCount
                                          , IABRenderedOutputSampleCountType &oRenderedOutputSampleCount);


        // Renders an IAB Bed Remap (iIABBedRemap) into output channels (ioOutputChannels).
        //
        // Note: Rendered output samples are accumulated (NOT overwritten) to sample buffers 
        // pointed to by "oOutputChannels".
        // 
        iabError RenderIABBedRemap(const IABBedRemapInterface& iIABBedRemap
                                   , const IABBedDefinitionInterface& iParentBed
                                   , IABSampleType **oOutputChannels
                                   , IABRenderedOutputChannelCountType iOutputChannelCount
                                   , IABRenderedOutputSampleCountType iOutputSampleBufferCount
                                   , IABRenderedOutputSampleCountType &oRenderedOutputSampleCount);

        // Renders an IAB Object Zone 19 (iIABZone19) into output channels (ioOutputChannels).
        //
        // Note: Rendered output samples are accumulated (NOT overwritten) to sample buffers 
        // pointed to by "oOutputChannels".
        // 
        iabError RenderIABObjectZone19(const IABObjectZoneDefinition19Interface& iIABZone19
                                       , IABSampleType **oOutputChannels
                                       , IABRenderedOutputChannelCountType iOutputChannelCount
                                       , IABRenderedOutputSampleCountType iOutputSampleBufferCount
                                       , IABRenderedOutputSampleCountType &oRenderedOutputSampleCount);

        // Renders an IAB Object Zone 19 Sub Block (iIABZone19SubBlock) into output channels (ioOutputChannels).
        //
        // Note: Rendered output samples are accumulated (NOT overwritten) to sample buffers 
        // pointed to by "oOutputChannels".
        // 
        iabError RenderIABZone19SubBlock(const IABZone19SubBlockInterface& iIABZone19SubBlock
                                         , IABSampleType **oOutputChannels
                                         , IABRenderedOutputChannelCountType iOutputChannelCount
                                         , IABRenderedOutputSampleCountType iOutputSampleBufferCount
                                         , IABRenderedOutputSampleCountType &oRenderedOutputSampleCount);

        // Update iOutputSampleBuffer contents with audio samples of IABAudioDataDLC or IABAudioDataPCM with iAudioDataID.
        // The PCM samples are converted to floating-point data and stored in buffer iOutputSampleBuffer.
        iabError UpdateAudioSampleBuffer(IABAudioDataIDType iAudioDataID, IABSampleType* iOutputSampleBuffer);
        
		// Similar to above, except that PCM samples are saved to sampleBufferFloat_.
		// Update audio sample buffer contents with samples of the current object/bed channel being rendered.
		// Samples are obtained using iAudioDataID, then converted to float and stored in the
		// working float buffer (sampleBufferFloat_). IABRenderer uses this float buffer
		// to store audio samples of the object/bed channel being rendered.
		iabError UpdateAudioSampleBuffer(IABAudioDataIDType iAudioDataID);

		// Reset vbapObject_ to default state
        // This should be called before using it to render a new object
        iabError ResetVBAPObject();
        
        // Finds index of the config file speaker that satisfies the snap criteria
        // 1. Absolute diff between corresponding Speaker position X, Y, Z and object position X, Y, Z all <= snap tolerance AND
        // 2. Speaker closest to the object will be the snap target and its channel index within the config file is returned.
        // A return value of -1 indicates that no speaker satisfies the criteria, i.e. object snap is not activated.
        int32_t FindSnapSpeakerIndex(CartesianPosInUnitCube iObjectPosition, float iSnapTolerance);

		// Call to enable rendering 96k IAB to 48k output
		void Enable96kTo48kRendering() { render96kTo48k_ = true; }

		// Call to disable rendering 96k IAB to 48k output. Output is in 96k.
		void Disable96kTo48kRendering() { render96kTo48k_ = false; }

        // Checks to see if the sample rate and frame rate combination is supported for rendering.
        bool IsSupported(IABFrameRateType iFrameRate, IABSampleRateType iSampleRate);

		// *** Member variables

		// Keeps track of any non-fatal warnings that occur
		std::map<iabError, int> warnings_;

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

		// Pointer to instance of vbap renderer. Object created by, and owned by IABRenderer instance.
		// IABRenderer Destructor must delete object upon instance destruction.
        IABVBAP::VBAPRenderer     *vbapRenderer_;

		// Pointer to instance of ChannelGainsProcessor.Object created by, and owned by IABRenderer instance.
		// IABRenderer Destructor must delete object upon instance destruction.
		IABGAINSPROC::ChannelGainsProcessor     *channelGainsProcessor_;

		// Variable to help passing metaID of parent element (eg. bed, object) to its children.
		uint32_t parentMetaID_;

        // Number of audio channels output by the renderer
        IABRenderedOutputChannelCountType   numRendererOutputChannels_;
        
        // Number of audio samples per channel output by the renderer
        IABRenderedOutputSampleCountType    numSamplePerRendererOutputChannel_;
        
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
		uint32_t frameSampleCount_;

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

        // Interior extended source to vbap extended source conversion class instance
        IABInterior iabInterior_;
        
        // Object zone9 control
        IABObjectZone9 *iabObjectZone9_;
        
		// Notes:
        // In some real time processing applications or use cases, it is desirable allocate memory during renderer initialisation only
        // because allocation/de-allocation takes time and could cause problems if these happen during time-critcal processing.
        // To support this requirement, the following working buffers and data structures will allocated during IABRenderer
        // initialisation and re-used when rendering each object (also used when rendering a bed channell as object).
        // There will be no memory allocation during any post-initialisation IABRenderer calls.
        
        // Pointer to a vbapRendererObject instance. This is a working data structure when rendering an object.
        // The instance must be reset to the default state with ResetVBAPObject() before it can be used for a new object.
        IABVBAP::vbapRendererObject     *vbapObject_;
        
        // Array of pointers to individual channels in the output buffer block.
        // When rendering an object definition, the renderer works on one subblock at a time and the pointers are updated
        // for each subblock to fill the correct segment of the output frame.
        // When rendering a bed channel as "object", no pointer update is required since the frame is rendered as a single block.
        IABSampleType                   **outputBufferPointers_;
        
        // Pointer to 32-bit integer object/bed channel audio sample buffer.
        // The buffer stores decoded DLC audio samples of the object/bed channel being rendered.
        int32_t                         *sampleBufferInt_;
        
        // Pointer to float object/bed channel audio buffer. Samples are converted from the integer buffer (sampleBufferInt_)
        // and stored in this float buffer which is used for applying VBAP gains.
        IABSampleType                   *sampleBufferFloat_;

		// *** Add support for object decorrelation in binary ON/OFF mode.
		//

		// IAB decorrelators. Object decorr at v1.1.
		IABDecorrelation *iabDecorrelation_;

		// Flag to indicate if iabDecorrelation_ is in reset state.
		bool decorrelationInReset_;

		// Flag to indicate presence of any decorr object in a frame.
		bool hasDecorrObjects_;

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
		// Note: Channel order is based on target configuration iConfig, with which the IABRenderer instance
		// is created.
		//
		IABSampleType *decorrOutputBuffer_;

		// Array of output buffer pointers. Each pointer points to the start of a rendered output channel
		// that is to be decorrelated.
		IABSampleType  **decorrOutputChannelPointers_;

		// VectDSP acceleration engine for summing up C output with D output
		CoreUtils::VectDSPInterface *vectDSP_;

		// *****************************************************************************
		// For internal testing

		// ================================================================
		// private members used for internal dev + tests
		bool enableFrameGainsCache_;
	};

} // namespace ImmersiveAudioBitstream
} // namespace SMPTE

#endif // __IABRENDERER_H__
