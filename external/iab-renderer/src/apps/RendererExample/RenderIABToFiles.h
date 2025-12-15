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

#ifndef RENDERIABTOFILES_H_
#define RENDERIABTOFILES_H_

#include <fstream>
#include <iterator>
#include <iomanip>
#include <map>
#include <iostream>
#include <vector>
#include <sstream>
#include <algorithm>
#include <stdint.h>

#include "IABElementsAPI.h"
#include "IABRendererAPI.h"
#include "IABParserAPI.h"

#include "waveutils/WaveIO.h"

#if (__GNUC__)
#include <errno.h>
#endif

#ifdef _WIN32
#include <direct.h>		// for _mkdir
#define SEPARATOR ("\\")
#ifndef S_IRWXU
#define S_IRWXU		0x700	// note that S_IRWXU is not defined in _WIN32, also not used for _mkdir call, include here as a dummy for compilation
#endif
#define MKDIR(fn, mode) _mkdir(fn)
#else
#include <sys/stat.h> // for mkdir
#define SEPARATOR ("/")
#define MKDIR(fn, mode) mkdir(fn, mode)
#endif

using namespace SMPTE::ImmersiveAudioBitstream;

// Structure for command line options
struct CommandLineParams
{
    CommandLineParams()
    {
        inputFileStem_ = "";
        inputFileExt_ = "";
        outputPath_ = "";
        rendererConfigFilePath_ = "";
        multiFilesInput_ = true;
        showUsage_ = false;
        showExtendedInfo_ = false;

		enableMT_ = false;
		threadPoolSize_ = 4;				// default to 4 threads

        ignoreBitStreamVersion_ = false;
    }
    
    std::string inputFileStem_;             // Stores input file name stem, including relative or full path
    std::string inputFileExt_;              // Stores IAB bitstream file extension
    std::string outputPath_;
	std::string rendererConfigFilePath_;    // Stores renderer configuration file, including relative or full path
    bool multiFilesInput_;                  // indicates IA bitstream is split into multiple files, one IAB frame per file
    bool showUsage_;                        // true to show application usage
    bool showExtendedInfo_;                 // true to show extended help information

	// Options for multi-threaded version, if supported
	bool enableMT_;                         // true to switch to IABRendererMT lib
	uint32_t threadPoolSize_;				// Size of threadpool for MT. Effective only when enableMT_ is enabled

    // Dev control to allow parsing of bitstreams with invalid versions
    bool ignoreBitStreamVersion_;           // When set to true the app will attempt to parse bitstreams with
                                            // invalid version numbers.
};

/**
 *
 * Class to render SMPTE Immersive Audio bitstream to wav files.
 * The class uses an IAB Parser to parse the bitstream and an IAB Renderer to render the parsed IAB frame to the specified target configuration.
 * The rendered output audio samples are then written to mono wav files.
 */

class RenderIABToFiles
{
    
public:
    
    // Constructor
    RenderIABToFiles();
    
    // Destructor
    ~RenderIABToFiles();
    
    /**
     * Renders an IA bitstream to a specified configuration and write audio samples to mono wav files
     * @param[in] iCparams commandline parameter data structure providing information on input path, output path and target configuration
     * @returns 0 if no error and returns 1 to indicate an error has occurred.     
     */
    int32_t RenderIABitstreamToFiles(CommandLineParams& iCparams);

    /**
     * Looks at error code from the renderer, handles warnings.
     *
     * For a warning, keeps track of how many times the warning occurred.
     *
     * @returns True if it is an error, false for a warning
     */
    bool IsRendererError(iabError errorCode);

    /**
     * Issues any warnings from the renderer to std::cerr
     */
    void IssueRendererWarnings() const;

    iabError errorCode() const { return errorCode_; }
    
private:

    typedef struct OutputFileInfo
    {
        OutputFileInfo()
        {
            outputWav_ = NULL;
            channelName_ = "";            
        };
        
        WAVFile         *outputWav_;
        std::string     channelName_;
        
    } OutputFileInfo;

    // Opens the specified file, creates a stream and assigns pointer to file_
    iabError    OpenInputFile(std::string iInputFileName);
    
    // Closes input and output files
    iabError    CloseInputOutputFiles();
    
    // Creates output mono wav file
    bool        CreateOutputFilesForSpeakers(uint32_t iSampleRate, const std::map<std::string, int32_t> iSpeakerToOutputIndexMap);
    
    // Creates an IAB Renderer and initialises with the specified renderer configuration file
    bool        CreateAndInitializeRenderer(std::string iRendererConfigFilePath, bool iEnableMT, uint32_t iThreadPoolSize);

    // Writes a frame of rendered audio samples to wav files
    iabError    WriteRendererOutputToFiles();
    
    // ******************
    // Class data members
    // ******************
    
    // Pointer to IAB parser
    IABParserInterface*     iabParser_;
    
    // Input file name stem for constructing input file to open for processing
    std::string             inputFileStem_;
    
    // Input file extension for constructing input file to open for processing
    std::string             inputFileExt_;
    
    // Path to the output wav files
    std::string             outputPath_;
    
    // Pointer to the current input file to be processed.
    std::ifstream 			*inputFile_;
    
    // Rendering related
    IABRendererInterface                *iabRendererinterface_;     // Pointer to IAB renderer

#if __linux__ || __APPLE__
	IABRendererMTInterface              *iabRendererMTinterface_;   // Pointer to multi-threaded IAB renderer
#endif

	RenderUtils::IRendererConfiguration *rendererConfig_;           // Pointer to the renderer configuration file
    IABRenderedOutputChannelCountType   outputChannelCount_;        // Number of rendered output channel
    
    // Maximum number of samples per channel supported by the IAB Renderer. It is used for memory allocation purpose to ensure
    // RenderIABToFiles has allocated enough buffer to handle the maximum size.
    // Actual number of rendered samples depends on frame rate and sample rate of the parsed IAB frame and will be
    // less than or equal to the maximum value. See iabFrameSampleCount_ below.
    IABRenderedOutputSampleCountType    maxOutputSampleCount_;
    
    // Number of samples per rendered output channel for current frame, derived from frame rate and sample rate of current parsed IAB frame
    // In practice, the entire bitstream is expected to have fixed sample rate and frame rate, so frame sample count should stay
    // the same throughtout
    IABRenderedOutputSampleCountType    iabFrameSampleCount_;

    // This map provides output file information including channel name, rendered output index and wav writter
    // Map key is the rendered output channel index. Map value is a struture containing channel name and wav writer pointer
    // This map is created from configuration speaker list during IABRenderer initialisation and is used to access the correct
    // channel when writing rendered samples to wav files.
    std::map<int32_t, OutputFileInfo>      outputFileMap_;
    
    // Keeps track of issued warnings so they are only issued once
    std::map<iabError, int> issuedWarnings_;

    // Rendered output buffer. This buffer block contains all rendered output channels, arranged in a channel by channel basis, i.e.
    // non-interleaving. Channel order is same as order in oWavFiles_
    float                   *outBuffer_;
    
    // Array of output buffer pointers. Each pointer points to the start of a rendered output channel.
    // Channel order is same as order in oWavFiles_
    float                   **outPointers_;
    
    // Input frame count to keep track of number of IAB frames processed, also used for progress update
    uint32_t                inputFrameCount_;

    // SDK error code, if error occurred
    iabError                errorCode_;
};

#endif /* RENDERIABTOFILES_H_ */
