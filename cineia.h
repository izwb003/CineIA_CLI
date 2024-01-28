/*
 Copyright (c) 2024. Steven Song (izwb-003)
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 */

#ifndef CINEIA_CLI_CINEIA_H
#define CINEIA_CLI_CINEIA_H

#include <IABErrors.h>
#include <common/IABElements.h>

using namespace SMPTE::ImmersiveAudioBitstream;

namespace CineIA {
    struct iabFrameInfo {
        IABFrameRateType frameRate;
        IABSampleRateType sampleRate;
        IABBitDepthType bitDepth;
        IABMaxRenderedRangeType maxRendered;
        uint32_t objectDefinitionCount = 0;
        uint32_t bedDefinitionCount = 0;
        uint32_t bedDefinitionChannelCount;
    };

    iabError getIABFrameInfo(std::istream *iInputStream, iabFrameInfo &oIABFrameInfo);

    void showError(iabError error);

    iabError reassembleIAB(std::istream *iInputStream, std::vector<char> &oOutputBuffer, uint32_t &oOutputLength);

    void copyPreambleValue(std::istream *iIMFBuffer, std::vector<char> &ioOutputBuffer, uint32_t &ioOutputLength);

    int convertFrameRate(IABFrameRateType iFrameRate);

    int convertSampleRate(IABSampleRateType iSampleRate);

    int convertBitDepth(IABBitDepthType iBitDepth);
}

#endif //CINEIA_CLI_CINEIA_H
