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
 * Header file for common IAB errors.
 *
 * @file
 */


#ifndef __IABERRORS_H__
#define	__IABERRORS_H__

namespace SMPTE {
namespace ImmersiveAudioBitstream {

	/**
	*
	* Error Codes for IAB libraries.
	*
	* Error codes returned by IAB library API.
	*
	*/
	enum commonErrorCodes {
        
		kIABNoError                           = 0,              /**< No error. */
		kIABGeneralError                      = 1,              /**< Error: General error. */
		kIABBadArgumentsError                 = 2,              /**< Error: Bad arguments. */
		kIABAlreadyInitError                  = 3,              /**< Error: Instance already initialized. */
		kIABMemoryError                       = 4,              /**< Error: Memory allocation failed. */
		kIABNoSuchParameterError              = 5,              /**< Error: Invalid parameters. */
		kIABNotImplementedError               = 6,              /**< Error: API not implemented/supported. */
		kIABSystemIOError                     = 7,              /**< Error: Input/Output error in file/stream processing. */
        kIABDuplicateSubElementsError         = 8,              /**< Error: Duplicate sub-elements are in the list. */
        
        kIABDataFieldFrameHeaderChanged       = 100,            /**< Error found in data checking: fixed field in frame header changed. */
        kIABDataFieldInvalidFrameHeader       = 101,            /**< Error found in data checking: frame header. */
        kIABDataFieldFrameSubElementsNotConsistent = 102,       /**< Error found in data checking: inconsistent frame sub-elements. */
        kIABDataFieldInvalidBedDefinition     = 103,            /**< Error found in data checking: bed definition. */
        kIABDataFieldInvalidBedChannel        = 104,            /**< Error found in data checking: bed channel. */
        kIABDataFieldInvalidBedRemap          = 105,            /**< Error found in data checking: bed remap. */
        kIABDataFieldInvalidObjectDefinition  = 106,            /**< Error found in data checking: object definition. */
        kIABDataFieldInvalidObjectSubBlock    = 107,            /**< Error found in data checking: object sub-block. */
        kIABDataFieldInvalidZone19            = 108,            /**< Error found in data checking: object zone 19. */
        kIABDataFieldInvalidDLC               = 109,            /**< Error found in data checking: audio data DLC. */
        kIABDataFieldInvalidPCM               = 110,            /**< Error found in data checking: audio data PCM. */
        kIABDataFieldInvalidAuthoringToolInfo = 111,            /**< Error found in data checking: authoring tool info. */
        kIABDataFieldInvalidUserData          = 112,            /**< Error found in data checking: user data element. */
        kIABArgumentIncorrectDLCSampleCount   = 113,            /**< Error found in API call: Incorrect frame sample count for Audio DLC. */
		kIABArgumentIncorrectPCMSampleCount   = 114,            /**< Error found in API call: Incorrect frame sample count for Audio PCM. */

        kIABElementDLCMalConstructedError     = 200,            /**< Error found in AudioDataDLC element construction/instantiation. */

		kIABPackerGeneralError                = 1000,           /**< IAB packer general error. Fatal. Start of a block of errors used by IAB bitstream packer only. */
		kIABPackerObjectGainRangeError        = 1001,           /**< Packer error: Out-of-range gain value encountered during packing. Fatal. */
		kIABPackerObjectPositionRangeError    = 1002,           /**< Packer error: Out-of-range object position value encountered during packing. Fatal. */
		kIABPackerPackedStreamEmptyError      = 1003,           /**< Packer error: Packed stream has zero length. */
		kIABPackerObjectSpreadModeError       = 1004,           /**< Packer error: Out-of-range object spread mode value encountered during packing. Fatal. */
		kIABPackerDLCError                    = 1005,           /**< Packer error: error during DLC encoding. */
		kIABPackerPCMError                    = 1006,           /**< Packer error: error during PCM packing. */
        
        kIABParserGeneralError                = 2000,           /**< IAB parser general error. Fatal. Start of a block of errors used by IAB bitstream parser only. */
		kIABParserParsingError                = 2001,           /**< Parsing error: General error during parsing. Fatal. */
		kIABParserBitstreamReaderNotPresentError = 2002,        /**< Parsing error: bitstream reader not present. Fatal. */
		kIABParserEndOfStreamReached          = 2003,           /**< Parsing: End of stream reached. */
		kIABParserMissingPreambleError        = 2004,           /**< Error in parsing, preamble sub-frame is missing. Fatal. */
		kIABParserIASubFrameHeaderError       = 2005,           /**< Error in parsing IA sub-frame header. Fatal. */
		kIABParserIABElementHeaderError       = 2006,           /**< Error in parsing IAB element header data fields. Fatal. */
		kIABParserIABFrameError               = 2007,           /**< Error in parsing IAB Frame element data fields. Fatal. */
		kIABParserIABBedDefinitionError       = 2008,           /**< Error in parsing IAB BedDefinition element data fields. Fatal. */
		kIABParserIABChannelError             = 2009,           /**< Error in parsing IAB channel data fields. Fatal. */
		kIABParserIABObjectDefinitionError    = 2010,           /**< Error in parsing IAB ObjectDefinition element data fields. Fatal. */
		kIABParserDLCDecodingError            = 2011,           /**< Parsing error: Unable to get audio samples from DLC decoder Fatal. */
		kIABParserIABObjectSubBlockError      = 2012,           /**< Error in parsing IAB object sub block data fields. Fatal. */
		kIABParserIABBedRemapSubBlockError    = 2013,           /**< Error in parsing IAB bed remap sub block data fields. Fatal. */
		kIABParserIABBedRemapError            = 2014,           /**< Error in parsing IAB bed remap data fields. Fatal. */
		kIABParserIABZone19SubBlockError      = 2015,           /**< Error in parsing IAB zone19 sub block data fields. Fatal. */
		kIABParserIABObjectZone19Error        = 2016,           /**< Error in parsing IAB IABObjectZoneDefinition19 data fields. Fatal. */
		kIABParserIABAuthoringToolInfoError   = 2017,           /**< Error in parsing IAB Authoring Tool Info data fields. Fatal. */
		kIABParserIABUserDataError            = 2018,           /**< Error in parsing IAB user data element data fields. Fatal. */
        kIABParserInvalidFrameSubElementError = 2019,           /**< Parsing error: Found an invalid frame sub-element type. Fatal. */
        kIABParserSkipElementError            = 2020,           /**< Error when skip parsing an IAB element. Fatal. */
        kIABParserIABDLCError                 = 2021,           /**< Error in parsing IAB DLC element data fields. Fatal. */
        kIABParserIABPCMError                 = 2022,           /**< Error in parsing IAB PCM element data fields. Fatal. */
        kIABParserNoParsedFrameError          = 2023,           /**< Parsing error: No parsed frame to get data from. Fatal. */
        kIABParserNoAuthoringToolInfoError    = 2024,           /**< No IAB Authoring Tool Info in parsed IAB frame. Fatal. */
		kIABParserPCMUnpackingError           = 2025,           /**< Error during PCM sample unpacking. */
        kIABParserInvalidVersionNumberError   = 2026,           /**< Parsing error: Found an invalid version number. Fatal. */
        kIABParserInvalidSampleRateError      = 2027,           /**< Parsing error: Found an invalid sample rate. Fatal. */
        kIABParserInvalidFrameRateError       = 2028,           /**< Parsing error: Found an invalid frame rate. Fatal. */
        kIABParserInvalidBitDepthError        = 2029,           /**< Parsing error: Found an invalid bit depth. Fatal. */
        kIABParserFrameRateNotImplementedError = 2030,          /**< Parsing error: Support for this frame rate is not implmented. Fatal. */
        
		kIABRendererGeneralError              = 3000,           /**< IAB renderer general error. Fatal. Start of a block of errors used by IAB bitstream renderer only. */
        kIABRendererNotInitialisedError       = 3001,           /**< IAB renderer error. Fatal. IAB Renderer has not been fully initialised. */
        kIABRendererNoSoundfieldInConfig      = 3002,           /**< IAB renderer error. Unable to determine active bed use case since soundfield is missing in the config file. */
		kIABRendererUnsupportedSampleRateError= 3003,           /**< IAB renderer error. Fatal. Sample rate not supported. */
        kIABRendererInvalidRadiusError        = 3004,           /**< IAB renderer error. Fatal. IAB Object radius should not be less than 0. */
		kIABRendererVBAPRenderingError        = 3005,           /**< IAB renderer error signalling error condition from VBAP processing. Fatal. */
		kIABRendererApplyChannelGainsError    = 3006,           /**< IAB renderer error signalling error condition from ChannelGainsProcessor processing. Fatal. */
        kIABRendererObjectDefinitionError     = 3007,           /**< IAB renderer error. IAB renderer has encountered an error while attempting to render an object definition */
        kIABRendererBedDefinitionError        = 3008,           /**< IAB renderer error. IAB renderer has encountered an error while attempting to render a bed definition */
        kIABRendererBedChannelError           = 3009,           /**< IAB renderer error. IAB renderer has encountered an error while attempting to render a bed channel */
        kIABRendererCoordConversionError      = 3010,           /**< IAB renderer error. IAB renderer has encountered an error while attempting to convert VBAP coordinate to IAB coordiante */
        kIABRendererZone9Error                = 3011,           /**< IAB renderer error. IAB renderer has encountered an error while attempting to render an object zone9 */
		kIABRendererBedRemapError             = 3012,           /**< IAB renderer error. IAB renderer has encountered an error while attempting to render a bed remap */
		kIABRendererUnsupportedFrameRateError = 3013,           /**< IAB renderer error. Fatal. Frame rate not supported. */
        kIABRendererSampleCountMismatchError  = 3014,           /**< IAB renderer error. Fatal. IABAudioDataPCM sample count different from IAB frame sample count. */
		kIABRendererNoLFEInConfigForBedLFEWarning = 3015,       /**< IAB renderer warning. Bed LFE not rendered due to target config not having LFE */
		kIABRendererNoLFEInConfigForRemapLFEWarning = 3016,     /**< IAB renderer warning. Remapped LFE not rendered due to target config not having LFE */
		kIABRendererDownmixChannelError       = 3017,           /**< IAB renderer error. Fatal. Downmix channel not found. An error in configuration file. */
		kIABRendererAssetNumberExceedsMax     = 3018,           /**< IAB renderer warning. IAB frame contains more than 128 asset element, a limit used by IABRenderer. */
		kIABRendererEmptyObjectZoneWarning    = 3100,           /**< IAB renderer warning. Renderer configuration does not have speaker in one or more zones. */

		kIABValidatorTerminationError         = 4000,           /**< IAB validator lib termination error. Fatal. Client to terminate the active validation processing when encountering the error. */

		// Error codes in [10000 11999] range used by IAB Validator
		// [10000 10999]: error range
		kValidateGeneralError                 = 10000,          /**< IAB validation general error. */
		kValidateErrorIAFrameIllegalBitstreamVersion,           /**< IAB validation error. IAFrame bitstream version illegal. */
		kValidateErrorIAFrameUnsupportedSampleRate,             /**< IAB validation error. Unsupported IAFrame Sample Rate. */
		kValidateErrorIAFrameUnsupportedBitDepth,               /**< IAB validation error. Unsupported IAFrame BitDepth. */
		kValidateErrorIAFrameUnsupportedFrameRate,              /**< IAB validation error. Unsupported IAFrame FrameRate. */
		kValidateErrorIAFrameMaxRenderedExceeded,               /**< IAB validation error. MaxRendered of IAFrame exceeding limit. */
		kValidateErrorIAFrameSubElementCountConflict,           /**< IAB validation error. IAFrame sub-element count not agreeing with sub-element list size. */
		kValidateErrorIAFrameBitstreamVersionNotPersistent,     /**< IAB validation error. IAFrame bitstream version changes over program frames. */
		kValidateErrorIAFrameSampleRateNotPersistent,           /**< IAB validation error. IAFrame Sample Rate changes over program frames. */
		kValidateErrorIAFrameBitDepthNotPersistent,             /**< IAB validation error. IAFrame BitDepth changes over program frames. */

        // 10010
		kValidateErrorIAFrameFrameRateNotPersistent,            /**< IAB validation error. IAFrame FrameRate changes over program frames. */
        kValidateErrorIAFrameUndefinedElementType,              /**< IAB validation error. IAFrame containing undefined sub-element type. */
        kValidateErrorIAFrameSizeLimitExceeded,                 /**< IAB validation error. IAframe element size exceeds limit for current frame rate. */
		kValidateErrorBedDefinitionDuplicateMetaID,             /**< IAB validation error. BedDefinition MetaID Duplication. */
		kValidateErrorBedDefinitionMultiActiveSubElements,      /**< IAB validation error. BedDefinition containing mutiple simultaneous active sub-elements. */
		kValidateErrorBedDefinitionHierarchyLevelExceeded,      /**< IAB validation error. BedDefinition sub-element hierarchy level of > 1. */
		kValidateErrorBedDefinitionChannelCountConflict,        /**< IAB validation error. BedDefinition channel count not agreeing with channel list size. */
		kValidateErrorBedDefinitionDuplicateChannelID,          /**< IAB validation error. BedDefinition containing duplicated channelID code . */
		kValidateErrorBedDefinitionUnsupportedGainPrefix,       /**< IAB validation error. BedDefinition containing an unsupported ChannelGainPrefix code. */
		kValidateErrorBedDefinitionUnsupportedDecorPrefix,      /**< IAB validation error. BedDefinition containing an unsupported ChannelDecorCoefPrefix code. */

        // 10020
        kValidateErrorBedDefinitionAudioDescriptionTextExceeded,/**< IAB validation error. BedDefinition containing an AudioDescriptionText field whose size exceeds limit. */
		kValidateErrorBedDefinitionSubElementCountConflict,     /**< IAB validation error. BedDefinition sub-element count not agreeing with sub-element list size. */
        kValidateErrorBedDefinitionInvalidChannelID,            /**< IAB validation error. BedDefinition containing an invalid channelID code. */
        kValidateErrorBedDefinitionInvalidUseCase,              /**< IAB validation error. BedDefinition containing an invalid useCase code. */
        kValidateErrorBedDefinitionSubElementsNotAllowed,       /**< IAB validation error. BedDefinition must not contain sub-elements. */
        kValidateErrorBedDefinitionCountNotPersistent,          /**< IAB validation error. BedDefinition count changes over program frames. */
        kValidateErrorBedDefinitionChannelCountNotPersistent,   /**< IAB validation error. BedDefinition channel count changes over program frames. */
        kValidateErrorBedDefinitionMetaIDNotPersistent,         /**< IAB validation error. BedDefinition meta ID changes over program frames. */
        kValidateErrorBedDefinitionChannelIDsNotPersistent,     /**< IAB validation error. BedDefinition channel IDs change over program frames. */
        kValidateErrorBedDefinitionConditionalStateNotPersistent, /**< IAB validation error. BedDefinition conditional flag or use case changes over program frames. */

        // 10030
		kValidateErrorBedRemapDuplicateMetaID,                  /**< IAB validation error. BedRemap MetaID Duplication. */
		kValidateErrorBedRemapSourceChannelCountNotEqualToBed,  /**< IAB validation error. BedRemap containing source channel count that is not equal to ChannelCount of parent bed. */
		kValidateErrorBedRemapSubblockCountConflict,            /**< IAB validation error. BedRemap remap sub-block count not agreeing with sub-block list size. */
		kValidateErrorBedRemapSourceChannelCountConflict,       /**< IAB validation error. BedRemap containing source channel count that is not equal to input size of remap coefficient array. */
		kValidateErrorBedRemapDestinationChannelCountConflict,  /**< IAB validation error. BedRemap containing destination channel count that is not equal to output size of remap coefficient array. */
        kValidateErrorBedRemapInvalidDestChannelID,             /**< IAB validation error. BedRemap containing an invalid destination channelID code. */
        kValidateErrorBedRemapInvalidUseCase,                   /**< IAB validation error. BedRemap containing an invalid useCase code. */
        kValidateErrorBedRemapNotAnAllowedSubElement,           /**< IAB validation error. BedRemap is not allowed element type. */
		kValidateErrorObjectDefinitionDuplicateMetaID,          /**< IAB validation error. ObjectDefinition MetaID Duplication. */
        kValidateErrorObjectDefinitionMultiActiveSubElements,   /**< IAB validation error. ObjectDefinition containing mutiple simultaneous active sub-elements. */

        // 10040
        kValidateErrorObjectDefinitionHierarchyLevelExceeded,   /**< IAB validation error. ObjectDefinition sub-element hierarchy level > 1. */
		kValidateErrorObjectDefinitionPanSubblockCountConflict, /**< IAB validation error. ObjectDefinition pan sub-block count not agreeing with sub-block list size. */
		kValidateErrorObjectDefinitionUnsupportedGainPrefix,    /**< IAB validation error. ObjectDefinition containing an unsupported ObjectGainPrefix code. */
		kValidateErrorObjectDefinitionUnsupportedZoneGainPrefix,/**< IAB validation error. ObjectDefinition containing an unsupported ZoneGainPrefix code. */
		kValidateErrorObjectDefinitionUnsupportedSpreadMode,    /**< IAB validation error. ObjectDefinition containing an unsupported Object spread mode code. */
		kValidateErrorObjectDefinitionUnsupportedDecorPrefix,   /**< IAB validation error. ObjectDefinition containing an unsupported ObjectDecorCoefPrefix code. */
		kValidateErrorObjectDefinitionAudioDescriptionTextExceeded,/**< IAB validation error. ObjectDefinition containing an AudioDescriptionText field whose size exceeds limit. */
        kValidateErrorObjectDefinitionSubElementCountConflict,  /**< IAB validation error. ObjectDefinition sub-element count not agreeing with sub-element list size. */
        kValidateErrorObjectDefinitionInvalidUseCase,           /**< IAB validation error. ObjectDefinition containing an invalid useCase code. */
        kValidateErrorObjectDefinitionInvalidSubElementType,    /**< IAB validation error. ObjectDefinition sub-element type is not valid. */

        // 10050
        kValidateErrorObjectDefinitionConditionalStateNotPersistent, /**< IAB validation error. ObjectDefinition conditional flag or use case changes over program frames. */
		kValidateErrorObjectZoneDefinition19SubblockCountConflict, /**< IAB validation error. ObjectZoneDefinition19 sub-block count not agreeing with sub-block list size. */
		kValidateErrorObjectZoneDefinition19UnsupportedZoneGainPrefix, /**< IAB validation error. ObjectZoneDefinition19 containing an unsupported ZoneGainPrefix code. */
		kValidateErrorAudioDataDLCAudioDataIDZero,              /**< IAB validation error. AudioDataDLC AudioDataID == 0. */
		kValidateErrorAudioDataDLCDuplicateAudioDataID,         /**< IAB validation error. AudioDataDLC AudioDataID Duplication. */
		kValidateErrorAudioDataDLCUnsupportedSampleRate,        /**< IAB validation error. AudioDataDLC contain unsupported sample rate. */
		kValidateErrorAudioDataDLCSampleRateConflict,           /**< IAB validation error. AudioDataDLC DLCSampleRate not agreeing with IAFrame.SampleRate. */
        kValidateErrorAudioDataDLCNotAnAllowedSubElement,       /**< IAB validation error. AudioDataDLC is not an allowed element type. */
		kValidateErrorAudioDataPCMAudioDataIDZero,              /**< IAB validation error. AudioDataPCM AudioDataID == 0. */
		kValidateErrorAudioDataPCMDuplicateAudioDataID,         /**< IAB validation error. AudioDataPCM AudioDataID Duplication. */

        // 10060
        kValidateErrorAudioDataPCMNotAnAllowedSubElement,       /**< IAB validation error. AudioDataPCM is not allowed element type  for the constraint set being validated. */
		kValidateErrorMissingAudioDataEssenceElement,           /**< IAB validation error. An referred Audio Data essence element (PCM or DLC) is missing from frame. */
        kValidateErrorUserDataNotAnAllowedSubElement,           /**< IAB validation error. UserData is not allowed element type. */
        kValidateErrorDLCUsedWithIncompatibleFrameRate,         /**< IAB validation error. Fractional frame rate incompatible with DLC coding. */

        // DbyCinema Specific error codes
		//
        kValidateErrorDolCinIAFrameUnsupportedSampleRate = 10100,     /**< IAB validation error. Unsupported IAFrame SampleRate for DbyCinema constraint Set. */
        kValidateErrorDolCinBedDefinitionSubElementsNotAllowed,       /**< IAB validation error. BedDefinition must not contain sub-elements for DbyCinema constraint Set. */
        kValidateErrorDolCinBedDefinitionInvalidChannelID,            /**< IAB validation error. BedDefinition containing an invalid channelID code for DbyCinema constraint Set. */
        kValidateErrorDolCinBedDefinitionInvalidUseCase,              /**< IAB validation error. BedDefinition containing an invalid useCase code for DbyCinema constraint Set. */
        kValidateErrorDolCinBedDefinitionMultipleBedsNotAllowed,      /**< IAB validation error. More than one BedDefinition found but only one is allowed for DbyCinema constraint Set. */
        kValidateErrorDolCinBedDefinitionInvalidGainPrefix,           /**< IAB validation error. BedDefinition containing an invalid ChannelGainPrefix code for DbyCinema constraint Set. */
        kValidateErrorDolCinBedDefinitionChannelDecorInfoExistNotZero, /**< IAB validation error. BedDefinition channel decor exists not = 0 for DbyCinema constraint Set. */
        kValidateErrorDolCinBedDefinitionMaxChannelCountExceeded,     /**< IAB validation error. BedDefinition channel count limit exceeded for DbyCinema constraint Set. */
        kValidateErrorDolCinBedDefinitionCountNotPersistent,          /**< IAB validation error. BedDefinition count changes over program frames  for DbyCinema constraint Set. */
        kValidateErrorDolCinBedDefinitionMetaIDNotPersistent,         /**< IAB validation error. BedDefinition meta ID changes over program frames for DbyCinema constraint Set. */

        // 10110
        kValidateErrorDolCinBedDefinitionChannelListNotPersistent,    /**< IAB validation error. BedDefinition channel list changes over program frames for DbyCinema constraint Set. */
        kValidateErrorDolCinBedRemapUnsupportedGainPrefix,            /**< IAB validation error. BedRemap containing an unsupported RemapGainPrefix code for DbyCinema constraint Set. */
        kValidateErrorDolCinBedRemapNotAnAllowedSubElement,           /**< IAB validation error. BedRemap is not allowed sub-element type in the current element for DbyCinema constraint Set. */
        kValidateErrorDolCinObjectDefinitionSubElementsNotAllowed,    /**< IAB validation error. ObjectDefinition must not contain sub-elements for DbyCinema constraint Set. */
        kValidateErrorDolCinObjectDefinitionInvalidUseCase,           /**< IAB validation error. ObjectDefinition containing an invalid useCase code for DbyCinema constraint Set. */
        kValidateErrorDolCinObjectDefinitionInvalidGainPrefix,        /**< IAB validation error. ObjectDefinition containing an invalid gainPrefix code for DbyCinema constraint Set. */
        kValidateErrorDolCinObjectDefinitionInvalidZoneGainPrefix,    /**< IAB validation error. ObjectDefinition containing an invalid zoneGainPrefix code for DbyCinema constraint Set. */
        kValidateErrorDolCinObjectDefinitionInvalidSpreadMode,        /**< IAB validation error. ObjectDefinition containing an invalid Object spreadMode code for DbyCinema constraint Set. */
        kValidateErrorDolCinObjectDefinitionInvalidDecorPrefix,       /**< IAB validation error. ObjectDefinition containing an invalid Object decorCoefPrefix code for DbyCinema constraint Set. */
        kValidateErrorDolCinObjectDefinitionSnapTolExistsNotZero,     /**< IAB validation error. ObjectDefinition snapTolExists not zero for DbyCinema constraint Set. */

        // 10120
        kValidateErrorDolCinObjectDefinitionMaxObjectCountExceeded,   /**< IAB validation error. ObjectDefinition object count limit exceeded for DbyCinema constraint Set. */
        kValidateErrorDolCinObjectDefinitionNonSequenctialMetaID,     /**< IAB validation error. ObjectDefinition meta ID not sequential for DbyCinema constraint Set. */
        kValidateErrorDolCinObjectZoneDefinition19NotAnAllowedSubElement,  /**< IAB validation error. ObjectZoneDefinition19 is not allowed element type in the current element for DbyCinema constraint Set. */
        kValidateErrorDolCinAuthoringToolInfoNotAnAllowedSubElement,  /**< IAB validation error. AuthoringToolInfo is not allowed element type for DbyCinema constraint Set. */
        kValidateErrorDolCinObjectDefinitionZoneGainsNotAPreset,      /**< IAB validation error. ObjectDefinition zone gain not a preset for DbyCinema constraint Set. */

        kValidateErrorDolIMFBedDefinitionInvalidChannelID,            /**< IAB validation error. BedDefinition containing an invalid channelID code for DbyIMF constraint Set. */
        kValidateErrorDolIMFBedDefinitionInvalidGainPrefix,           /**< IAB validation error. BedDefinition containing an invalid ChannelGainPrefix code for DbyIMF constraint Set. */
        kValidateErrorDolIMFBedDefinitionChannelDecorInfoExistNotZero, /**< IAB validation error. BedDefinition channel decor exists not = 0 for DbyIMF constraint Set. */
        kValidateErrorDolIMFObjectDefinitionInvalidGainPrefix,        /**< IAB validation error. ObjectDefinition containing an invalid gainPrefix code for DbyIMF constraint Set. */
        kValidateErrorDolIMFObjectDefinitionInvalidZoneGainPrefix,    /**< IAB validation error. ObjectDefinition containing an invalid zoneGainPrefix code for DbyIMF constraint Set. */

        // 10130
        kValidateErrorDolIMFObjectDefinitionInvalidSpreadMode,        /**< IAB validation error. ObjectDefinition containing an invalid Object spreadMode code for DbyIMF constraint Set. */
        kValidateErrorDolIMFObjectDefinitionInvalidDecorPrefix,       /**< IAB validation error. ObjectDefinition containing an invalid Object decorCoefPrefix code for DbyIMF constraint Set. */
        kValidateErrorDolIMFObjectDefinitionSnapTolExistsNotZero,     /**< IAB validation error. ObjectDefinition snapTolExists not zero for DbyIMF constraint Set. */

		// [11000 11999]: warning range
		kValidateGeneralWarning               = 11000,                /**< IAB validation general warning. */
		kValidateWarningFrameContainFrame,                            /**< IAB validation warning. IAFrame containing another IAFrame element as a sub-element. */
		kValidateWarningFrameContainBedRemap,                         /**< IAB validation warning. IAFrame containing a BedRemap element as a direct sub-element. */
		kValidateWarningFrameContainObjectZoneDefinition19,           /**< IAB validation warning. IAFrame containing an ObjectZoneDefinition19 as a direct sub-element. */
		kValidateWarningFrameContainUndefinedSubElement,              /**< IAB validation warning. IAFrame containing an undefined element as a sub-element. */
		// MaxRendered check now lowered from error to warning.
		kValidateWarningIAFrameMaxRenderedNotMatchObjectNumbers,      /**< IAB validation warning. MaxRendered of IAFrame does not agree with channel/object numbers. */
		kValidateWarningAuthoringToolInfoMultipleElements,            /**< IAB validation warning. IAFrame containing more than 1 AuthoringToolInfo element. */
		kValidateWarningBedDefinitionUndefinedUseCase,                /**< IAB validation warning. BedDefinition containing undefined or reserved usecase code. */
		kValidateWarningBedDefinitionUndefinedChannelID,              /**< IAB validation warning. BedDefinition containing undefined or reserved channelID code. */
		kValidateWarningBedDefinitionUndefinedAudioDescription,       /**< IAB validation warning. BedDefinition containing undefined or reserved AudioDescription code. */

        // 11010
		kValidateWarningBedDefinitionContainUnsupportedSubElement,    /**< IAB validation warning. BedDefinition containing an unsupported element as a sub-element. */
        kValidateWarningBedDefinitionAlwaysActiveSubElement,          /**< IAB validation error. BedDefinition containing a non-conditional BedDefinition sub-element. */
		kValidateWarningBedRemapUndefinedUseCase,                     /**< IAB validation warning. BedRemap containing undefined or reserved usecase code. */
		kValidateWarningBedRemapUndefinedChannelID,                   /**< IAB validation warning. BedRemap containing undefined or reserved channelID code for destination. */
		kValidateWarningObjectDefinitionUndefinedUseCase,             /**< IAB validation warning. ObjectDefinition containing undefined or reserved usecase code. */
		kValidateWarningObjectDefinitionUndefinedAudioDescription,    /**< IAB validation warning. ObjectDefinition containing undefined or reserved AudioDescription code. */
		kValidateWarningObjectDefinitionMultipleZone19SubElements,    /**< IAB validation warning. ObjectDefinition containing multiple ObjectZoneDefinition19 sub-elements. */
		kValidateWarningObjectDefinitionContainUnsupportedSubElement, /**< IAB validation warning. ObjectDefinition containing an unsupported element as a sub-element. */
		kValidateWarningObjectDefinitionAlwaysActiveSubElement,       /**< IAB validation warning. ObjectDefinition containing a non-conditional ObjectDefinition sub-element. */
		kValidateWarningUnreferencedAudioDataDLCElement,              /**< IAB validation warning. AudioDataDLC element not referenced/used by any of bed channels or objects. */
        
        // 11020
		kValidateWarningUnreferencedAudioDataPCMElement,              /**< IAB validation warning. AudioDataPCM element not referenced/used by any of bed channels or objects. */

        // profile-specific warning codes
        kValidateWarningDolIMFObjectDefinitionZoneGainsNotAPreset = 11100, /**< IAB validation warning. ObjectDefinition zone gain not a preset for DbyIMF constraint Set. */
        kValidateWarningDolIMFNotMeetingContinuousAudioSequence,      /**< IAB validation warning. Order of frame bed, object and linked PCM elements not meeting sequence requirement by DbyIMF. */
        kValidateWarningDolCinSoleBedNotComplete91OH,                 /**< IAB validation warning. The only bed in frame is not a complete 9.1OH. (warning from real-world testing, for DbyCinema.) */
        kValidateWarningDolIMFSoleBedNotComplete91OH,                 /**< IAB validation warning. The only bed in frame is not a complete 9.1OH. (warning from real-world testing, for DbyIMF.) */
    };

}
}

#endif // __IABERRORS_H__
