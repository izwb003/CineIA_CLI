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

#include <iostream>
#include <iomanip>

#include <AS_02_IAB.h>
#include <AS_DCP.h>

#include "cineia.h"

// Console color definition
#define NONE         "\033[m"
#define RED          "\033[0;32;31m"
#define LIGHT_RED    "\033[1;31m"
#define GREEN        "\033[0;32;32m"
#define LIGHT_GREEN  "\033[1;32m"
#define BLUE         "\033[0;32;34m"
#define LIGHT_BLUE   "\033[1;34m"
#define DARY_GRAY    "\033[1;30m"
#define CYAN         "\033[0;36m"
#define LIGHT_CYAN   "\033[1;36m"
#define PURPLE       "\033[0;35m"
#define LIGHT_PURPLE "\033[1;35m"
#define BROWN        "\033[0;33m"
#define YELLOW       "\033[1;33m"
#define LIGHT_GRAY   "\033[0;37m"
#define WHITE        "\033[1;37m"

void showProgramInfo() {
    printf("\n\tCineIA_CLI Version %d.%d.%d, %s: ", PROJECT_VERSION_MAJOR, PROJECT_VERSION_MINOR, PROJECT_VERSION_PATCH, __DATE__);
    printf("IMF IAB to Atmos Compatible Cinema IAB converter.\n");
    printf("\t\t\tCopyright (c) 2024 @izwb003, @Shino_Rize, @Connor\n");
    printf("\t\tPowered by IABLib and AS-DCP. Run \"cineia -l\" for more information.\n\n");
    printf(RED" Warning:" NONE);
    printf(" THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS\n"
           " OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,\n"
           " FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE\n"
           " AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER\n"
           " LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,\n"
           " OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE\n"
           " SOFTWARE.\n");
    printf(YELLOW" Note:" NONE);
    printf(" The output of this program cannot represent the product quality of\n"
           " Dolby Laboratories. This program can only be used for UGC content production\n"
           " and cannot be used for professional content production work. For professional\n"
           " content distribution needs, please contact Dolby Laboratories.\n\n");
}

void showHelp() {
    printf("Usage: cineia [-n] [-f] <input file path> <output file path> [-l] [-h]\n\n"
           "<input file path>: Full or relative path to the IMF IAB file for conversion.\n"
           "<output file path>: Full or relative path to the DCP IAB file output.\n\n\n"

           "Option Summary:\n\n"
           "-n,\t--no-copy-preamble\t\tDo not copy the \"PreambleValue\" part from IMF IAB bitstream.\n"
           "\t\t\t\t\tTry using this argument if the output bitstream does not work fine.\n"
           "-f,\t--force-dolby-constraint\tModify the bitstream to ensure that every detail conforms to the Dolby constraint.\n"
           "\t\t\t\t\tTry using this argument if the output bitstream causes error,\n"
           "\t\t\t\t\tbut it may cause the bitstream to not work as expected.\n"
           "-l,\t--show-licenses\t\t\tShow open-source licenses and quit.\n"
           "-h,\t--help\t\t\t\tPrint this help message and quit.\n"
    );
}

void showLicense() {
    printf("\t\t\tOpen Source Licenses\n\n");
    printf("CineIA_CLI makes use of third-party software licensed under open-source licenses:\n\n");
    printf("==============================================================================================\n\n");
    printf("asdcplib (https://github.com/cinecert/asdcplib)\n");
    printf("CineIA_CLI uses software from asdcplib under the asdcplib BSD 3-Clause license:\n\n");
    printf("AS-DCP Lib is Copyright (c) 2003-2012, John Hurst\n"
           "All rights reserved.\n"
           "\n"
           "Redistribution and use in source and binary forms, with or without\n"
           "modification, are permitted provided that the following conditions\n"
           "are met:\n"
           "1. Redistributions of source code must retain the above copyright\n"
           "   notice, this list of conditions and the following disclaimer.\n"
           "2. Redistributions in binary form must reproduce the above copyright\n"
           "   notice, this list of conditions and the following disclaimer in the\n"
           "   documentation and/or other materials provided with the distribution.\n"
           "3. The name of the author may not be used to endorse or promote products\n"
           "   derived from this software without specific prior written permission.\n"
           "\n"
           "THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR\n"
           "IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES\n"
           "OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.\n"
           "IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,\n"
           "INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT\n"
           "NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,\n"
           "DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY\n"
           "THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT\n"
           "(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF\n"
           "THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.\n"
           "IMF Essence Component (AS-02) implementation:\n"
           "(AS_02.h  AS_02_internal.h  AS_02_JP2K.cpp  AS_02_MXF.cpp  AS_02_PCM.cpp\n"
           " as-02-unwrap.cpp  as-02-wrap.cpp  h__02_Reader.cpp  h__02_Writer.cpp)\n"
           "Copyright (c) 2011-2012, Robert Scheler, Heiko Sparenberg Fraunhofer IIS, John Hurst\n"
           "All rights reserved.\n"
           "\n"
           "Redistribution and use in source and binary forms, with or without\n"
           "modification, are permitted provided that the following conditions\n"
           "are met:\n"
           "1. Redistributions of source code must retain the above copyright\n"
           "   notice, this list of conditions and the following disclaimer.\n"
           "2. Redistributions in binary form must reproduce the above copyright\n"
           "   notice, this list of conditions and the following disclaimer in the\n"
           "   documentation and/or other materials provided with the distribution.\n"
           "3. The name of the author may not be used to endorse or promote products\n"
           "   derived from this software without specific prior written permission.\n"
           "\n"
           "THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR\n"
           "IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES\n"
           "OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.\n"
           "IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,\n"
           "INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT\n"
           "NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,\n"
           "DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY\n"
           "THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT\n"
           "(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF\n"
           "THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.\n\n");
    printf("==============================================================================================\n\n");
    printf("IABLib (https://github.com/DTSProAudio/iab-renderer)\n");
    printf("CineIA_CLI uses software from IABLib under the BSD 3-Clause license:\n\n");
    printf("BSD 3-Clause License\n"
           "\n"
           "Copyright (c) 2023, DTSProAudio\n"
           "\n"
           "Redistribution and use in source and binary forms, with or without\n"
           "modification, are permitted provided that the following conditions are met:\n"
           "\n"
           "1. Redistributions of source code must retain the above copyright notice, this\n"
           "   list of conditions and the following disclaimer.\n"
           "\n"
           "2. Redistributions in binary form must reproduce the above copyright notice,\n"
           "   this list of conditions and the following disclaimer in the documentation\n"
           "   and/or other materials provided with the distribution.\n"
           "\n"
           "3. Neither the name of the copyright holder nor the names of its\n"
           "   contributors may be used to endorse or promote products derived from\n"
           "   this software without specific prior written permission.\n"
           "\n"
           "THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS \"AS IS\"\n"
           "AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE\n"
           "IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE\n"
           "DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE\n"
           "FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL\n"
           "DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR\n"
           "SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER\n"
           "CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,\n"
           "OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE\n"
           "OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.\n\n");
}

struct cmdSettings {
    bool copyPreambleValue = true;
    bool forceDolbyConstraint = false;
    std::string inputFileName;
    std::string outputFileName;
};

bool parseCommandLineOptions(int argc, const char* argv[], cmdSettings &settings) {
    if(argc == 1) {
        printf(RED" Error:" NONE);
        printf(" No command line parameters were read.\n");
        printf(" \tRun \"cineia -h\" or \"cineia --help\" for help.\n");
        return false;
    }

    for(int parseCmd = 1; parseCmd < argc; parseCmd ++) {
        if(std::string(argv[parseCmd]) == "-h" || std::string(argv[parseCmd]) == "--help") {
            showHelp();
            return false;
        }
        else if(std::string(argv[parseCmd]) == "-l" || std::string(argv[parseCmd]) == "--show-licenses") {
            showLicense();
            return false;
        }
        else if(std::string(argv[parseCmd]) == "-n" || std::string(argv[parseCmd]) == "--no-copy-preamble")
            settings.copyPreambleValue = false;
        else if(std::string(argv[parseCmd]) == "-f" || std::string(argv[parseCmd]) == "--force-dolby-constraint")
            settings.forceDolbyConstraint = true;
        else if(settings.inputFileName.empty())
            settings.inputFileName = std::string(argv[parseCmd]);
        else if(!settings.inputFileName.empty() && settings.outputFileName.empty())
            settings.outputFileName = std::string(argv[parseCmd]);
        else {
            printf(RED" Error:" NONE);
            printf(" Invalid argument: %s.\n", argv[parseCmd]);
            printf(" \tRun \"cineia -h\" or \"cineia --help\" for help.\n");
            return false;
        }
    }

    if(settings.inputFileName.empty() || settings.outputFileName.empty()) {
        printf(RED" Error:" NONE);
        printf(" No input or output file specified.\n"
               " \tBoth must be specified simultaneously.\n");
        return false;
    }
    return true;
}

std::string generateDuration(uint32_t frameCount, uint32_t frameRate) {
    uint32_t seconds = frameCount / frameRate;
    uint32_t hours = seconds / 3600;
    uint32_t minutes = (seconds % 3600) / 60;
    uint32_t remainingSeconds = seconds % 60;

    std::stringstream ss;
    ss << std::setfill('0') << std::setw(2) << hours << ":";
    ss << std::setfill('0') << std::setw(2) << minutes << ":";
    ss << std::setfill('0') << std::setw(2) << remainingSeconds;

    return ss.str();
}

int main(int argc, const char* argv[]) {
    // Initialize and parse commands
    showProgramInfo();
    ASDCP::Result_t result = ASDCP::RESULT_OK;
    cmdSettings settings;
    if(!parseCommandLineOptions(argc, argv, settings)) return -1;

    // Begin reading input file

    // Open input file
    printf("Reading input file %s ...  ", settings.inputFileName.c_str());
    AS_02::IAB::MXFReader reader;
    result = reader.OpenRead(settings.inputFileName);
    if(result.Failure()) {
        printf(RED" Error:" NONE);
        printf(" Unable to open input file: %s", settings.inputFileName.c_str());
        return -2;
    }

    // Get file info
    ASDCP::WriterInfo iInfo;
    result = reader.FillWriterInfo(iInfo);
    if(result.Failure()) {
        printf(RED" Error:" NONE);
        printf(" Unable to parse input file info.");
        return -3;
    }

    printf(GREEN"OK\n\n" NONE);

    // Read IMF frames
    AS_02::IAB::MXFReader::Frame iFrame;
    std::stringstream iFrameStream;
    uint32_t frameCount;

    result = reader.GetFrameCount(frameCount);

    // Read the first IABFrame to get information
    CineIA::iabFrameInfo iFrameInfo;
    result = reader.ReadFrame(0, iFrame);
    if(result.Failure()) {
        printf(RED" Error:" NONE);
        printf(" Unable to read IAB frames.");
        return -4;
    }
    iFrameStream.write((const char*)iFrame.second, iFrame.first);

    std::ofstream test("test.bin", std::ios::binary);
    test.write((char*)iFrame.second, iFrame.first);

    if(CineIA::getIABFrameInfo(&iFrameStream, iFrameInfo) != kIABNoError) {
        printf(RED" Error:" NONE);
        printf(" Failed to get IAB frames' info. The IABFrame may has error.");
        return -4;
    }

    printf("\t\t\tIAB info\n");
    printf("======================================================================\n");
    printf("\tFrame count\t\t\t%d\n", frameCount);
    printf("\tDuration\t\t\t%s\n", generateDuration(frameCount, (uint32_t)CineIA::convertFrameRate(iFrameInfo.frameRate)).c_str());
    printf("\tFrame rate\t\t\t%dfps\n", CineIA::convertFrameRate(iFrameInfo.frameRate));
    printf("\tAudio sample rate\t\t%dHz\n", CineIA::convertSampleRate(iFrameInfo.sampleRate));
    printf("\tAudio bit depth\t\t\t%dbits\n", CineIA::convertBitDepth(iFrameInfo.bitDepth));
    printf("\tChannels\t\t\t%d\n", iFrameInfo.maxRendered);
    printf("\tBed count\t\t\t%d\n", iFrameInfo.bedDefinitionCount);
    printf("\tBed channel count\t\t%d\n", iFrameInfo.bedDefinitionChannelCount);
    printf("\tObject count\t\t\t%d\n", iFrameInfo.objectDefinitionCount);
    printf("======================================================================\n\n");

    return 0;
}