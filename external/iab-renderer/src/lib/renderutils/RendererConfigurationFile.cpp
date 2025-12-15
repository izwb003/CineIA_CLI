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

#include <algorithm>
#include <cctype>

#include "renderutils/RendererConfigurationFile.h"
#include "coreutils/CoreDefines.h"
#include "RendererConfigUtils.h"

namespace RenderUtils
{

    IRendererConfigurationFile* IRendererConfigurationFile::FromBuffer(const char *iTextConfig)
    {
        RendererConfigurationFile *configFile = new RendererConfigurationFile();

        rendererConfigError err = configFile->InitFromCfgFile(iTextConfig);
        if (err != kNoRendererConfigurationError)
        {
            delete configFile;
            configFile = NULL;
        }

        return configFile;
    }

    IRendererConfiguration* RendererConfigurationFile::newCopy() const
    {
        IRendererConfiguration* newConfig = new RendererConfigurationFile(*this);

        return newConfig;
    }

    rendererConfigError RendererConfigurationFile::GetConfigVersion(int32_t &oVersion) const
    {
        oVersion = fCurrentConfigVersion;

        return kNoRendererConfigurationError;
    }

    //
    // Part 1: Functions to set, get and query parameters in a RendererConfiguration.
    //

    // cfg-file version handling

    bool RendererConfigurationFile::checkConfigFileVersion(int32_t pVersion)
    {

        fConfigFileVersionChecked = ( pVersion == fCurrentConfigVersion );

        return fConfigFileVersionChecked;
    }


    bool RendererConfigurationFile::getConfigVersionChecked() const
    {

        return fConfigFileVersionChecked;
    }

    rendererConfigError RendererConfigurationFile::setAuthoringTool(const std::string &pAuthoringTool)
    {
        fAuthoringTool = pAuthoringTool;
        return kNoRendererConfigurationError;
    }

    rendererConfigError RendererConfigurationFile::setAuthoringToolVersion(const std::string &pAuthoringToolVersion)
    {
        fAuthoringToolVersion = pAuthoringToolVersion;
        return kNoRendererConfigurationError;
    }

    rendererConfigError RendererConfigurationFile::setRecommendedPracticeVersion(const std::string &pRPVersion)
    {
        fRecommendedPracticeVersion = pRPVersion;
        return kNoRendererConfigurationError;
    }

    // Renderer flags

    void RendererConfigurationFile::setDebug(bool pDebug)
    {

        fDebugEnabled = pDebug;
    }


    bool RendererConfigurationFile::getDebug() const
    {

        return fDebugEnabled;
    }


    void RendererConfigurationFile::setDecorr(bool pDecorr)
    {

        fDecorrEnabled = pDecorr;
    }


    rendererConfigError RendererConfigurationFile::GetDecorr(bool &oDecorrelated) const
    {

        oDecorrelated = fDecorrEnabled;
        return kNoRendererConfigurationError;
    }


	void RendererConfigurationFile::setIABDecorrEnable(bool pIABDecorr)
	{

		fIABDecorrEnabled = pIABDecorr;
	}

	rendererConfigError RendererConfigurationFile::GetIABDecorrEnable (bool &oIABDecorrEnable) const
	{

		oIABDecorrEnable = fIABDecorrEnabled;
		return kNoRendererConfigurationError;
	}


    void RendererConfigurationFile::setSmooth(bool pSmooth)
    {

        fSmoothEnabled = pSmooth;
    }


    rendererConfigError RendererConfigurationFile::GetSmooth(bool &oSmooth) const
    {

        oSmooth = fSmoothEnabled;
        return kNoRendererConfigurationError;
    }



    // Sound field

    rendererConfigError RendererConfigurationFile::setTargetSoundfield(const std::string &pTargetSoundField)
    {

        if (!checkSoundfieldURIStringNotEmpty(pTargetSoundField))
        {
            return kInvalidParameterError;
        }

        fTargetSoundfield = pTargetSoundField;
        return kNoRendererConfigurationError;
    }


    rendererConfigError RendererConfigurationFile::GetTargetSoundfield(std::string& oTargetSoundField) const
    {

        oTargetSoundField = fTargetSoundfield;
        return kNoRendererConfigurationError;
    }


    // Speakers

    rendererConfigError RendererConfigurationFile::addSpeaker(const std::string &pName, int32_t pChan, float pAzimuth, float pElevation, const std::string pURI)
    {

        // check if provided parameters are permitted speaker parameters
        if (!checkSpeakerNameStringNotEmpty(pName))
        {
            return kInvalidParameterError ;
        }
        if (!checkValidOutputChannel(pChan))
        {
            return kInvalidParameterError ;
        }
        if (!checkValidAzimuth(pAzimuth))
        {
            return kInvalidParameterError;
        }
        if (!checkValidElevation(pElevation))
        {
            return kInvalidParameterError;
        }

        // prevent duplicate speakers with same name,
        if (fNameToSpeaker.find(pName) != fNameToSpeaker.end())
        {
            return kDuplicateSpeakerParametersError;
        }

        // prevent duplicate speakers with same output channel (other than -1 / virtual),
        if ((pChan > -1) && (fChanToSpeaker.find(pChan) != fChanToSpeaker.end()))
        {
            return kDuplicateSpeakerParametersError;
        }

        // prevent duplicate speakers with same URI (other than empty URI).
        if ((pURI != "") && (fURIToSpeaker.find(pURI) != fURIToSpeaker.end()))
        {
            return kDuplicateSpeakerParametersError;
        }

        // Set to -1 for virtual speaker
        int32_t outputIndex = -1;

        // For physical outputs, set outputIndex and update renderer output index maps
        if (pChan > -1)
        {
            outputIndex = (int32_t)fPhysicalSpeakers.size();
            fPhysicalSpeakers.push_back( RenderSpeaker(pName, pChan, outputIndex, pAzimuth, pElevation, pURI));
            fChanToRendererOutputIndex.insert(std::pair<int32_t,int32_t>(pChan, outputIndex));
            fNameToRendererOutputIndex.insert(std::pair<std::string,int32_t>(pName, outputIndex));

            if (pURI != "")
            {
                fURIToRendererOutputIndex.insert(std::pair<std::string,int32_t>(pURI, outputIndex));
            }
        }

        fSpeakers.push_back( RenderSpeaker(pName, pChan, outputIndex, pAzimuth, pElevation, pURI) );

        int32_t spkindex = (int32_t) ( fSpeakers.size() - 1);

        fNameToSpeaker.insert(std::pair<std::string,int32_t>(pName, spkindex));

        if (pURI != "")
        {
            fURIToSpeaker.insert(std::pair<std::string,int32_t>(pURI, spkindex));
        }

        if (pChan > -1)
        {
            fChanToSpeaker.insert(std::pair<int32_t,int32_t>(pChan, spkindex));
        }

        return kNoRendererConfigurationError;
    }


    rendererConfigError RendererConfigurationFile::GetSpeakers(const std::vector<RenderSpeaker>* &oSpeakers) const
    {
        oSpeakers = &fSpeakers;

        return kNoRendererConfigurationError;
    }

    rendererConfigError RendererConfigurationFile::GetTotalSpeakerCount(uint32_t &oTotalSpeakerCount) const
    {
        oTotalSpeakerCount = static_cast<uint32_t>(fSpeakers.size());

        return kNoRendererConfigurationError;
    }

    rendererConfigError RendererConfigurationFile::HasSpeaker(const std::string &iName, bool &oHasSpeaker) const
    {

        if (fNameToSpeaker.find(iName) != fNameToSpeaker.end())
        {
            oHasSpeaker = true;
        }
        else
        {
            oHasSpeaker = false;
        }

        return kNoRendererConfigurationError;
    }


    rendererConfigError RendererConfigurationFile::GetSpeakerIndexByName(const std::string &iName, int32_t &oIndex) const
    {

        if (fNameToSpeaker.find(iName) == fNameToSpeaker.end())
        {

            oIndex = -1;

            return kNoSuchSpeakerError;
        }

        oIndex = static_cast<int32_t>(fNameToSpeaker.at(iName));

        return kNoRendererConfigurationError;
    }


    rendererConfigError RendererConfigurationFile::GetSpeakerIndexbyChannel(int32_t iChan, int32_t &oIndex) const
    {

        if (fChanToSpeaker.find(iChan) == fChanToSpeaker.end())
        {

            oIndex = -1;

            return kNoSuchSpeakerError;
        }

        oIndex = fChanToSpeaker.at(iChan);
        return kNoRendererConfigurationError;
    }


    rendererConfigError RendererConfigurationFile::GetSpeakerIndexbyURI(const std::string &iURI, int32_t &oIndex) const
    {

        if (fURIToSpeaker.find(iURI) == fURIToSpeaker.end())
        {

            oIndex = -1;

            return kNoSuchSpeakerError;
        }

        oIndex = fURIToSpeaker.at(iURI);
        return kNoRendererConfigurationError;
    }

    // LFE

    rendererConfigError RendererConfigurationFile::setLFEbyOutputChannel(int32_t pChan)
    {

        rendererConfigError err = kNoRendererConfigurationError;

        if (fChanToSpeaker.find(pChan) != fChanToSpeaker.end())
        {

            fLFESpeakerIndex=fChanToSpeaker.at(pChan);
        }
        else
        {

            err = kNoSuchSpeakerError;
        }

        return err;
    }


    rendererConfigError RendererConfigurationFile::setLFEbySpeakerName(std::string  pName)
    {

        rendererConfigError err = kNoRendererConfigurationError;


        if (fNameToSpeaker.find(pName) != fNameToSpeaker.end())
        {

            fLFESpeakerIndex=fNameToSpeaker[pName];
        }
        else
        {

            err = kNoSuchSpeakerError;
        }

        return err;
    }


    rendererConfigError RendererConfigurationFile::GetLFEIndex(int32_t& oIndex) const
    {

        oIndex = fLFESpeakerIndex;

        if (fLFESpeakerIndex > -1)
        {
            return kNoRendererConfigurationError;
        }

        return kNoLFESpeakerError;
    }

    rendererConfigError RendererConfigurationFile::GetPhysicalSpeakers(const std::vector<RenderSpeaker>* &oSpeakers) const
    {
        oSpeakers = &fPhysicalSpeakers;

        return kNoRendererConfigurationError;
    }

    rendererConfigError RendererConfigurationFile::GetPhysicalSpeakerCount(uint32_t &oPhysicalSpeakerCount) const
    {
        oPhysicalSpeakerCount = static_cast<uint32_t>(fPhysicalSpeakers.size());

        return kNoRendererConfigurationError;
    }

    rendererConfigError RendererConfigurationFile::GetRendererOutputIndexByName(const std::string &iName, int32_t &oIndex) const
    {
        if (fNameToRendererOutputIndex.find(iName) == fNameToRendererOutputIndex.end())
        {

            oIndex = -1;

            return kNoSuchSpeakerError;
        }

        oIndex = static_cast<int32_t>(fNameToRendererOutputIndex.at(iName));
        return kNoRendererConfigurationError;
    }

    rendererConfigError RendererConfigurationFile::GetRendererOutputIndexbyChannel(int32_t iChan, int32_t &oIndex) const
    {
        if (fChanToRendererOutputIndex.find(iChan) == fChanToRendererOutputIndex.end())
        {

            oIndex = -1;

            return kNoSuchSpeakerError;
        }

        oIndex = static_cast<int32_t>(fChanToRendererOutputIndex.at(iChan));
        return kNoRendererConfigurationError;
    }

    rendererConfigError RendererConfigurationFile::GetRendererOutputIndexbyURI(const std::string &iURI, int32_t &oIndex) const
    {
        if (fURIToRendererOutputIndex.find(iURI) == fURIToRendererOutputIndex.end())
        {

            oIndex = -1;

            return kNoSuchSpeakerError;
        }

        oIndex = static_cast<int32_t>(fURIToRendererOutputIndex.at(iURI));
        return kNoRendererConfigurationError;
    }

    rendererConfigError RendererConfigurationFile::GetLFERendererOutputIndex(int32_t& oIndex) const
    {
        oIndex = -1;
        int32_t lfeSpeakerindex = -1;
        rendererConfigError returnCode = GetLFEIndex(lfeSpeakerindex);

        if (kNoRendererConfigurationError == returnCode)
        {
            returnCode = GetRendererOutputIndexbyChannel(lfeSpeakerindex, oIndex);

            if ((kNoRendererConfigurationError != returnCode) || (oIndex < 0))
            {
                return kNoLFESpeakerError;
            }

            return kNoRendererConfigurationError;
        }

        return kNoLFESpeakerError;
    }

    const std::map<std::string, int32_t>& RendererConfigurationFile::GetSpeakerNameToOutputIndexMap() const
    {
        return fNameToRendererOutputIndex;
    }

	const std::map<std::string, int32_t>& RendererConfigurationFile::GetSpeakerURIToSpeakerIndexMap() const
	{
		return fURIToSpeaker;
	}

	const std::map<std::string, int32_t>& RendererConfigurationFile::GetSpeakerURIToOutputIndexMap() const
    {
        return fURIToRendererOutputIndex;
    }

    const std::map<int32_t, int32_t>& RendererConfigurationFile::GetSpeakerChannelToOutputIndexMap() const
    {
        return fChanToRendererOutputIndex;
    }

    // Downmix Maps

    rendererConfigError RendererConfigurationFile::addDownmix(const std::string pSourceName, std::vector<DownmixTarget> pDownmix)
    {

        int32_t source;

        if (GetSpeakerIndexByName(pSourceName, source) != kNoRendererConfigurationError)
        {
            return kNoSuchSpeakerError;
        }
        if (pDownmix.size() == 0)
        {
            return kInvalidParameterError;
        }
        if (fSpeakers[source].hasDownmix())
        {
            return kInvalidParameterError;    // speaker already has downmix map set.
        }

        for(std::vector<DownmixTarget>::iterator it = pDownmix.begin(); it != pDownmix.end(); ++it)
        {
            rendererConfigError err  = addSpeakerDownmix(source, it->fSpeakerName, it->fCoefficient);
            if (err != kNoRendererConfigurationError)
            {
                return err;
            }
        }
        fSpeakers[source].normalizeDownMixValues();

        return kNoRendererConfigurationError;
    }


    rendererConfigError RendererConfigurationFile::addSpeakerDownmix(int32_t source, const std::string pTargetName, const float pCoefficient)
    {
        int32_t target = 0;

        // check if target and coefficient are valid
        if (GetSpeakerIndexByName(pTargetName, target) != kNoRendererConfigurationError)
        {
            return kNoSuchSpeakerError;
        }
        if (checkValidDownmixCoefficient(pCoefficient) != true)
        {
            return kInvalidParameterError;
        }

        // no downmix to virtual speaker
        if (fSpeakers[target].getChannel() < 0)
        {
            return kInvalidParameterError;
        }

        // check if downmix from source to target already exists
        const std::vector<DownmixValue>* dwnmix;

        if (GetSpeakerDownmix(source, &dwnmix) != kNoRendererConfigurationError)
        {
            return kInvalidParameterError;
        }

        for(std::vector<DownmixValue>::const_iterator it = dwnmix->begin(); it != dwnmix->end(); ++it)
        {

            if (it->ch_ == fSpeakers[target].getChannel())
            {
                return kInvalidParameterError;
            }
        }

        DownmixValue dnmx = DownmixValue(fSpeakers[target].getChannel(), pCoefficient);
        fSpeakers[source].addDownMixValue(dnmx);


        return kNoRendererConfigurationError;
    }


    rendererConfigError RendererConfigurationFile::GetSpeakerDownmix(int32_t iIndex, const std::vector<DownmixValue>** oDownmixMap) const
    {
        if (iIndex >= static_cast<int32_t>(fSpeakers.size()))
        {
            return kNoDownMixSpeakerError;
        }

        *oDownmixMap = &fSpeakers[iIndex].getMixmaps();

        return kNoRendererConfigurationError;
    }



    // VBAP Patches

    rendererConfigError RendererConfigurationFile::addVBAPPatch(const std::string pName1, const std::string pName2, const std::string pName3)
    {

        int32_t s1, s2, s3;

        if (GetSpeakerIndexByName(pName1, s1) != kNoRendererConfigurationError)
        {
            return kNoSuchSpeakerError;
        }
        if (GetSpeakerIndexByName(pName2, s2) != kNoRendererConfigurationError)
        {
            return kNoSuchSpeakerError;
        }
        if (GetSpeakerIndexByName(pName3, s3) != kNoRendererConfigurationError)
        {
            return kNoSuchSpeakerError;
        }

        CoreUtils::Matrix3 m(fSpeakers[s1].getPosition(),
                             fSpeakers[s2].getPosition(),
                             fSpeakers[s3].getPosition());

        if (fabs(m.invert()) < CoreUtils::kEPSILON)
        {
            return kInvalidPatchError;
        }

        fPatches.push_back( RenderPatch(s1, s2, s3, m) );
        
        // Add speakers to VBAP speaker list
        // std::set keeps unique elements only, so if the speaker name is already in the fVBAPSpeakerList, it will not be inserted
        fVBAPSpeakerList.insert(pName1);
        fVBAPSpeakerList.insert(pName2);
        fVBAPSpeakerList.insert(pName3);

        return kNoRendererConfigurationError;
    }


    rendererConfigError RendererConfigurationFile::GetPatches(const std::vector<RenderPatch>* &oPatches) const
    {

        oPatches = &fPatches;
        return kNoRendererConfigurationError;
    }



    // Other queries

    rendererConfigError RendererConfigurationFile::GetChannelCount(uint32_t &oChannelCount) const
    {

        oChannelCount = static_cast<uint32_t>(fChannelCount);

        return kNoRendererConfigurationError;
    }


    rendererConfigError RendererConfigurationFile::HasBottomHemisphere(bool &oHasBottomHemisphere) const
    {

        oHasBottomHemisphere = fHasBottomHemisphere;

        return kNoRendererConfigurationError;
    }

    //
    // Part 2: C++ functions to tokenize and parse cfg files.
    //

    // Functions that process tokenized lines of cfg files.

    bool RendererConfigurationFile::processC_Decorr(std::vector<std::string> cmds, rendererConfigError &err)
    {


        err = kInvalidCfgLineError;

        if ((cmds[0] == "C") &&  (cmds[1] == "DECORR") && (cmds.size() == 3))
        {

            int32_t decorrval = 0;

            if (readnumerical<int32_t>(cmds[2], decorrval))
            {

                setDecorr(decorrval > 0);
                err = kNoRendererConfigurationError;
            }
            else
            {
                err = kInvalidCfgLineError;
            }
        }

        return (err == kNoRendererConfigurationError);
    }

	bool RendererConfigurationFile::processC_IABDecorrEnable(std::vector<std::string> cmds, rendererConfigError &err)
	{


		err = kInvalidCfgLineError;

		if ((cmds[0] == "C") && (cmds[1] == "IABDECORR") && (cmds.size() == 3))
		{

			int32_t iabdecorrval = 0;

			if (readnumerical<int32_t>(cmds[2], iabdecorrval))
			{

				setIABDecorrEnable(iabdecorrval > 0);
				err = kNoRendererConfigurationError;
			}
			else
			{
				err = kInvalidCfgLineError;
			}
		}

		return (err == kNoRendererConfigurationError);
	}

    bool RendererConfigurationFile::processC_Smooth(std::vector<std::string> cmds, rendererConfigError &err)
    {


        err = kInvalidCfgLineError;

        if ((cmds[0] == "C") &&  (cmds[1] == "SMOOTH") && (cmds.size() == 3))
        {

            int32_t smoothval = 0;

            if (readnumerical<int32_t>(cmds[2], smoothval))
            {

                setSmooth(smoothval > 0);
                err = kNoRendererConfigurationError;
            }

        }

        return (err == kNoRendererConfigurationError);
    }


    bool RendererConfigurationFile::processC_Debug(std::vector<std::string> cmds, rendererConfigError &err)
    {


        err = kInvalidCfgLineError;

        if ((cmds[0] == "C") &&  (cmds[1] == "DEBUG") && (cmds.size() == 3))
        {

            int32_t debugval = 0;

            if (readnumerical<int32_t>(cmds[2], debugval))
            {

                setDebug(debugval > 0);
                err = kNoRendererConfigurationError;
            }

        }

        return (err == kNoRendererConfigurationError);
    }

    bool RendererConfigurationFile::processK_AuthoringTool(std::vector<std::string> cmds, rendererConfigError &err)
    {

        err = kInvalidCfgLineError;

        if ((cmds[0] == "K") &&  (cmds[1] == "AUTHTOOL") && (cmds.size() == 3))
        {

            err = setAuthoringTool(cmds[2]);

        }

        return (err == kNoRendererConfigurationError);
    }

    bool RendererConfigurationFile::processK_AuthoringToolVersion(std::vector<std::string> cmds, rendererConfigError &err)
    {

        err = kInvalidCfgLineError;

        if ((cmds[0] == "K") &&  (cmds[1] == "AUTHTOOLVERSION") && (cmds.size() == 3))
        {

            err = setAuthoringToolVersion(cmds[2]);

        }

        return (err == kNoRendererConfigurationError);
    }

    bool RendererConfigurationFile::processK_RecommendedPracticeVersion(std::vector<std::string> cmds, rendererConfigError &err)
    {
        err = kInvalidCfgLineError;

        if ((cmds[0] == "K") &&  (cmds[1] == "RPVERSION") && (cmds.size() == 3))
        {

            err = setRecommendedPracticeVersion(cmds[2]);

        }

        return (err == kNoRendererConfigurationError);
    }

    bool RendererConfigurationFile::processV_Version(std::vector<std::string> cmds, rendererConfigError &err)
    {


        err = kInvalidCfgLineError;

        if ((cmds[0] == "V") &&  (cmds.size() == 2))
        {

            int32_t version = -1;

            if (readnumerical<int32_t>(cmds[1], version))
            {


                if (checkConfigFileVersion(version))
                {

                    err = kNoRendererConfigurationError;

                }
                else
                {

                    err = kDeprecatedCfgFileError;
                    return false;
                }
            }

        }

        return (err == kNoRendererConfigurationError);
    }


    bool RendererConfigurationFile::processE_Soundfield(std::vector<std::string> cmds, rendererConfigError &err)
    {

        err = kInvalidCfgLineError;

        if ((cmds[0] == "E") &&   (cmds.size() == 2))
        {

            err = setTargetSoundfield(cmds[1]);

        }

        return (err == kNoRendererConfigurationError);
    }


    bool RendererConfigurationFile::processS_Speaker(std::vector<std::string> cmds, rendererConfigError &err)
    {


        err = kInvalidCfgLineError;

        if ((cmds[0] == "S") && ((cmds.size() == 5) || (cmds.size() == 6)))
        {
            std::string name;
            int32_t chan;
            float azimuth;
            float elevation;
            std::string uri = "";


            /* Get Speaker Name */
            if (cmds[1].size()==0)
            {
                err = kInvalidCfgLineError;
                return false;
            }

            name=cmds[1];

            // Comment in this line to disable case sensitivity in speaker names:
            // transform(name.begin(), name.end(), name.begin(), ::toupper);


            /* Get Speaker Channel */

            if (!readnumerical<int32_t>(cmds[2],chan))   // if cmds[2] is not numerical, we demand that it is '*'
            {

                if (cmds[2] == "*")
                {

                    chan = -1;
                }
                else
                {
                    err = kInvalidCfgLineError;
                    return false;
                }
            }


            else         // if cmds[2] is numerical, we demand the value is >= 0
            {
                if (chan < 0)
                {
                    err = kInvalidParameterError;
                    return false;
                }
            }

            if ( (chan + 1) > fChannelCount)
            {
                fChannelCount = chan + 1;    // adjust channelcount.
            }


            if (!readnumerical<float>(cmds[3],azimuth))
            {
                err = kInvalidCfgLineError;
                return false;
            }


            /* Get Elevation */
            if (!readnumerical<float>(cmds[4],elevation))
            {
                err = kInvalidCfgLineError;
                return false;
            }

            if (elevation < 0)
            {
                fHasBottomHemisphere = true;    // determine if bottom hemisphere needs configuring
            }



            /* Get URI */
            if (cmds.size() == 6)
            {

                uri = cmds[5];
            }


            /* Add Speaker with found parameters. Function checks for correct value ranges.   */
            err = addSpeaker(name, chan, azimuth, elevation, uri);

        }

        return (err == kNoRendererConfigurationError);
    }


    bool RendererConfigurationFile::processP_Patch(std::vector<std::string> cmds, rendererConfigError &err)
    {


        err = kInvalidCfgLineError;

        if ((cmds[0] == "P") && (cmds.size() == 4))
        {

            err = addVBAPPatch(cmds[1], cmds[2], cmds[3]);
        }

        return (err == kNoRendererConfigurationError);
    }


    bool RendererConfigurationFile::processM_MixMap(std::vector<std::string> cmds, rendererConfigError &err)
    {


        err = kInvalidCfgLineError;

        if ((cmds.size() > 1) && (cmds[0] == "M") && (cmds.size()%2 == 0) )
        {

            // get name of speaker to which to add the downmix
            std::string srcname = cmds[1];

            // vector of DownmixTarget structs to store all mixdown coefficients in m-line
            std::vector<DownmixTarget> targetmap;

            // for each pair of cmd strings...
            for (uint32_t j = 1; j < cmds.size() - 1; j = j + 2)
            {

                // get name of destination speaker
                std::string destname = cmds[j+1];

                // get downmix coefficient value
                float coef;
                if (!readnumerical<float>(cmds[j + 2],coef))
                {
                    err=kInvalidCfgLineError;
                    return false;
                }

                // add another Downmixtarget to the map:

                targetmap.push_back( DownmixTarget(destname, coef) );
            }

            // set map as a whole. this will return error if there are added downmix coefficients in the speaker already.
            err = addDownmix(srcname, targetmap);

        }

        return (err == kNoRendererConfigurationError);
    }


    bool RendererConfigurationFile::processW_LFE(std::vector<std::string> cmds, rendererConfigError &err)
    {

        err = kInvalidCfgLineError;

        if ((cmds[0] == "W") && (cmds.size() == 2))
        {

            int32_t lfechannel = 0;

            if (readnumerical<int32_t>(cmds[1], lfechannel))
            {

                err = setLFEbyOutputChannel(lfechannel);
            }
            else
            {

                err = setLFEbySpeakerName(cmds[1]);
            }
        }

        return (err == kNoRendererConfigurationError);
    }


    //
    // Part 3: Configuration sanity checks
    //

    bool RendererConfigurationFile::checkSoundfieldURIStringNotEmpty(std::string pSoundField) const
    {

        if (pSoundField.size()==0)
        {
            return false;
        }
        return true;
    }


    bool    RendererConfigurationFile::checkSpeakerNameStringNotEmpty(std::string pName) const
    {

        if (pName.size()==0)
        {
            return false;
        }
        return true;
    }


    bool    RendererConfigurationFile::checkValidOutputChannel(int32_t pChan) const
    {

        if (pChan < -1)
        {
            return false;
        }
        return true;
    }


    bool    RendererConfigurationFile::checkValidAzimuth(float pAzimuth) const
    {

        return ((pAzimuth <= 360) && (pAzimuth >= -360));
    }


    bool    RendererConfigurationFile::checkValidElevation(float pElevation) const
    {

        return ((pElevation <= 90) && (pElevation >= -90)) ;
    }


    bool    RendererConfigurationFile::checkValidDownmixCoefficient(float pCoef) const
    {

        return (pCoef >= 0);
    }


    bool RendererConfigurationFile::IsVBAPSpeaker(const std::string &iName) const
    {
        std::set<std::string>::iterator iter = fVBAPSpeakerList.find(iName);
        
        if (iter != fVBAPSpeakerList.end())
        {
            // If found return true
            return true;
        }
        
        // Not found, return false
        return false;
    }

    //
    // Part 4: Public functions of RendererConfiguration
    //

    rendererConfigError RendererConfigurationFile::InitFromCfgFile(const char *pTextConfig)
    {

        if (! (pTextConfig))
        {
            return kInvalidParameterError;
        }

        rendererConfigError rc = kNoRendererConfigurationError;

        std::stringstream ss;

        size_t slen = std::char_traits<char>::length(pTextConfig);

        for( size_t i = 0 ; i < slen ; ++i )
        {

            ss << std::use_facet< std::ctype<char> >( std::locale() ).narrow( pTextConfig[i], ' ' ) ;
        }

        std::string cfgstr = ss.str();

        std::vector<std::string> cfglns = linize(cfgstr);

        std::vector<std::string>::iterator iter;

        // do speaker + LFE + smoothing + soundfield

        for (iter = cfglns.begin(); iter != cfglns.end(); iter++)
        {

            std::vector<std::string> cmds  = tokenize(*iter);

            if (cmds.empty()) continue;

            transform(cmds[0].begin(),cmds[0].end(), cmds[0].begin(), toupper);


            if (cmds[0] == "V")
            {
                if( !processV_Version(cmds, rc) )
                {
                    return rc;
                }
            }


            if (cmds[0] == "C")
            {

                if (cmds.size() > 1)
                {

                    transform(cmds[1].begin(),cmds[1].end(), cmds[1].begin(), toupper);

                    if (cmds[1] == "SMOOTH")
                    {

                        if( !processC_Smooth(cmds, rc) )
                        {
                            return rc;
                        }

                    }
                    else if (cmds[1] == "DECORR")
                    {

                        if( !processC_Decorr(cmds, rc) )
                        {
                            return rc;
                        }

                    }
					else if (cmds[1] == "IABDECORR")
					{

						if (!processC_IABDecorrEnable(cmds, rc))
						{
							return rc;
						}

					}
					else if (cmds[1] == "DEBUG")
                    {

                        if( !processC_Debug(cmds, rc) )
                        {
                            return rc;
                        }

                    }
                    else
                    {

                        return kInvalidCfgLineError;
                    }

                }
                else
                {

                    return kInvalidCfgLineError;
                }
            };

            if (cmds[0] == "E")
            {
                if( ! processE_Soundfield(cmds, rc) )
                {
                    return rc;
                }
            }

            if (cmds[0] == "S")
            {
                if( ! processS_Speaker(cmds, rc) )
                {
                    return rc;
                }
            }

            if (cmds[0] == "K")
            {

                if (cmds.size() > 1)
                {

                    transform(cmds[1].begin(),cmds[1].end(), cmds[1].begin(), toupper);

                    if (cmds[1] == "AUTHTOOL")
                    {

                        if( !processK_AuthoringTool(cmds, rc) )
                        {
                            return rc;
                        }

                    }
                    else if (cmds[1] == "AUTHTOOLVERSION")
                    {

                        if( !processK_AuthoringToolVersion(cmds, rc) )
                        {
                            return rc;
                        }

                    }
                    else if (cmds[1] == "RPVERSION")
                    {

                        if( !processK_RecommendedPracticeVersion(cmds, rc) )
                        {
                            return rc;
                        }

                    }
                }
                else
                {

                    return kInvalidCfgLineError;
                }
            }
        }

        // do downmix  + patches

        for (iter = cfglns.begin(); iter != cfglns.end(); iter++)
        {

            std::vector<std::string> cmds = tokenize(*iter);

            if (cmds.empty()) continue;

            transform(cmds[0].begin(),cmds[0].end(), cmds[0].begin(), toupper);

            if (cmds[0] == "W")
            {
                if( !processW_LFE(cmds, rc) )
                {
                    return rc;
                }
            }


            if (cmds[0] == "M")
            {
                if( !processM_MixMap(cmds, rc) )
                {
                    return rc;
                }
            }


            if (cmds[0] == "P")
            {
                if( !processP_Patch(cmds, rc) )
                {
                    return rc;
                }
            }


        }

        return rc;
    }
}

