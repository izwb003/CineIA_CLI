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
 * IABPackerAPI.h
 *
 */


#ifndef __IABPACKERAPI_H__
#define	__IABPACKERAPI_H__

#include <memory>

#include "IABDataTypes.h"

namespace SMPTE
{
namespace ImmersiveAudioBitstream
{
	/**
     * Represent an interface for an IAB Packer. Must be defined by implementation.
	 *
	 * IABPackerInterface contains APIs to help application developers in creating/modifying/updating
	 * an IABFrame and packing a completed frame to IAB bitstream. Applications to create
	 * or author IAB contents can use APIs contained here in combination with APIs offered in
	 * "IABElementsAPI.h" to construct an IABFrame with intended content and bed/object parameters
	 * and sub-element hierarchy.
	 *
	 * The following APIs are expected to be used for all applications.
	 *     Create() - factory method to create an IABPackerInterface instance
	 *     Delete() - factory method to delete an IABPackerInterface instance
	 *     SetFrameRate() - change/set frame rate for the IABFrame to user value
	 *     SetSampleRate() - change/set audio sample rate for the IABFrame to user value
	 *     GetIABFrame() - Get access to IABFrame for changing/updating/modifying per content
	 *     PackIABFrame() - Pack IABFrame, when complete and ready, to bitstream to internal stream buffer
	 *     GetPackedBuffer() - Get access to packed stream buffer, for writing out, for example
	 *
	 * The following APIs are offered as helper/utility/short-cut. However, the helper APIs cover
	 * only limited and constrained constructs and use scenarios. Due to the nature of infinite 
	 * diversity in content creation, the usefullness and scope of helper APIs is likely
	 * to be limited and therefore may not meet the specific needs of all clients, both in parameter
	 * values and constructed hierarchy. 
	 *     GetNextUniqueMetaID() - Returns a unique meta ID (only if caller does not create any meta IDs outside of using this API.)
	 *     GetNextUniqueAudioID() - Returns a unique audio ID (only if caller does not create any audio IDs outside of using this API.)
	 *     AddBedDefinition() - Add a bed definition element to IABFrame. (high customization implementation.)
	 *     AddObjectDefinition() - Add an object definition element to IABFrame. (high customization implementation.)
	 *     AddDLCElements() - Add a list of audio data elements to IABFrame. (high customization implementation.)
	 *     UpdateAudioSamples() - Update audio sample for a list of Audio Data elements.
	 *     UpdateObjectMetaData() - Update meta data of an object. (highly customized and limited meta data scope.)
	 *
	 * When/If helpers are not suitable, application developers can use GetIABFrame() to access IABframe, and make own 
	 * changes directly by using APIs from "IABElementsAPI.h", per content requirement.
	 *
     * @class IABPackerInterface
     */

    class IABPackerInterface
    {
    public:
        
		/**
		* Create an IABPacker instance
		*
		* @memberof IABPackerInterface
		*
		* @returns a pointer to IABPackerInterface instance created
		*/
		static IABPackerInterface* Create();

        /**
         * Delete an IABPacker instance
		 *
		 * @warning During deletion of IABPackerInterface (IABPacker) instance, the internal
		 * IABFrame instance is deleted. Along with the deletion of the IABFrame instance, all 
		 * the sub-element instances/objects, in and throughout the sub element hierarchy of 
		 * the IABFrame, are also deleted.
         *
         * @memberof IABPackerInterface
         *
         * @param[in] iInstance pointer to the instance of the IABPackerInterface
         */
        static void Delete(IABPackerInterface* iInstance);

		/// Destructor
        virtual ~IABPackerInterface() {}

        /**
         * Return the API version of the Packer.
         *
         * @memberof IABPackerInterface
         *
         * @param[out] oVersion IABAPIVersion with the API version of the Packer
         *
         */
        virtual void GetAPIVersion(IABAPIVersionType& oVersion) = 0;

		/**
		* Set the frame rate for IAB packer.
		*
		* Note - IABPackerInterface::SetFrameRate() API is designed te be used during initialization of
		* IABPacker instance.
		*
		* It is not recommeded to change the frame rate of an IABPacker instance in the middle of its 
		* life cycle, even for use cases such as repurposing. Instead, it is recommended that an IABPacker
		* instance is created for a single IAB stream/frame, and deleted after completion of stream/frame.
		*
		* @memberof IABPackerInterface
		*
		* @param[in] iFrameRate Frame rate to set.
		* @return \link iabError \endlink if no errors. Other values indicate an error.
		*/
		virtual iabError SetFrameRate(IABFrameRateType iFrameRate) = 0;

		/**
		* Set the audio sample rate for IAB packer.
		*
		* @memberof IABPackerInterface
		*
		* @param[in] iSampleRate Sample rate to set.
		* @return \link iabError \endlink if no errors. Other values indicate an error.
		*/
		virtual iabError SetSampleRate(IABSampleRateType iSampleRate) = 0;

		/**
		* Get the (pointer to) internal IABFrame.
		*
		* API for caller to get access to internal IABFrame for directly updating/modifying sub-elements
		* and their lower level sub-elements if present. Modification can be performed in-situ. Refer to
		* APIs contained in "IABElementsAPI.h" for upating/modifying IAB elements of various types.
		*
		* The API gives caller the ability to update/modify directly IABFrame before calling PackIABFrame()
		* for packing. It offers high flexibity that is necessary for caller to efficiently create
		* the IABFrame per specific content requirement. At the same time, it is caller's reposibility
		* to ensure that modified IABFrame conforms to SMPTE ST2098-2 specification.
		*
		* @memberof IABPackerInterface
		*
		* @param[out] oIABFrame Output pointer to the inetrnal IABFrame by reference.
		*
		* @returns \link kIABNoError \endlink if no errors. Other values indicate an error.
		*/
		virtual iabError GetIABFrame(IABFrameInterface* &oIABFrame) = 0;

		/**
         * Pack an IAB frame (class internal IABFrame) into a stream.
         *
         * @memberof IABPackerInterface
         *
         * @return \link iabError \endlink if no errors. Other values indicate an error.
         */
        virtual iabError PackIABFrame() = 0;

        /**
         * Get packed frame buffer. Client should pack the IAB frame with PackIABFrame() before calling this method.
         * Packed frame buffer is owned by the packer instance.
         *
         * @memberof IABPackerInterface
         *
         * @param[out] oProgramBuffer reference to the packed frame buffer.
         * @param[out] oProgramBufferLength reference to the length of packed frame buffer.
         *
         * @return \link iabError \endlink if no errors. Other values indicate an error.
         */
        virtual iabError GetPackedBuffer(std::vector<char>& oProgramBuffer, uint32_t& oProgramBufferLength) = 0;
        
		// ******* Below is a list of helper APIs that are highly customized.
		// ******* Retained for unit tests and possible client usage.
		// *******

		/**
		* Get next unique meta ID.
		*
		* A helper and utility API that returns a unique meta ID without duplication.
		* @note uniqueness and non-duplication is violated if caller creates any meta IDs
		* outside of this API.
		*
		* @returns a meta ID.
		*/
		virtual IABMetadataIDType GetNextUniqueMetaID() = 0;

		/**
		* Get next unique audio ID.
		*
		* A helper and utility API that returns a unique audio ID without duplication.
		* @note uniqueness and non-duplication is violated if caller creates any audio IDs
		* outside of this API.
		*
		* @returns an audio ID.
		*/
		virtual IABAudioDataIDType GetNextUniqueAudioID() = 0;

		/**
		* Create a BedDefinition element of specified bed layout, and add element to internal
		* IABFrame as a sub-element. Some basic set up is performed during this call. Due to its
		* customization, the instance created by this quick call may not meet all needs.
		*
		* @note This is a helper function with basic set up. Caller may want to further update/modify
		* parameters per intended bed specifications and content requirement.
		*
		* @note Channel composition is specified solely by iAudioIDs, a parameter of IABBedMappingInfo
		* type. Per ST2098-2 spec, iBedLayout and iAudioIDs do NOT have to correlate, in full or even 
		* in part.
		*
		* @note Users are encouraged to investigate/use APIs in "IABElementsAPI.h" directly, to create 
		* and customize BedDefinition instance, if instance created by this call does not meet purpose.
		*
		* @memberof IABPackerInterface
		*
		* @param[in] iBedID meta ID for the bed element.
		* @param[in] iBedLayout layout for the bed element.
		* @param[in] iAudioIDs Channel composition list and their mapping to corrsponding Audio IDs.
		* @param[out] oBedDefinition pointer to the created bed element.
		*
		* @return \link iabError \endlink if no errors. Other values indicate an error.
		*/
        virtual iabError AddBedDefinition(IABMetadataIDType iBedID, 
			IABUseCaseType iBedLayout, 
			const IABBedMappingInfo& iAudioIDs, 
			IABBedDefinitionInterface* &oBedDefinition) = 0;

		/**
		* Create a ObjectDefinition element, and add it to internal IABFrame as a sub-element. 
		* Some basic set up is performed during this call. Due to its customization, the instance 
		* created by this quick call may not meet all needs.
		*
		* @note This is a helper function with basic set up. Caller may want to further update/modify
		* parameters per intended object specifications and content requirement.
		*
		* @note Users are encouraged to investigate and use APIs in "IABElementsAPI.h" directly, to create
		* and customize ObjectDefinition instance, if instance created by this call does not meet purpose.
		*
		* @memberof IABPackerInterface
		*
		* @param[in] iObjectID meta ID for the object element.
		* @param[in] iBedLayout layout for the bed element.
		* @param[in] iAudioDataID corrsponding Audio ID.
		* @param[out] oObjectDefinition pointer reference to the created object element.
		*
		* @return \link iabError \endlink if no errors. Other values indicate an error.
		*/
		virtual iabError AddObjectDefinition(IABMetadataIDType iObjectID, 
			IABAudioDataIDType iAudioDataID, 
			IABObjectDefinitionInterface* &oObjectDefinition) = 0;

		/**
		* Update audio samples for a list of Audio data objects with Audio IDs
		*
		* @note This is a helper function with high customization. Client is not obliged to use it and 
		* may want to implement own code for updating audio samples from frame to frame.
		*
		* @memberof IABPackerInterface
		*
		* @param[in] iAudioSources a list of audio IDs with corresponding sample starter pointer for updating.
		*
		* @return \link iabError \endlink if no errors. Other values indicate an error.
		*/
		virtual iabError UpdateAudioSamples(std::map<IABAudioDataIDType, int32_t*> iAudioSources) = 0;
        
		/**
		* Update meta data of an object element.
		*
		* @note This is a helper function with high customization and narrow scope of types of parameters
		* that are updated. Client is not obliged to use it and may want to implement own code for 
		* object parameters per requirement, from frame to frame.
		*
		* @memberof IABPackerInterface
		*
		* @param[in] iObjectID meta ID for the object element who meta data is updated.
		* @param[in] iSubblockPanParameters struct - meta data to update to.
		*
		* @return \link iabError \endlink if no errors. Other values indicate an error.
		*/
        virtual iabError UpdateObjectMetaData(IABMetadataIDType iObjectID, std::vector<IABObjectPanningParameters> iSubblockPanParameters) = 0;
        
		/**
		* Create a list of IABAudioDataDLC elements and add them to internal IABFrame.
		*
		* @note This is a helper function with high customization and narrow scope of usage. Its
		* use is linked with other helper APIs calls including AddBedDefinition() and
		* AddObjectDefinition(). In general, for example, caller is likely to use the same set of 
		* audio IDs here as those in the AddBedDefinition() and AddObjectDefinition() calls.
		* Client is not obliged to use it and may want to implement own code that meet own
		* content generation requirement.
		*
		* @memberof IABPackerInterface
		*
		* @param[in] iAudioDataIDList List of audio IDs for which IABAudioDataDLC elements are 
		*            created and added to internal IABFrame.
		*
		* @return \link iabError \endlink if no errors. Other values indicate an error.
		*/
		virtual iabError AddDLCElements(std::vector<IABAudioDataIDType> iAudioDataIDList) = 0;
        
        /**
         * Create an AuthoringToolInfo element and add it to internal IABFrame as a sub-element.
         *
         * @note This helper function is expected to be called once during packer initialisation to create
         * and set up an AuthoringToolInfo element. Further calls will update the element content without
         * creating additional authoring tool elements.
         *
         * @memberof IABPackerInterface
         *
         * @param[in] iAuthoringToolInfoString string with authoring tool information. The packer only checks
         * that the string is not empty and does not validate its content. The caller is responsibile for
         * ensuring that the string is constructed correctly per 2098-2 specification.
         *
         * @return \link iabError \endlink if no errors. Other values indicate an error.
         */
        virtual iabError AddAuthoringToolInfo(const std::string &iAuthoringToolInfoString) = 0;
        
        /**
         * Create a UserData element and add it to internal IABFrame as a sub-element.
         *
         * @note 2098-2 allows multiple userData elements in the same bitstream, a new userData
         * element will therefore be added to the IABFrame every time this call is made.
         *
         * @memberof IABPackerInterface
         *
         * @param[in] iUserID SMPTE-Administered Universal Label.
         * matches the length of a SMPTE-Administered Universal Label and does not validate its content.
         * The caller is responsibile for ensuring that the string is constructed correctly per 2098-2 specification.
         * @param[in] iUserDataBytes user data bytes. The packer only checks that the vector size is not empty and
         * the caller is responsibile for ensuring that the data block is constructed correctly per 2098-2 specification.
         *
         * @return \link iabError \endlink if no errors. Other values indicate an error.
         */
        virtual iabError AddUserData(const uint8_t(&iUserID)[16], const std::vector<uint8_t> iUserDataBytes) = 0;

    };
    
} // namespace ImmersiveAudioBitstream
} // namespace SMPTE


#endif // __IABPACKERAPI_H__
