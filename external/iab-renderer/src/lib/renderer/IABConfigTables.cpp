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
* IABConfigTables.cpp
*
* @file
*/

#include "IABConfigTables.h"

namespace SMPTE
{
	namespace ImmersiveAudioBitstream
	{
		/****************************************************************************
		* IABConfigTables
		*****************************************************************************/

		// IABConfigTables::CreateBedChannelInfoMap() implementation
		IABBedChannelInfoMap IABConfigTables::CreateBedChannelInfoMap()
		{
			IABBedChannelInfoMap mapTable;

			mapTable[kIABChannelID_Left]              = IABRendererBedChannelInfo(speakerURILeft, vbapSpeakerCoordLeft);
			mapTable[kIABChannelID_LeftCenter]        = IABRendererBedChannelInfo(speakerURILeftCenter, vbapSpeakerCoordLeftCenter);
			mapTable[kIABChannelID_Center]            = IABRendererBedChannelInfo(speakerURICenter, vbapSpeakerCoordCenter);
			mapTable[kIABChannelID_RightCenter]       = IABRendererBedChannelInfo(speakerURIRightCenter, vbapSpeakerCoordRightCenter);
			mapTable[kIABChannelID_Right]             = IABRendererBedChannelInfo(speakerURIRight, vbapSpeakerCoordRight);
			mapTable[kIABChannelID_LeftSideSurround]  = IABRendererBedChannelInfo(speakerURILeftSideSurround, vbapSpeakerCoordLeftSideSurround);
			mapTable[kIABChannelID_LeftSurround]      = IABRendererBedChannelInfo(speakerURILeftSurround, vbapSpeakerCoordLeftSurround);
			mapTable[kIABChannelID_LeftRearSurround]  = IABRendererBedChannelInfo(speakerURILeftRearSurround, vbapSpeakerCoordLeftRearSurround);
			mapTable[kIABChannelID_RightRearSurround] = IABRendererBedChannelInfo(speakerURIRightRearSurround, vbapSpeakerCoordRightRearSurround);
			mapTable[kIABChannelID_RightSideSurround] = IABRendererBedChannelInfo(speakerURIRightSideSurround, vbapSpeakerCoordRightSideSurround);
			mapTable[kIABChannelID_RightSurround]     = IABRendererBedChannelInfo(speakerURIRightSurround, vbapSpeakerCoordRightSurround);
			mapTable[kIABChannelID_LeftTopSurround]   = IABRendererBedChannelInfo(speakerURILeftTopSurround, vbapSpeakerCoordLeftTopSurround);
			mapTable[kIABChannelID_RightTopSurround]  = IABRendererBedChannelInfo(speakerURIRightTopSurround, vbapSpeakerCoordRightTopSurround);
			mapTable[kIABChannelID_LFE]               = IABRendererBedChannelInfo(speakerURILFE, speakerCoordLFE);
			mapTable[kIABChannelID_LeftHeight]        = IABRendererBedChannelInfo(speakerURILeftHeight, vbapSpeakerCoordLeftHeight);
			mapTable[kIABChannelID_RightHeight]       = IABRendererBedChannelInfo(speakerURIRightHeight, vbapSpeakerCoordRightHeight);
			mapTable[kIABChannelID_CenterHeight]      = IABRendererBedChannelInfo(speakerURICenterHeight, vbapSpeakerCoordCenterHeight);
			mapTable[kIABChannelID_LeftSurroundHeight]      = IABRendererBedChannelInfo(speakerURILeftSurroundHeight, vbapSpeakerCoordLeftSurroundHeight);
			mapTable[kIABChannelID_RightSurroundHeight]     = IABRendererBedChannelInfo(speakerURIRightSurroundHeight, vbapSpeakerCoordRightSurroundHeight);
			mapTable[kIABChannelID_LeftSideSurroundHeight]  = IABRendererBedChannelInfo(speakerURILeftSideSurroundHeight, vbapSpeakerCoordLeftSideSurroundHeight);
			mapTable[kIABChannelID_RightSideSurroundHeight] = IABRendererBedChannelInfo(speakerURIRightSideSurroundHeight, vbapSpeakerCoordRightSideSurroundHeight);
			mapTable[kIABChannelID_LeftRearSurroundHeight]  = IABRendererBedChannelInfo(speakerURILeftRearSurroundHeight, vbapSpeakerCoordLeftRearSurroundHeight);
			mapTable[kIABChannelID_RightRearSurroundHeight] = IABRendererBedChannelInfo(speakerURIRightRearSurroundHeight, vbapSpeakerCoordRightRearSurroundHeight);
			mapTable[kIABChannelID_TopSurround]       = IABRendererBedChannelInfo(speakerURITopSurround, vbapSpeakerCoordTopSurround);

			// ITU-R BS.2051-2 channels.
			mapTable[kIABChannelID_LeftTopFront]      = IABRendererBedChannelInfo(speakerURILeftTopFront, vbapSpeakerCoordLeftTopFront);
			mapTable[kIABChannelID_RightTopFront]     = IABRendererBedChannelInfo(speakerURIRightTopFront, vbapSpeakerCoordRightTopFront);
			mapTable[kIABChannelID_LeftTopBack]       = IABRendererBedChannelInfo(speakerURILeftTopBack, vbapSpeakerCoordLeftTopBack);
			mapTable[kIABChannelID_RightTopBack]      = IABRendererBedChannelInfo(speakerURIRightTopBack, vbapSpeakerCoordRightTopBack);
			mapTable[kIABChannelID_TopSideLeft]       = IABRendererBedChannelInfo(speakerURITopSideLeft, vbapSpeakerCoordTopSideLeft);
			mapTable[kIABChannelID_TopSideRight]      = IABRendererBedChannelInfo(speakerURITopSideRight, vbapSpeakerCoordTopSideRight);
			mapTable[kIABChannelID_LFE1]              = IABRendererBedChannelInfo(speakerURILFE1, speakerCoordLFE1);
			mapTable[kIABChannelID_LFE2]              = IABRendererBedChannelInfo(speakerURILFE2, speakerCoordLFE2);
			mapTable[kIABChannelID_FrontLeft]         = IABRendererBedChannelInfo(speakerURIFrontLeft, vbapSpeakerCoordFrontLeft);
			mapTable[kIABChannelID_FrontRight]        = IABRendererBedChannelInfo(speakerURIFrontRight, vbapSpeakerCoordFrontRight);

			return mapTable;
		}

		// Initialize IABConfigTables::soundfieldToUsecaseMap
		IABBedChannelInfoMap IABConfigTables::bedChannelInfoMap = IABConfigTables::CreateBedChannelInfoMap();

		// IABConfigTables::CreateSoundfieldToUsecaseMap() implementation
		SoundfieldURIToIABUseCaseMap IABConfigTables::CreateSoundfieldToIABUseCaseMap()
		{
			SoundfieldURIToIABUseCaseMap mapTable;

			mapTable[soundfieldURI_51] = kIABUseCase_5_1;
			mapTable[soundfieldURI_71DS] = kIABUseCase_7_1_DS;
			mapTable[soundfieldURI_71SDS] = kIABUseCase_7_1_SDS;
			mapTable[soundfieldURI_111HT] = kIABUseCase_11_1_HT;
			mapTable[soundfieldURI_131HT] = kIABUseCase_13_1_HT;
			mapTable[soundfieldURI_91OH] = kIABUseCase_9_1_OH;

			// ITU-R BS.2051-2 soundfields/use cases.
			mapTable[soundfieldURI_20_ITUA] = kIABUseCase_2_0_ITUA;
            mapTable[soundfieldURI_514_ITUD] = kIABUseCase_5_1_4_ITUD;
            mapTable[soundfieldURI_714_ITUJ] = kIABUseCase_7_1_4_ITUJ;

			return mapTable;
		}

		// Initialize IABConfigTables::soundfieldToUsecaseMap with IABConfigTables::CreateSoundfieldToUsecaseMap()
		SoundfieldURIToIABUseCaseMap IABConfigTables::soundfieldToIABUseCaseMap = IABConfigTables::CreateSoundfieldToIABUseCaseMap();

	} // namespace ImmersiveAudioBitstream
} // namespace SMPTE



