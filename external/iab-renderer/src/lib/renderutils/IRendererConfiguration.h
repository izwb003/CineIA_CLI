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

#ifndef __RENDERERCONFIGURATIONAPI__
#define __RENDERERCONFIGURATIONAPI__

#include <vector>
#include <stdint.h>
#include <map>

#include "renderutils/RenderSpeaker.h"
#include "RenderPatch.h"
#include "RendererConfigErrors.h"

namespace RenderUtils
{

    /**
     * Represents a Renderer Configuration data structure. Must be defined by implementations.
     * Used for configuring an IRenderer or determining how an IRenderer is configured.
     * Immutable.
     *
     * @class IRendererConfiguration
     */
    class IRendererConfiguration
    {
    public:
        virtual ~IRendererConfiguration() {}

		/**
		* Returns a pointer to a copy of renderer configuration object.
		* Client is resposible for managing and deleting the copy after use.
		*
		* @returns Pointer to a copy of renderer configuration object.
		*
		* @class IRendererConfiguration
		*/
		virtual IRendererConfiguration* newCopy() const = 0;

        /**
         * Gets decorrelated flag from the renderer configuration.
         *
         * @param[out] oDecorrelated decorrelated flag
         *
         * @returns \link kNoRendererConfigurationError \endlink if no errors. Other values indicate an error.
         *
         * @class IRendererConfiguration
         */
        virtual rendererConfigError GetDecorr(bool &oDecorrelated) const = 0;

		/**
		* Gets IAB decorrelation mode enable flag from the renderer configuration.
		*
		* @param[out] oIABDecorrEnable IAB decorrelation enable flag
		*
		* @returns \link kNoRendererConfigurationError \endlink if no errors. Other values indicate an error.
		*
		* @class IRendererConfiguration
		*/
		virtual rendererConfigError GetIABDecorrEnable(bool &oIABDecorrEnable) const = 0;

		/**
         * Gets smoothing flag from the renderer configuration.
         *
         * @param[out] oSmooth true indicates smoothing should be applied to the renderer output gains.
         * false means do not apply smoothing.
         *
         * @returns \link kNoRendererConfigurationError \endlink if no errors. Other values indicate an error.
         *
         * @class IRendererConfiguration
         */
        virtual rendererConfigError GetSmooth(bool &oSmooth) const = 0;

        /**
         * Gets target soundfield string from the renderer configuration.
         *
         * @param[out] oTargetSoundField reference to target soundfield string.
         *
         * @returns \link kNoRendererConfigurationError \endlink if no errors. Other values indicate an error.
         *
         * @class IRendererConfiguration
         */
        virtual rendererConfigError GetTargetSoundfield(std::string& oTargetSoundField) const = 0;

        /**
         * Gets the list of all speakers in the renderer configuration, including both virtual and physical speakers.
         * @sa RenderSpeaker
         *
         * @param[out] oSpeakers reference to the pointer to the RenderSpeakers vector.
         *
         * @returns \link kNoRendererConfigurationError \endlink if no errors. Other values indicate an error.
         *
         * @class IRendererConfiguration
         */
        virtual rendererConfigError GetSpeakers(const std::vector<RenderSpeaker>* &oSpeakers) const = 0;

        /**
         * Gets number of speakers in the configuration, including both virtual and physical speakers.
         *
         * @param[out] oTotalSpeakerCount number of speakers.
         *
         * @returns \link kNoRendererConfigurationError \endlink if no errors. Other values indicate an error.
         *
         * @class IRendererConfiguration
         */
        virtual rendererConfigError GetTotalSpeakerCount(uint32_t &oTotalSpeakerCount) const = 0;

        /**
         * Gets the render patches in the renderer configuration.
         * @sa RenderPatch
         *
         * @param[out] oPatches reference to the point of RenderPatch vector.
         *
         * @returns \link kNoRendererConfigurationError \endlink if no errors. Other values indicate an error.
         *
         * @class IRendererConfiguration
         */
        virtual rendererConfigError GetPatches(const std::vector<RenderPatch>* &oPatches) const = 0;

        /**
         * Checks if the configuration has a specific speaker.
         *
         * @param[in] iName name string of the speaker to check.
         * @param[out] oHasSpeaker true indicates that the configuration has the speaker, false means it does not have the speaker.
         *
         * @returns \link kNoRendererConfigurationError \endlink if no errors. Other values indicate an error.
         *
         * @class IRendererConfiguration
         */
        virtual rendererConfigError HasSpeaker(const std::string &iName, bool &oHasSpeaker) const = 0;

        /**
         * Gets index of the speaker using a name string. See description on GetChannelCount().
         *
         * @param[in] iName name string of the speaker to get.
         * @param[out] oIndex associated with speaker.
         *
         * @returns \link kNoRendererConfigurationError \endlink if no errors. A return value of kNoSuchSpeakerError
         * indicates that the name is not associated with a physical speaker.
         *
         * @class IRendererConfiguration
         */
        virtual rendererConfigError GetSpeakerIndexByName(const std::string &iName, int32_t &oIndex) const = 0;

        /**
         * Gets index of a speaker using a channel number. See description on GetChannelCount().
         *
         * @param[in] iChan channel number of the speaker to get.
         * @param[out] oIndex index associated with the speaker.
         *
         * @returns \link kNoRendererConfigurationError \endlink if no errors. A return value of kNoSuchSpeakerError
         * indicates that the channel number is not associated with a physical speaker.
         *
         * @class IRendererConfiguration
         */
        virtual rendererConfigError GetSpeakerIndexbyChannel(int32_t iChan, int32_t &oIndex) const = 0;

        /**
         * Gets index of a speaker using a URI. See description on GetChannelCount().
         *
         * @param[in] iURI reference to URI string of the speaker to get.
         * @param[out] oIndex index associated with the speaker.
         *
         * @returns \link kNoRendererConfigurationError \endlink if no errors. A return value of kNoSuchSpeakerError
         * indicates that the URI is not associated with a physical speaker.
         *
         * @class IRendererConfiguration
         */
        virtual rendererConfigError GetSpeakerIndexbyURI(const std::string &iURI, int32_t &oIndex) const = 0;

        /**
         * Gets index of the LFE speaker.
         * This index represents channel number ("chan") in the config file.
         *
         * @param[out] oIndex index associated with the LFE speaker.
         *
         * @returns \link kNoRendererConfigurationError \endlink if no errors. Other values indicate an error.
         *
         * @class IRendererConfiguration
         */
        virtual rendererConfigError GetLFEIndex(int32_t& oIndex) const = 0;

		/**
		* Gets a reference to the speaker URI to speaker index map.
		* This map contains all speakers with non-empty URI. Map key is the speaker URI string and map value is the
		* speaker index associated with the speakers in the list from GetSpeakers() call.
		*
		* @returns a constant reference to the map.
		*
		* @class IRendererConfiguration
		*/
		virtual const std::map<std::string, int32_t>& GetSpeakerURIToSpeakerIndexMap() const = 0;

		/**
         * Gets number of renderer output channels. This number is derived from the highest "chan" index in the configuration
         * file. "chan" represents index of the output channel associated with a physical speaker. It does not need to start
         * from 0 or need to appear sequentially and could have a value larger than the physical speaker count. To account for
         * these factors, legacy MDA VBAP renderer allocates a single block of output buffer using this channel count and
         * internally divides the block into output slots, one for each speaker. A client may use one of the
         * get speaker index APIs to obtain an index to access a particular output buffer slot, e.g. for writing samples to
         * output file. When chanel count is larger than number of physical speakers in the configuration, excess slots will
         * result and these slots become wasted resources. The application will also have to implement a matching buffer arrangement.
         * APIs that get speaker index should be used with implemetation that allocates buffer based on this channel count.
         *
         * Note that virtual speakers in the configuration file have "chan" set to "*" and the configuration file utilities
         * map this this to the speaker's ch_ = -1. All speakers with ch_ = -1 therefore are virtual speakers that have
         * outputIndex_ = -1.
         *
         * For applications that allocate output buffers for physical channel only, use GetPhysicalSpeakerCount() and
         * the APIs that get renderer output index instead.
         *
         * @param[out] oChannelCount number of channels.
         *
         * @returns \link kNoRendererConfigurationError \endlink if no errors. Other values indicate an error.
         *
         * @class IRendererConfiguration
         */
        virtual rendererConfigError GetChannelCount(uint32_t &oChannelCount) const = 0;

        /**
         * Finds if the renderer configuration has speakers in the bottom hemisphere.
         *
         * @param[out] oHasBottomHemisphere true if the configuration has speakers in bottom hemisphere.
         * False means no speakers in the bottom hemisphere.
         *
         * @returns \link kNoRendererConfigurationError \endlink if no errors. Other values indicate an error.
         *
         * @class IRendererConfiguration
         */
        virtual rendererConfigError HasBottomHemisphere(bool &oHasBottomHemisphere) const = 0;

        /**
         * Gets the list of physical speakers in the renderer configuration.
         * @sa RenderSpeaker
         *
         * @param[out] oSpeakers reference to the pointer to the RenderSpeakers vector.
         *
         * @returns \link kNoRendererConfigurationError \endlink if no errors. Other values indicate an error.
         *
         * @class IRendererConfiguration
         */
        virtual rendererConfigError GetPhysicalSpeakers(const std::vector<RenderSpeaker>* &oSpeakers) const = 0;

        /**
         * Gets number of physical speakers in the configuration and it is same as number of renderer output channels.
         *
         * @param[out] oPhysicalSpeakerCount number of physical.
         *
         * @returns \link kNoRendererConfigurationError \endlink if no errors. Other values indicate an error.
         *
         * @class IRendererConfiguration
         */
        virtual rendererConfigError GetPhysicalSpeakerCount(uint32_t &oPhysicalSpeakerCount) const = 0;

        /**
         * Gets renderer output index of a speaker using a name string. See renderer output buffer allocation and
         * indexing in GetChannelCount() API description.
         *
         * @param[in] iName name string of the speaker to get.
         * @param[out] oIndex renderer output index associated with speaker.
         *
         * @returns \link kNoRendererConfigurationError \endlink if no errors. A return value of kNoSuchSpeakerError
         * indicates that the name is not associated with a physical speaker.
         *
         * @class IRendererConfiguration
         */
        virtual rendererConfigError GetRendererOutputIndexByName(const std::string &iName, int32_t &oIndex) const = 0;

        /**
         * Gets renderer output index of a speaker using a channel number. See renderer output buffer allocation and
         * indexing in GetChannelCount() API description.
         *
         * @param[in] iChan channel number of the speaker to get.
         * @param[out] oIndex renderer output index associated with the speaker.
         *
         * @returns \link kNoRendererConfigurationError \endlink if no errors. A return value of kNoSuchSpeakerError
         * indicates that the channel number is not associated with a physical speaker.
         *
         * @class IRendererConfiguration
         */
        virtual rendererConfigError GetRendererOutputIndexbyChannel(int32_t iChan, int32_t &oIndex) const = 0;

        /**
         * Gets renderer output index of a speaker using a URI. See renderer output buffer allocation and
         * indexing in GetChannelCount() API description.
         *
         * @param[in] iURI reference to URI string of the speaker to get.
         * @param[out] oIndex renderer output index associated with the speaker.
         *
         * @returns \link kNoRendererConfigurationError \endlink if no errors. A return value of kNoSuchSpeakerError
         * indicates that the URI is not associated with a physical speaker.
         *
         * @class IRendererConfiguration
         */
        virtual rendererConfigError GetRendererOutputIndexbyURI(const std::string &iURI, int32_t &oIndex) const = 0;

        /**
         * Gets renderer output index of the LFE speaker. This index represents channel number ("chan") in the config file.
         * See renderer output buffer allocation and indexing in GetChannelCount() API description.
         *
         * @param[out] oIndex renderer output index associated with the LFE speaker.
         *
         * @returns \link kNoRendererConfigurationError \endlink if no errors. Other values indicate an error.
         *
         * @class IRendererConfiguration
         */
        virtual rendererConfigError GetLFERendererOutputIndex(int32_t& oIndex) const = 0;

        /**
         * Gets a reference to the speaker name to renderer output index map.
         * This map contains all physical speakers. Map key is the speaker name and map value is the
         * renderer output index associated with the speaker.
         *
         * @returns a constant reference to the map.
         *
         * @class IRendererConfiguration
         */
        virtual const std::map<std::string, int32_t>& GetSpeakerNameToOutputIndexMap() const = 0;

        /**
         * Gets a reference to the speaker URI to renderer output index map.
         * This map contains only speakers with URI. Map key is the speaker URI string and map value is the
         * renderer output index associated with the speaker.
         *
         * @returns a constant reference to the map.
         *
         * @class IRendererConfiguration
         */
        virtual const std::map<std::string, int32_t>& GetSpeakerURIToOutputIndexMap() const = 0;

        /**
         * Gets a reference to the speaker channel number to renderer output index map.
         * This map contains all physical speakers. Map key is the speaker channel number ("chan" in the config file)
         * and map value is the renderer output index associated with the speaker.
         *
         * @returns a constant reference to the map.
         *
         * @class IRendererConfiguration
         */
        virtual const std::map<int32_t, int32_t>& GetSpeakerChannelToOutputIndexMap() const = 0;
        
        /**
         * Checks if a speaker belongs to any VBAP RenderPatches.
         *
         * @param[in] iName name of the speaker to be checked.
         *
         * @returns true if speaker belongs to a VBAP RenderPatch in the renderer configuration, otherwise returns false.
         *
         * @class IRendererConfiguration
         */
        virtual bool IsVBAPSpeaker(const std::string &iName) const = 0;

    protected:

        IRendererConfiguration() {}
    };

    /**
     * Represents a Renderer Configuration File object. Must be defined by implementations.
     * Used for serializing a speaker configuration file.
     * A client can use the base class of IRendererConfiguration to access data
     * or configure an IRenderer.
     *
     * @class IRendererConfigurationFile
     */
    class IRendererConfigurationFile : public IRendererConfiguration
    {
    public:

        /**
         * Creates an IRendererConfigurationFile object from a
         * speaker configuration file.
         *
         * @memberof IRendererConfigurationFile
         *
         * @param[in] iTextConfig const char containing a speaker configuration file
         * @return \link kNoRendererConfigurationError \endlink if no errors occurred.
         */
        static IRendererConfigurationFile* FromBuffer(const char *iTextConfig);

        virtual ~IRendererConfigurationFile() {}

        /**
         * Returns the configuration file version
         *
         * @memberof IRendererConfigurationFile
         *
         * @param[out] oVersion int32_t with the version of the speaker configuration file.
         * @return \link kNoRendererConfigurationError \endlink if no errors occurred.
         */
        virtual rendererConfigError GetConfigVersion(int32_t &oVersion) const = 0;

    protected:

        IRendererConfigurationFile() {}

    private:
    };

} // namespace RenderUtils

#endif  // __RENDERERCONFIGURATIONAPI__
