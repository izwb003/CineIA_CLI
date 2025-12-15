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
 * IABConfigTables.h
 *
 * @file
 */


#ifndef __IABCONFIGTABLES_H__
#define	__IABCONFIGTABLES_H__

#include "coreutils/Vector3.h"
#include "IABDataTypes.h"
#include "common/IABConstants.h"

namespace SMPTE
{
namespace ImmersiveAudioBitstream
{
	// Structure for polar position, in degrees
	typedef struct PolarPosition
	{
		float       radius_;							// Radius, 1: on unit circle, range [0, 2]
		float       azimuth_;							// azimuth angle, in degree, range [-180, 180]
		float       elevation_;							// elevation angle, in degree, range [0, 90]

		// Default constructor
		PolarPosition()
		{
			radius_ = 1.0f;
			azimuth_ = 0.0f;
			elevation_ = 0.0f;
		}

		// Parameterized constructor
		PolarPosition(float radius, float azimuth, float elevation)
		{
			radius_ = radius;
			azimuth_ = azimuth;
			elevation_ = elevation;
		}
	} PolarPosition;

	// *******************
	// Polar coordinate for speakers (corresponding to channels for ST2098-2 channel ID codes.
	// Refer to ST428-12 + ST2098-5 for channel definitions.)
	//
	// Speaker polar/spherical positions [radius, azimuth, elevation]
	// (defined in MDA and/or nearfield recommended Practice docs.)
	//
	const PolarPosition polarPositionSpeakerL(1.0f, -30.0f, 0.0f);
	const PolarPosition polarPositionSpeakerLC(1.0f, -16.1f, 0.0f);
	const PolarPosition polarPositionSpeakerC(1.0f, 0.0f, 0.0f);
	const PolarPosition polarPositionSpeakerRC(1.0f, 16.1f, 0.0f);
	const PolarPosition polarPositionSpeakerR(1.0f, 30.0f, 0.0f);
	const PolarPosition polarPositionSpeakerLSS(1.0f, -90.0f, 0.0f);
	const PolarPosition polarPositionSpeakerLS(1.0f, -110.0f, 0.0f);
	const PolarPosition polarPositionSpeakerLRS(1.0f, -150.0f, 0.0f);
	const PolarPosition polarPositionSpeakerRRS(1.0f, 150.0f, 0.0f);
	const PolarPosition polarPositionSpeakerRSS(1.0f, 90.0f, 0.0f);
	const PolarPosition polarPositionSpeakerRS(1.0f, 110.0f, 0.0f);
	const PolarPosition polarPositionSpeakerLTS(1.0f, -90.0f, 60.0f);
	const PolarPosition polarPositionSpeakerRTS(1.0f, 90.0f, 60.0f);
	const PolarPosition polarPositionSpeakerLFE(1.0f, 0.0f, 0.0f);
	const PolarPosition polarPositionSpeakerLH(1.0f, -30.0f, 21.0f);
	const PolarPosition polarPositionSpeakerRH(1.0f, 30.0f, 21.0f);
	const PolarPosition polarPositionSpeakerCH(1.0f, 0.0f, 24.0f);
	const PolarPosition polarPositionSpeakerLSH(1.0f, -110.0f, 30.0f);
	const PolarPosition polarPositionSpeakerRSH(1.0f, 110.0f, 30.0f);
	const PolarPosition polarPositionSpeakerLSSH(1.0f, -90.0f, 30.0f);
	const PolarPosition polarPositionSpeakerRSSH(1.0f, 90.0f, 30.0f);
	const PolarPosition polarPositionSpeakerLRSH(1.0f, -150.0f, 21.0f);
	const PolarPosition polarPositionSpeakerRRSH(1.0f, 150.0f, 21.0f);
	const PolarPosition polarPositionSpeakerTS(1.0f, 0.0f, 90.0f);

	// The 10 channels below are added in ST2098-2:2019. 
	// These channels are defined in ITU-R BS.2051-2.
	// 
	const PolarPosition polarPositionSpeakerLTF(1.0f, -24.79f, 35.99f);		// LTF position based on DTS LFH
	const PolarPosition polarPositionSpeakerRTF(1.0f, 24.79f, 35.99f);		// RTF position based on DTS RFH
	const PolarPosition polarPositionSpeakerLTB(1.0f, -155.21f, 35.99f);	// LTB position based on DTS LRH
	const PolarPosition polarPositionSpeakerRTB(1.0f, 155.21f, 35.99f);		// RTB position based on DTS RRH
	const PolarPosition polarPositionSpeakerTSSL(1.0f, -90.0f, 30.0f);
	const PolarPosition polarPositionSpeakerTSSR(1.0f, 90.0f, 30.0f);
	const PolarPosition polarPositionSpeakerLFE2(1.0f, 45.0f, -30.0f);
	const PolarPosition polarPositionSpeakerLFE3(1.0f, -45.0f, -30.0f);
	const PolarPosition polarPositionSpeakerLW(1.0f, -60.0f, 0.0f);
	const PolarPosition polarPositionSpeakerRW(1.0f, 60.0f, 0.0f);

	// *******************
    // URIs for bed channels
    const std::string speakerURILeft                      = "urn:smpte:ul:060E2B34.0401010D.03020101.00000000";
	const std::string speakerURILeftCenter                = "urn:smpte:ul:060E2B34.0401010D.0302010B.00000000";
    const std::string speakerURICenter                    = "urn:smpte:ul:060E2B34.0401010D.03020103.00000000";
	const std::string speakerURIRightCenter               = "urn:smpte:ul:060E2B34.0401010D.0302010C.00000000";
    const std::string speakerURIRight                     = "urn:smpte:ul:060E2B34.0401010D.03020102.00000000";
    const std::string speakerURILeftSideSurround          = "urn:smpte:ul:060E2B34.0401010D.03020107.00000000";
    const std::string speakerURILeftSurround              = "urn:smpte:ul:060E2B34.0401010D.03020105.00000000";
    const std::string speakerURILeftRearSurround          = "urn:smpte:ul:060E2B34.0401010D.03020109.00000000";
    const std::string speakerURIRightRearSurround         = "urn:smpte:ul:060E2B34.0401010D.0302010A.00000000";
    const std::string speakerURIRightSideSurround         = "urn:smpte:ul:060E2B34.0401010D.03020108.00000000";
    const std::string speakerURIRightSurround             = "urn:smpte:ul:060E2B34.0401010D.03020106.00000000";
    const std::string speakerURILeftTopSurround           = "http://dts.com/mda/channels#TSL";
    const std::string speakerURIRightTopSurround          = "http://dts.com/mda/channels#TSR";
    const std::string speakerURILFE                       = "urn:smpte:ul:060E2B34.0401010D.03020104.00000000";
	const std::string speakerURILeftHeight                = "http://dts.com/mda/channels#LH";
	const std::string speakerURIRightHeight               = "http://dts.com/mda/channels#RH";
	const std::string speakerURICenterHeight              = "http://dts.com/mda/channels#CH";
	const std::string speakerURILeftSurroundHeight        = "http://dts.com/mda/channels#LHS";		// Note that the URI intentionally contains "Lhs" instead of "Lsh" (the latter may mean "Left Side Height").
	const std::string speakerURIRightSurroundHeight       = "http://dts.com/mda/channels#RHS";		// Note that the URI intentionally contains "Rhs" instead of "Rsh" (the latter may mean "Right Side Height").
	const std::string speakerURILeftSideSurroundHeight    = "http://dts.com/mda/channels#LSSH";
	const std::string speakerURIRightSideSurroundHeight   = "http://dts.com/mda/channels#RSSH";
	const std::string speakerURILeftRearSurroundHeight    = "http://dts.com/mda/channels#LRSH";
	const std::string speakerURIRightRearSurroundHeight   = "http://dts.com/mda/channels#RRSH";
	const std::string speakerURITopSurround               = "http://dts.com/mda/channels#TS";

	// For ITU-R BS.2051-2 channels.
	const std::string speakerURILeftTopFront              = "tag:dts.com,2015:dtsx:channel:LFH";    // LFH speaker URI-maps to IAB LTF
	const std::string speakerURIRightTopFront             = "tag:dts.com,2015:dtsx:channel:RFH";    // RFH speaker URI-maps to IAB RTF
	const std::string speakerURILeftTopBack               = "tag:dts.com,2015:dtsx:channel:LRH";    // LRH speaker URI-maps to IAB LTB
	const std::string speakerURIRightTopBack              = "tag:dts.com,2015:dtsx:channel:RRH";    // RRH speaker URI-maps to IAB RTB
	const std::string speakerURITopSideLeft               = "tag:dts.com,2020:dtsx:channel:TSSL";
	const std::string speakerURITopSideRight              = "tag:dts.com,2020:dtsx:channel:TSSR";
	const std::string speakerURILFE1                      = "tag:dts.com,2015:dtsx:channel:LFE2";
	const std::string speakerURILFE2                      = "tag:dts.com,2015:dtsx:channel:LFE3";
	const std::string speakerURIFrontLeft                 = "tag:dts.com,2015:dtsx:channel:LW";
	const std::string speakerURIFrontRight                = "tag:dts.com,2015:dtsx:channel:RW";

	// *******************
	// Cartesian coordinate for bed channels (for ST2098-2 channel ID codes, ST428-12 + ST2098-5 + ITU-R BS.2051-2)
	// Cartesian coordinates are converted from their polar positions.

#define degreeToRadian (iabKPI / 180.0)
#define PolarToVBAPCartesian(inPolar) (float)(sin(inPolar.azimuth_ * degreeToRadian) * cos(inPolar.elevation_ * degreeToRadian)), \
		(float)(cos(inPolar.azimuth_ * degreeToRadian) * cos(inPolar.elevation_ * degreeToRadian)), \
		(float)(sin(inPolar.elevation_ * degreeToRadian))

	const CoreUtils::Vector3 vbapSpeakerCoordLeft(PolarToVBAPCartesian(polarPositionSpeakerL));
	const CoreUtils::Vector3 vbapSpeakerCoordLeftCenter(PolarToVBAPCartesian(polarPositionSpeakerLC));
	const CoreUtils::Vector3 vbapSpeakerCoordCenter(PolarToVBAPCartesian(polarPositionSpeakerC));
	const CoreUtils::Vector3 vbapSpeakerCoordRightCenter(PolarToVBAPCartesian(polarPositionSpeakerRC));
	const CoreUtils::Vector3 vbapSpeakerCoordRight(PolarToVBAPCartesian(polarPositionSpeakerR));
	const CoreUtils::Vector3 vbapSpeakerCoordLeftSideSurround(PolarToVBAPCartesian(polarPositionSpeakerLSS));
	const CoreUtils::Vector3 vbapSpeakerCoordLeftSurround(PolarToVBAPCartesian(polarPositionSpeakerLS));
	const CoreUtils::Vector3 vbapSpeakerCoordLeftRearSurround(PolarToVBAPCartesian(polarPositionSpeakerLRS));
	const CoreUtils::Vector3 vbapSpeakerCoordRightRearSurround(PolarToVBAPCartesian(polarPositionSpeakerRRS));
	const CoreUtils::Vector3 vbapSpeakerCoordRightSideSurround(PolarToVBAPCartesian(polarPositionSpeakerRSS));
	const CoreUtils::Vector3 vbapSpeakerCoordRightSurround(PolarToVBAPCartesian(polarPositionSpeakerRS));
	const CoreUtils::Vector3 vbapSpeakerCoordLeftTopSurround(PolarToVBAPCartesian(polarPositionSpeakerLTS));
	const CoreUtils::Vector3 vbapSpeakerCoordRightTopSurround(PolarToVBAPCartesian(polarPositionSpeakerRTS));
	const CoreUtils::Vector3 speakerCoordLFE(PolarToVBAPCartesian(polarPositionSpeakerLFE));
	const CoreUtils::Vector3 vbapSpeakerCoordLeftHeight(PolarToVBAPCartesian(polarPositionSpeakerLH));
	const CoreUtils::Vector3 vbapSpeakerCoordRightHeight(PolarToVBAPCartesian(polarPositionSpeakerRH));
	const CoreUtils::Vector3 vbapSpeakerCoordCenterHeight(PolarToVBAPCartesian(polarPositionSpeakerCH));
	const CoreUtils::Vector3 vbapSpeakerCoordLeftSurroundHeight(PolarToVBAPCartesian(polarPositionSpeakerLSH));
	const CoreUtils::Vector3 vbapSpeakerCoordRightSurroundHeight(PolarToVBAPCartesian(polarPositionSpeakerRSH));
	const CoreUtils::Vector3 vbapSpeakerCoordLeftSideSurroundHeight(PolarToVBAPCartesian(polarPositionSpeakerLSSH));
	const CoreUtils::Vector3 vbapSpeakerCoordRightSideSurroundHeight(PolarToVBAPCartesian(polarPositionSpeakerRSSH));
	const CoreUtils::Vector3 vbapSpeakerCoordLeftRearSurroundHeight(PolarToVBAPCartesian(polarPositionSpeakerLRSH));
	const CoreUtils::Vector3 vbapSpeakerCoordRightRearSurroundHeight(PolarToVBAPCartesian(polarPositionSpeakerRRSH));
	const CoreUtils::Vector3 vbapSpeakerCoordTopSurround(PolarToVBAPCartesian(polarPositionSpeakerTS));

	// ITU-R BS.2051-2 channels/speakers.
	const CoreUtils::Vector3 vbapSpeakerCoordLeftTopFront(PolarToVBAPCartesian(polarPositionSpeakerLTF));
	const CoreUtils::Vector3 vbapSpeakerCoordRightTopFront(PolarToVBAPCartesian(polarPositionSpeakerRTF));
	const CoreUtils::Vector3 vbapSpeakerCoordLeftTopBack(PolarToVBAPCartesian(polarPositionSpeakerLTB));
	const CoreUtils::Vector3 vbapSpeakerCoordRightTopBack(PolarToVBAPCartesian(polarPositionSpeakerRTB));
	const CoreUtils::Vector3 vbapSpeakerCoordTopSideLeft(PolarToVBAPCartesian(polarPositionSpeakerTSSL));
	const CoreUtils::Vector3 vbapSpeakerCoordTopSideRight(PolarToVBAPCartesian(polarPositionSpeakerTSSR));
	const CoreUtils::Vector3 speakerCoordLFE1(PolarToVBAPCartesian(polarPositionSpeakerLFE2));
	const CoreUtils::Vector3 speakerCoordLFE2(PolarToVBAPCartesian(polarPositionSpeakerLFE3));
	const CoreUtils::Vector3 vbapSpeakerCoordFrontLeft(PolarToVBAPCartesian(polarPositionSpeakerLW));
	const CoreUtils::Vector3 vbapSpeakerCoordFrontRight(PolarToVBAPCartesian(polarPositionSpeakerRW));

	// *******************
	// Soundfield URIs for ST2098-2 specified target layouts (use cases)
	// 5.1, 7.1DS, 7.1SDS, 9.1OH, 11.1HT, 13.1HT,
	// 2.0 (ie. ITU-A), 5.1.4 (ie. ITU-D), and 7.1.4 (ie. ITU-J) 
	const std::string soundfieldURI_51 = "urn:smpte:ul:060E2B34.0401010D.03020201.00000000";
	const std::string soundfieldURI_71DS = "urn:smpte:ul:060E2B34.0401010D.03020202.00000000";
	const std::string soundfieldURI_71SDS = "urn:smpte:ul:060E2B34.0401010D.03020203.00000000";
	const std::string soundfieldURI_91OH = "http://dts.com/mda/soundfield#91OH";
	const std::string soundfieldURI_111HT = "http://dts.com/mda/soundfield#111HT";
	const std::string soundfieldURI_131HT = "http://dts.com/mda/soundfield#131HT";
    const std::string soundfieldURI_20_ITUA = "tag:dts.com,2020:dtsx:channel-layout:2.0";
    const std::string soundfieldURI_514_ITUD = "tag:dts.com,2015:dtsx:channel-layout:5.1.4";
    const std::string soundfieldURI_714_ITUJ = "tag:dts.com,2015:dtsx:channel-layout:7.1.4";

	// *******************
	// Defines an IAB bed channel's associated speaker URI and spatial coordinates.
	// Speaker URI will be used to find the channel's position in the renderer output buffer block
	// If the speaker assoicated with the bed channel is not present in the target layout,
	// the bed channel will be rendered as an object using the spatial coordinates.
	typedef struct IABRendererBedChannelInfo
	{
		std::string             speakerURI_;                // The channel's associated speaker URI
		CoreUtils::Vector3      speakerVBAPCoordinates_;    // The channel's VBAP Cartesian coordinates [x, y, z], range [-1.0, 1.0]

		// Default constructor
		IABRendererBedChannelInfo()
		{
			speakerURI_ = "";
			speakerVBAPCoordinates_ = CoreUtils::Vector3();
		}

		// Parameterized constructor
		IABRendererBedChannelInfo(std::string speakerURI,
			CoreUtils::Vector3 speakerVBAPCoordinates)
		{
			speakerURI_ = speakerURI;
			speakerVBAPCoordinates_ = CoreUtils::Vector3(
				speakerVBAPCoordinates.x,
				speakerVBAPCoordinates.y,
				speakerVBAPCoordinates.z);
		}
	} IABRendererBedChannelInfo;

	// Map for use in creating table for mapping speaker/channel ID to corresponding 
	// speaker values: (speaker URI, nominal speaker coordinates).
	// Channel ID as key.
	typedef std::map<IABChannelIDType, IABRendererBedChannelInfo> IABBedChannelInfoMap;

	// Map for use in creating table for mapping soundfield URI to IAB use case.
	// Soundfield URI as key.
	typedef std::map<std::string, IABUseCaseType> SoundfieldURIToIABUseCaseMap;

	// *******************
	// Class containing map/tables between configuration and IAB parameters, either direction
	//
	class IABConfigTables {
	public:

		// (Config channel ID -> speaker/channel infomation) map table
		// Map containing speaker URI and VBAP Cartesian coordinates.
		static IABBedChannelInfoMap bedChannelInfoMap;

		// (Config soundfield URI -> IAB usecase) map table
		static SoundfieldURIToIABUseCaseMap soundfieldToIABUseCaseMap;

	private:

		// Create and initialize map table, IABConfigTables::bedChannelInfoMap
		static IABBedChannelInfoMap CreateBedChannelInfoMap();

		// Create and initialize map table, IABConfigTables::soundfieldToIABUseCaseMap
		static SoundfieldURIToIABUseCaseMap CreateSoundfieldToIABUseCaseMap();
	};

} // namespace ImmersiveAudioBitstream
} // namespace SMPTE

#endif // __IABCONFIGTABLES_H__
