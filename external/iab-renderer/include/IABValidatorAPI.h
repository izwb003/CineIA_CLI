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
 * IABValidatorAPI.h
 *
 */


#ifndef __IABVALIDATORAPI_H__
#define	__IABVALIDATORAPI_H__

#include <memory>
#include <vector>

#include "IABErrors.h"

namespace SMPTE
{
namespace ImmersiveAudioBitstream
{    
    /**
     *
     * Represent an interface for an IAB Validator. Must be defined by implementation.
     *
     * @class IABValidatorInterface
     */

    class IABValidatorInterface
    {
    public:
        
        /**
         * Creates an IABValidator instance.
         *
         * @memberof IABValidatorInterface
         *
		 * @returns a pointer to IABValidatorInterface instance created
         */
        static IABValidatorInterface* Create();
        
        /**
         * Deletes an IABValidator instance
         *
         * @memberof IABValidatorInterface
         *
         * @param[in] iInstance pointer to the instance of the IABValidatorInterface to be deleted
         */
        static void Delete(IABValidatorInterface* iInstance);

		/// Destructor
        virtual ~IABValidatorInterface() {}

        /**
         *
         * Validate an IAB frame.
         *
         * @memberof IABValidatorInterface
         *
         * @param[in] iIABFrame pointer to the IAFrame to be validated
         * @param[in] iFrameIndex index of the frame being validated.
         * @return \link kIABNoError \endlink if no errors. Other values indicate an error.
         */
        virtual iabError ValidateIABFrame(const IABFrameInterface *iIABFrame, uint32_t iFrameIndex) = 0;
        
        /** Gets the validation result for constraint set iConstraintSet.
         *
         * @memberof IABValidatorInterface
         *
		 * @param[in] iConstraintSet requested constraint set
		 * @return ValidationResult Result of validation against iConstraintSet, after processing IAB 
		 * frames passed in so far.
         */
		virtual const ValidationResult& GetValidationResult(SupportedConstraintsSet iConstraintSet) = 0;

		/** Gets a list of validation issues reported under the single set CS iConstraintSet.
		* The issue list combines those reported under the profile with dependency
		* hierarchical constraint set(s).
		*
		* @memberof IABValidatorInterface
		*
		* @param[in] iConstraintSet requested constraint set
		* @return std::vector<ValidationIssue> List of issues found during validation against iConstraintSet, 
		* after processing IAB frames passed in so far.
		*/
		virtual const std::vector<ValidationIssue>& GetValidationIssues(SupportedConstraintsSet iConstraintSet) = 0;
        
		/** Gets a list of validation issues reported under the single set CS iConstraintSet.
		* Unlike GetValidationIssues(), the issue list does not contain dependency constraint set(s).
		*
		* @memberof IABValidatorInterface
		*
		* @param[in] iConstraintSet requested constraint set
		* @return std::vector<ValidationIssue> List of issues found during validation against iConstraintSet 
		* only, after processing IAB frames passed in so far.
		*/
		virtual const std::vector<ValidationIssue>& GetValidationIssuesSingleSetOnly(SupportedConstraintsSet iConstraintSet) = 0;
	};

	/**
	* Interface for an event handler, which is called by IABEventHandler when encountering a validation event. Must be defined by implementation.
	*
	* @class IABEventHandler
	*/
	class IABEventHandler
	{
	public:

		virtual ~IABEventHandler() {}

		/**
		* Called by the validator every time an event is encountered.
		*
		* @memberof IABEventHandler
		*
		* @param[in] iIssue issue report by the validator
		*
		* @returns if false, the validator should stop processing the frame; otherwise it should continue processing the frame.
		*/
		virtual bool Handle(const ValidationIssue& iIssue) = 0;

	};

} // namespace ImmersiveAudioBitstream
} // namespace SMPTE


#endif // __IABVALIDATORAPI_H__
