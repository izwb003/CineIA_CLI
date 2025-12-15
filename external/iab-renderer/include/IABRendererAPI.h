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
 * IABRendererAPI.h
 *
 */

#ifndef __IABRENDERERAPI_H__
#define	__IABRENDERERAPI_H__

#include <memory>

#include "IABDataTypes.h"
#include "renderutils/IRendererConfiguration.h"


namespace SMPTE
{
namespace ImmersiveAudioBitstream
{
    /**
     *
     * Generic IAB Renderer interface. Single threaded. Must be implemented.
     *
     * @class IABRendererInterface
     */
    
    class IABRendererInterface
    {
    public:

        /**
         * Creates an IABRenderer instance
         *
         * @memberof IABRendererInterface
         *
         * @param[in] iConfig pointer to an instance of RenderUtils::IRendererConfiguration
         *
         * @returns a pointer to IABRendererInterface instance created
         */
        static IABRendererInterface* Create(RenderUtils::IRendererConfiguration &iConfig);
        
        /**
         * Deletes an IABRenderer instance
         *
         * @memberof IABRendererInterface
         *
         * @param[in] iInstance pointer to the instance of the IABRendererInterface
         */
        static void Delete(IABRendererInterface* iInstance);
        
        /// Destructor
        virtual ~IABRendererInterface() {}
        
        /**
         * Returns the number of audio channels output by the renderer. Immutable across the lifetime
         * of the IABRendererInterface instance.
         *
         * @memberof IABRendererInterface
         *
         * @return Number of audio channels.
         */
        virtual IABRenderedOutputChannelCountType GetOutputChannelCount() const = 0;
        
        /**
         * Returns maximum number of audio samples per channel output by the renderer. Immutable across the lifetime
         * of the IABRendererInterface instance.
         *
         * @memberof IABRendererInterface
         *
         * @return Maximum number of audio samples per channel.
         */
        virtual IABRenderedOutputSampleCountType GetMaxOutputSampleCount() const = 0;
        
		 /**
		 * Renders an IAB frame (iIABFrame) into output channels (ioOutputChannels).
		 * The number of actual audio samples written into each of the audio channels held by ioOutputChannels
		 * is determined by the sample rate and frame rate of the iIABFrame, and returned in oRenderedOutputSampleCount.
		 * Memory backing ioOutputChannels is allocated and owned by the caller, and not used by the
		 * IABRendererInterface instance between calls to IABRendererInterface::RenderIABFrame().
		 *
		 * @memberof IABRendererInterface
		 *
		 * @param[in] iIABFrame IAB frame to be rendered.
		 * @param[in,out] ioOutputChannels Pointer to an array of iOutputChannelCount pointers, each corresponding
		 *            to a audio channel and each pointing to an array of iOutputSampleBufferCount audio samples.
		 * @param[in] iOutputChannelCount Number of output channels allocated. Must be greater than or equal to
		 *            GetOutputChannelCount().
		 * @param[in] iOutputSampleBufferCount Number of output samples allocated per channel. Must be greater than
		 *            or equal to GetMaxOutputSampleCount().
		 * @param[out] oRenderedOutputSampleCount Actual number of output samples rendered and written into each of
		 *            the audio channel arrays in oOutputChannels. Smaller than or equal to iOutputSampleBufferCount.
		 * @return \link iabKNoError \endlink if no errors occurred. Other return values indicate that an error has
		 *            occured, no valid rendered samples are returned, and the IABRendererInterface instance can
		 *            no longer be used.
		 */
		virtual iabError RenderIABFrame(const IABFrameInterface& iIABFrame
                                        , IABSampleType **ioOutputChannels
                                        , IABRenderedOutputChannelCountType iOutputChannelCount
                                        , IABRenderedOutputSampleCountType iOutputSampleBufferCount
                                        , IABRenderedOutputSampleCountType &oRenderedOutputSampleCount) = 0;
        
    };

#ifdef MT_RENDERER_ENABLED

	/**
	*
	* Multi-threaded IAB Renderer interface. Must be implemented.
	*
	* IABRendererMTInterface supports multi-threading processing internally.
	*
	* @class IABRendererMTInterface
	*/

	class IABRendererMTInterface
	{
	public:

		/**
		* Creates an IABRendererMT instance.
		* 
		* Note: Client can specify the number of persistent worker threads to create with iThreadPoolSize. The 
		* worker threads are responsible for decoding DLC, unpacking PCM, rendering objects and beds. Threads 
		* persist through the life time of an IABRendererMT instance.
		*
		* When calling RenderIABFrame(), the total number of threads is (1 + iThreadPoolSize), with the "1"
		* running at IAB fram level and sending jobs to job queue for iThreadPoolSize worker to process.
		*
		* The range of iThreadPoolSize is [1, 8]. It is, however, recommended to set iThreadPoolSize to
		* >= 2, and with a multi core CPU present, for faster IAB rendering. Setting iThreadPoolSize to "1",
		* though allowed, is generally not recommended. 
		*
		* @memberof IABRendererMTInterface
		*
		* @param[in] iConfig pointer to an instance of RenderUtils::IRendererConfiguration
		* @param[in] iThreadPoolSize number of persistent asset/object/bed processing threads to create
		*
		* @returns a pointer to IABRendererMTInterface instance created
		*/
		static IABRendererMTInterface* Create(RenderUtils::IRendererConfiguration &iConfig, uint32_t iThreadPoolSize);

		/**
		* Deletes an IABRendererMT instance
		*
		* @memberof IABRendererMTInterface
		*
		* @param[in] iInstance pointer to the instance of the IABRendererMTInterface
		*/
		static void Delete(IABRendererMTInterface* iInstance);

		/// Destructor
		virtual ~IABRendererMTInterface() {}

		/**
		* Returns the number of audio channels output by the MT renderer. Immutable across the lifetime
		* of the IABRendererMTInterface instance.
		*
		* @memberof IABRendererMTInterface
		*
		* @return Number of audio channels.
		*/
		virtual IABRenderedOutputChannelCountType GetOutputChannelCount() const = 0;

		/**
		* Returns maximum number of audio samples per channel output by the MT renderer. Immutable across
		* the lifetime of the IABRendererMTInterface instance.
		*
		* @memberof IABRendererMTInterface
		*
		* @return Maximum number of audio samples per channel.
		*/
		virtual IABRenderedOutputSampleCountType GetMaxOutputSampleCount() const = 0;

		/**
		* Returns number of cores in host PC.
		*
		* @memberof IABRendererMTInterface
		*
		* @return number of cores in host PC.
		*/
		virtual uint32_t GetProcessorCoreNumber() = 0;

		/**
		* Renders an IAB frame (iIABFrame) into output channels (ioOutputChannels).
		* The number of actual audio samples written into each of the audio channels held by ioOutputChannels
		* is determined by the sample rate and frame rate of the iIABFrame.
		* Memory backing ioOutputChannels is allocated and owned by the caller, and not used by the 
		* IABRendererMTInterface instance between calls to IABRendererMTInterface::RenderIABFrame().
		*
		* @memberof IABRendererMTInterface
		*
		* @param[in] iIABFrame IAB frame to be rendered.
		* @param[in,out] ioOutputChannels Pointer to an array of iOutputChannelCount pointers, each corresponding 
		*            to a audio channel and each pointing to an array of iOutputSampleBufferCount audio samples.
		* @param[in] iOutputChannelCount Number of output channels allocated. Must be greater than or equal to 
		*            GetOutputChannelCount().
		* @param[in] iOutputSampleBufferCount Number of output samples allocated per channel. Must be greater than 
		*            or equal to GetMaxOutputSampleCount().
		* @return \link iabKNoError \endlink if no errors occurred. Other return values indicate that an error has 
		*            occured, no valid rendered samples are returned, and the IABRendererMTInterface instance can 
		*            no longer be used.
		*/
		virtual iabError RenderIABFrame(const IABFrameInterface& iIABFrame
			, IABSampleType **ioOutputChannels
			, IABRenderedOutputChannelCountType iOutputChannelCount
			, IABRenderedOutputSampleCountType iOutputSampleBufferCount) = 0;

	};

#endif // #ifdef MT_RENDERER_ENABLED

} // namespace ImmersiveAudioBitstream
} // namespace SMPTE


#endif // __IABRENDERERAPI_H__
