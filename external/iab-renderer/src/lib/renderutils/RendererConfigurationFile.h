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

#ifndef __RENDERERCONFIGURATIONFILE_H__
#define __RENDERERCONFIGURATIONFILE_H__

#include "renderutils/IRendererConfiguration.h"

#include <iostream>
#include <vector>
#include <map>
#include <set>

namespace RenderUtils
{
    class RendererConfigurationFile : public IRendererConfigurationFile
    {

    public:
        // Begin IRendererConfigurationFile defined APIs
        //

        RendererConfigurationFile() :
            fConfigFileVersionChecked(false),
            fLFESpeakerIndex(-1),
            fChannelCount(0),
            fDecorrEnabled(false),
			fIABDecorrEnabled(true),
            fSmoothEnabled(true),
            fDebugEnabled(false),
            fHasBottomHemisphere(false)
        {
        }

        virtual ~RendererConfigurationFile()
        {
        }

        virtual IRendererConfiguration* newCopy() const;

        virtual rendererConfigError GetConfigVersion(int32_t &oVersion) const;

        virtual rendererConfigError GetDecorr(bool &oDecorrelated) const;

		virtual rendererConfigError GetIABDecorrEnable(bool &oIABDecorrEnable) const;

		virtual rendererConfigError GetSmooth(bool &oSmooth) const;

        virtual rendererConfigError GetTargetSoundfield(std::string& oTargetSoundField) const;

        virtual rendererConfigError GetSpeakers(const std::vector<RenderSpeaker>* &oSpeakers) const;

        virtual rendererConfigError GetTotalSpeakerCount(uint32_t &oTotalSpeakerCount) const;

        virtual rendererConfigError GetPatches(const std::vector<RenderPatch>* &oPatches) const;

        virtual rendererConfigError HasSpeaker(const std::string &iName, bool &oHasSpeaker) const;

        virtual rendererConfigError GetSpeakerIndexByName(const std::string &iName, int32_t &oIndex) const;

        virtual rendererConfigError GetSpeakerIndexbyChannel(int32_t iChan, int32_t &oIndex) const;

        virtual rendererConfigError GetSpeakerIndexbyURI(const std::string &iURI, int32_t &oIndex) const;

        virtual rendererConfigError GetLFEIndex(int32_t& oIndex) const;

		virtual const std::map<std::string, int32_t>& GetSpeakerURIToSpeakerIndexMap() const;

		virtual rendererConfigError GetChannelCount(uint32_t &oChannelCount) const;

        virtual rendererConfigError HasBottomHemisphere(bool &oHasBottomHemisphere) const;

        virtual rendererConfigError GetPhysicalSpeakers(const std::vector<RenderSpeaker>* &oSpeakers) const;

        virtual rendererConfigError GetPhysicalSpeakerCount(uint32_t &oPhysicalSpeakerCount) const;

        virtual rendererConfigError GetRendererOutputIndexByName(const std::string &iName, int32_t &oIndex) const;

        virtual rendererConfigError GetRendererOutputIndexbyChannel(int32_t iChan, int32_t &oIndex) const;

        virtual rendererConfigError GetRendererOutputIndexbyURI(const std::string &iURI, int32_t &oIndex) const;

        virtual rendererConfigError GetLFERendererOutputIndex(int32_t& oIndex) const;

        virtual const std::map<std::string, int32_t>& GetSpeakerNameToOutputIndexMap() const ;

        virtual const std::map<std::string, int32_t>& GetSpeakerURIToOutputIndexMap() const;

        virtual const std::map<int32_t, int32_t>& GetSpeakerChannelToOutputIndexMap() const;
        
        virtual bool IsVBAPSpeaker(const std::string &iName) const;

        //
        // End IRendererConfiguration defined APIs

        // Initialize configuration from cfg file string.
        virtual rendererConfigError InitFromCfgFile(const char *pTextConfig);

    private:
        static const unsigned int fCurrentConfigVersion = 3;

        bool fConfigFileVersionChecked;

        std::string fTargetSoundfield;
        int32_t fLFESpeakerIndex;
        int32_t fChannelCount;
        bool fDecorrEnabled;
		bool fIABDecorrEnabled;
        bool fSmoothEnabled;
        bool fDebugEnabled;
        std::string fAuthoringTool;
        std::string fAuthoringToolVersion;
        std::string fRecommendedPracticeVersion;


        bool  fHasBottomHemisphere;
        std::vector<RenderSpeaker>			fSpeakers;
        std::vector<RenderPatch>			fPatches;

        // Legacy speaker index maps for MDA
        std::map<int32_t, int32_t>          fChanToSpeaker;
        std::map<std::string, int32_t>      fNameToSpeaker;
        std::map<std::string, int32_t>      fURIToSpeaker;

        // Vectors and maps for physical outputs, these are used for IAB rendering and output mapping
        std::vector<RenderSpeaker>			fPhysicalSpeakers;
        std::map<int32_t, int32_t>          fChanToRendererOutputIndex;
        std::map<std::string, int32_t>      fNameToRendererOutputIndex;
        std::map<std::string, int32_t>      fURIToRendererOutputIndex;
        
        // List of configuration speakers that are in one or more VBAP RenderPatch
        std::set<std::string>               fVBAPSpeakerList;

        bool checkConfigFileVersion(int32_t pVersion);


        //
        // Set configuration parameters
        //
        rendererConfigError addSpeakerDownmix(int32_t source, const std::string pTargetName, float pCoefficient);


        // Internal data structure used for creating a renderer config
        //
        struct DownmixTarget
        {
            std::string fSpeakerName;

            float    fCoefficient;

            DownmixTarget(std::string pSpkr, float pCoef) :
                fSpeakerName(pSpkr),
                fCoefficient(pCoef)
            {
            }
        };

        rendererConfigError GetSpeakerDownmix(int32_t iIndex, const std::vector<DownmixValue>** oDownmixMap) const;

        rendererConfigError setTargetSoundfield(const std::string &pTargetSoundField);
        rendererConfigError addSpeaker(const std::string &pName, int32_t pChan, float pAzimuth, float pElevation, const std::string pURI);
        rendererConfigError setLFEbyOutputChannel(int32_t pChan);
        rendererConfigError setLFEbySpeakerName(std::string  pName);
        rendererConfigError addDownmix(const std::string pSourceName, const std::vector<DownmixTarget> pDownmix);
        rendererConfigError addVBAPPatch(const std::string pName1, const std::string pName2, const std::string pName3);
        rendererConfigError setAuthoringTool(const std::string &pAuthoringTool);
        rendererConfigError setAuthoringToolVersion(const std::string &pAuthoringToolVersion);
        rendererConfigError setRecommendedPracticeVersion(const std::string &pRPVersion);

        //
        // Process tokenized lines of cfg files.
        //
        bool processV_Version(std::vector<std::string> cmds, rendererConfigError &err);
        bool processC_Decorr(std::vector<std::string> cmds, rendererConfigError &err);
		bool processC_IABDecorrEnable(std::vector<std::string> cmds, rendererConfigError &err);
		bool processC_Smooth(std::vector<std::string> cmds, rendererConfigError &err);
        bool processC_Debug(std::vector<std::string> cmds, rendererConfigError &err);
        bool processE_Soundfield(std::vector<std::string> cmds, rendererConfigError &err);
        bool processS_Speaker(std::vector<std::string> cmds, rendererConfigError &err);
        bool processP_Patch(std::vector<std::string> cmds, rendererConfigError &err);
        bool processM_MixMap(std::vector<std::string> cmds, rendererConfigError &err);
        bool processW_LFE(std::vector<std::string> cmds, rendererConfigError &err);
        bool processK_AuthoringTool(std::vector<std::string> cmds, rendererConfigError &err);
        bool processK_AuthoringToolVersion(std::vector<std::string> cmds, rendererConfigError &err);
        bool processK_RecommendedPracticeVersion(std::vector<std::string> cmds, rendererConfigError &err);


        //
        // Functions to check configuration sanity.
        //
        bool    checkSoundfieldURIStringNotEmpty(std::string pSoundField) const;
        bool    checkSpeakerNameStringNotEmpty(std::string pName) const;
        bool    checkValidOutputChannel(int32_t pChan) const;
        bool    checkValidAzimuth(float pAzimuth) const;
        bool    checkValidElevation(float pElevation) const;
        bool    checkValidDownmixCoefficient(float pCoef) const;

        virtual void setDecorr(bool pDecorr);
		virtual void setIABDecorrEnable(bool pIABDecorr);
        virtual void setSmooth(bool pSmooth);
        virtual bool getDebug() const;
        virtual void setDebug(bool pDebug);
        virtual bool getConfigVersionChecked() const;
    };
}



#endif /* defined(__RENDERERCONFIGURATIONFILE_H__) */
