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

#include "RenderIABToFiles.h"

//========================================================================
void DisplayProgramID(void)
{
	printf("\n\t-----   Xperi ProAudio IABRender Example   %s, %s ------\n\n", __DATE__, __TIME__);
	fflush(stdout);
};

void ShowUsage()
{
    printf("Usage: IAB-renderer-example inputPath -c<ConfigFile> [options]\n\n"
           "========================================================================================================\n\n"
           "Required Arguments:\n\n"
           " inputPath      Path to the SMPTE Immersive Audio Bitstream (IAB) file(s) for rendering.\n"
           "                Bitstream files must have .iab extension.\n"
           "                The application supports single-file or multi-file input.\n"           
           "                Single-file input refers to the case when a single file contains all frames of\n"
           "                the bitstream.\n"
           "                Multi-file input refers to the case of a frame sequence in which the entire sequence\n"
           "                is stored in the same folder. The sequence consists of individual files with the same\n"
           "                name prefix, plus ascending index where each file contains a single IAB frame.\n"
           "                In both cases, the file name, including full or relative path, must be specified.\n"
           "                For multi-file input, the index part of the file name should be omitted.\n"
           " -c             Configuration file for the renderer, must include full or relative path to the file.\n"
           "                There should be no space or tab between -c and the configuration file name.\n\n"
           "========================================================================================================\n\n"
           "options         Represents optional parameters. See summary below. A space or tab must be used to separate\n"
           "                each option.\n\n"
           "Option Summary:\n\n"
           " -s             Specifies single-file input. When -s is not specified, multi-file input will be assumed.\n"
           " -o             Specifies the folder to store output wav files, including full or relative path.\n"
           "                The folder must exist.\n"
           "                When -o is not specified, the output files will be stored in the current working directory.\n"
           "                There is no space between -o and the output folder name.\n"
           "                WARNING: Existing wav files in the output folder with the same name will be over-written.\n"
           " -h, --help     Show this application usage information.\n"
           "                With -h or --help, other command-line parameters are ignored, with the exception of\n"
           "                --ExtraHelp, which will have priority over -h or --help.\n"
           "                Running IAB-renderer-example without parameters will also show this application\n"
           "                usage information.\n"
           " --ExtraHelp    Show extended application help information.\n"
           "                With --ExtraHelp, other command-line parameters are ignored.\n"
           "\n"
           " --IgnoreBitstreamVersion Attempt to parse input bitstreams with invalid versions.\n"
           "                WARNING: This SDK does not support input bitstreams with invalid version numbers\n"
           "                and processing may fail at any time. Use this option at your own risk.\n"
           "\n"
#ifdef MT_RENDERER_ENABLED
           " --MTRender     Use multi-threaded renderer. Default: single-threaded renderer.\n"
           " -t#            Thread pool size. Effective only when --MTRender is specified.\n"
           "                Range of thread pool size: [1, 8], Default to 4.\n"
           "\n"
#else
           "NOTE:  Multi-threaded rendering is not supported on this platform.\n"
           "\n"
#endif
           "\n\n"
           );
}

void ShowExtendedHelpInformation()
{
    printf("This application renders a SMPTE Immersive Audio Bitstream (IAB) to a specified output configuration\n"
           "and writes the audio samples of each rendered output channel to a 48kHz 24-bit mono PCM wav file.\n\n"
           "Naming convention for the wav file is Output-xxx.wav, where xxx is a channel label from the configuration\n"
           "file. For example, if the Left speaker in the configuration file has a label L, then its corresponding\n"
           "wav file will be named Output-L.wav\n\n"
           "Application usage examples:\n\n"
           "Example (1) multi-file input, assumming the current directory contains:\n"
           "    a) IAB-renderer-example application\n"
           "    b) A folder named INPUT with the frame sequence (individual IAB files bitstream_0000000.iab,\n"
           "       bitstream_0000001.iab, etc.)\n"
           "    c) A folder named CONFIG with renderer configuration file, c7.1.4.cfg\n"
           "    d) An empty folder named IAB_OUT\n\n"
           "    The command to render the frame sequence to configuration c7.1.4.cfg would be:\n\n"
           "        IAB-renderer-example INPUT/bitstream_.iab -oIAB_OUT -cCONFIG/c7.1.4.cfg\n\n"
           "Example (2) single-file input, assumming the current directory contains:\n"
           "    a) IAB-renderer-example application\n"
           "    b) A folder named INPUT with IAB file, bitstream.iab\n"
           "    c) A folder named CONFIG with renderer configuration file, c7.1.4.cfg.\n"
           "    d) An empty folder named IAB_OUT\n\n"
           "    The command to render bitstream.iab to configuration c7.1.4.cfg would be:\n\n"
           "        IAB-renderer-example INPUT/bitstream.iab -oIAB_OUT -cCONFIG/c7.1.4.cfg -s\n\n"
           "============================================================================================================\n\n"
           "============================================================================================================\n\n"
           );
    
}

bool ExtractCommandlineParams(int argc, char* argv[], CommandLineParams&  cliParams)
{
    
    std::string tmpString;
    size_t length, iabOffset, atmosOffset, charOffset;
    
    for (int i = 1; i < argc; ++i)
    {
        if (std::string(argv[i]).compare(0,2,"-o") == 0)
        {
            cliParams.outputPath_ = argv[i];
        }
        else if (std::string(argv[i]).compare(0,2,"-c") == 0)
        {
            cliParams.rendererConfigFilePath_ = argv[i];
        }
        else if (std::string(argv[i]).compare(0,2,"-s") == 0)
        {
            cliParams.multiFilesInput_ = false;
        }
        else if ((std::string(argv[i]).compare(0,2,"-h") == 0) || (std::string(argv[i]).compare(0,6,"--help") == 0))
        {
            if (!cliParams.showExtendedInfo_)
            {
                cliParams.showUsage_ = true;
            }
        }
        else if (std::string(argv[i]).compare(0,11,"--ExtraHelp") == 0)
        {
            cliParams.showExtendedInfo_ = true;
            cliParams.showUsage_ = false;
        }
		else if (std::string(argv[i]).compare(0, 10, "--MTRender") == 0)
		{
			cliParams.enableMT_ = true;
		}
		else if (std::string(argv[i]).compare(0, 2, "-t") == 0)
		{
			std::string TPSizeString = argv[i];
			const char* TPCharArray = TPSizeString.c_str();

			// Extract number specified with -l
			if ((TPSizeString.compare(0, 2, "-t") == 0) && (TPSizeString.size() > 2))
			{
				if (TPSizeString.find_first_not_of("0123456789", 2) != std::string::npos)
				{
					std::cerr << "!Error:  Invalid -t option." << std::endl << std::endl;
					return false;
				}

				cliParams.threadPoolSize_ = atoi(TPCharArray + 2);
			}

			// Check range
			if (cliParams.threadPoolSize_ > 8 || cliParams.threadPoolSize_ < 1)
			{
				std::cerr << "!Error: Thread pool size out of range." << std::endl << std::endl;
				return false;
			}
		}
        else if (std::string(argv[i]).compare(0, 24, "--IgnoreBitstreamVersion") == 0)
        {
            cliParams.ignoreBitStreamVersion_ = true;
        }
        else if (cliParams.inputFileStem_.size() == 0)
        {
            cliParams.inputFileStem_ = argv[i];
        }
    }
    
    if (cliParams.showUsage_ || cliParams.showExtendedInfo_)
    {
        // No need to process other parameters
        return true;
    }
    
    if (cliParams.inputFileStem_.size() == 0)
    {
        std::cerr << "!Error: Input file name is not specified." << std::endl << std::endl;
        return false;
    }
    
    tmpString = cliParams.inputFileStem_;
    length = tmpString.size();
    std::transform(tmpString.begin(), tmpString.end(), tmpString.begin(), ::tolower);
    
    iabOffset = tmpString.find(".iab");
    atmosOffset = tmpString.find(".atmos");
    
    // Check if inputFileStem_ ends with .iab or .atmos
    // Note that atmos is not an officially supported extension, but some Xperi internal test streams have this extension
    // offset is the character position of the dot, i.e "."
    if ((iabOffset != (length - 4)) && (atmosOffset != (length - 6)))
    {
        std::cerr << "!Error: No input file found. See usage information below for correct command line syntax." << std::endl;
        return false;
    }
    
    // Save file extension and name stem (without extension)
    if (iabOffset == (length - 4))
    {
        cliParams.inputFileExt_ = ".iab";
        cliParams.inputFileStem_.erase(cliParams.inputFileStem_.begin() + iabOffset, cliParams.inputFileStem_.end());
    }
    else
    {
        cliParams.inputFileExt_ = ".atmos";
        cliParams.inputFileStem_.erase(cliParams.inputFileStem_.begin() + atmosOffset, cliParams.inputFileStem_.end());
    }
    
    if (cliParams.outputPath_.size() != 0)
    {
        // no string after -o
        if (cliParams.outputPath_.size() < 3)
        {
            std::cerr << "!Error: -o parameter is not valid." << std::endl;
            return false;
        }

        cliParams.outputPath_.erase(cliParams.outputPath_.begin(), cliParams.outputPath_.begin()+2);
    }
    else
    {
        std::cerr << "Note: No -o specified. Output files will be saved in the current working directory. " << std::endl << std::endl;
               
        cliParams.outputPath_ = ".";
    }

    // Check configuration
    if (cliParams.rendererConfigFilePath_.size() != 0)
    {
        cliParams.rendererConfigFilePath_.erase(cliParams.rendererConfigFilePath_.begin(), cliParams.rendererConfigFilePath_.begin()+2);
        
        tmpString = cliParams.rendererConfigFilePath_;
        length = tmpString.size();
        std::transform(tmpString.begin(), tmpString.end(), tmpString.begin(), ::tolower);
        
        charOffset = tmpString.find(".cfg");
        
        if (charOffset != length - 4)
        {
            std::cerr << "!Error: Renderer configuration file name does not have a .cfg extension." << std::endl << std::endl;
            return false;
        }
    }
    else
    {
        std::cerr << "Error: No -c specified" << std::endl << std::endl;
        return false;
    }
    
    return true;
}

int main(int argc, char* argv[])
{
    int         error = 0;
    CommandLineParams cParams;
    
	DisplayProgramID();

	if (argc < 2)
    {
        ShowUsage();
        return 1;
	}

    if (!ExtractCommandlineParams(argc, argv, cParams))
    {
        ShowUsage();
        return 1;
    }
    
    if (cParams.showExtendedInfo_)
    {
        ShowExtendedHelpInformation();
        ShowUsage();
        return 0;
    }

    if (cParams.showUsage_)
    {
        ShowUsage();
        return 0;
    }

    // Command line options extracted, ready to start

    // Create a RenderIABToFiles instance to process the bitstream
    RenderIABToFiles *pRenderIAB = new RenderIABToFiles();

    // Render the IA bitstream and write output to wav files
    error = pRenderIAB->RenderIABitstreamToFiles(cParams);

    iabError errorCode = pRenderIAB->errorCode();

	delete pRenderIAB;

    // Check return code for error
    if (0 != error)
    {
        std::cerr
            << std::endl
            << "The application has aborted early with error. Any output files created are invalid and should be discarded.";
		if (errorCode) {
			std::cerr << " (Code " << errorCode << ")";
		}
		std::cerr << std::endl;
        return error;
    }

	return 0;
}

