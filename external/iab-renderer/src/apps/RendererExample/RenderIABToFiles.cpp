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
#include "common/IABConstants.h"
#include "IABUtilities.h"
#include <limits.h>

#if __linux__ || __APPLE__
// For getTimeMS()
#include <sys/time.h>

// returns time in ms
unsigned long getTimeMS()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (unsigned long)((tv.tv_sec % (60 * 60 * 24)) * 1000 + (tv.tv_usec / 1000));
}
#else
unsigned long getTimeMS()
{
    return 0UL;
}
#endif

#define     RENDERER_SAMPLERATE     48000

// Constructor
RenderIABToFiles::RenderIABToFiles()
{
    inputFile_ = NULL;
    inputFrameCount_ = 0;
    errorCode_ = kIABNoError;
    
    iabRendererinterface_ = NULL;
	rendererConfig_ = NULL;
	iabParser_ = NULL;
    outBuffer_ = NULL;
    outPointers_ = NULL;
    outputChannelCount_ = 0;
    maxOutputSampleCount_ = 0;
    iabFrameSampleCount_ = 0;

#ifdef MT_RENDERER_ENABLED
	iabRendererMTinterface_ = NULL;
#endif
}

// Destructor
RenderIABToFiles::~RenderIABToFiles()
{
    // Close input and output files
    CloseInputOutputFiles();

    if (outBuffer_)
    {
        delete [] outBuffer_;
    }
    
    if (outPointers_)
    {
        delete [] outPointers_;
    }
    
    if (iabRendererinterface_)
    {
        IABRendererInterface::Delete(iabRendererinterface_);
    }

#ifdef MT_RENDERER_ENABLED
	if (iabRendererMTinterface_)
	{
		IABRendererMTInterface::Delete(iabRendererMTinterface_);
	}
#endif
	if (rendererConfig_)
    {
        delete rendererConfig_;
    }
    
    if (iabParser_)
    {
        IABParserInterface::Delete(iabParser_);
    }
    	
}
bool RenderIABToFiles::CreateOutputFilesForSpeakers(uint32_t iSampleRate, const std::map<std::string, int32_t> iSpeakerToOutputIndexMap)
{
    bool createFileError = false;;
    
    uint32_t nWaveChannels = MONO_CHANNEL;
    uint32_t nWaveBytesPerSample = BITWIDTH_3BYTES;		// 24-bit sample width
    
    // Construct file name stem
    std::ostringstream oStrStream;
    std::string wavFileNameStem;
    oStrStream << outputPath_.c_str() << SEPARATOR << "Output-";
    wavFileNameStem = oStrStream.str();
    
    // iSpeakerToOutputIndexMap contains the short name and corresponding renderer output index of each physical speaker in the configuration.
    // Use this map information to link each output file wav writer to the corresonding renderer output channel and save in outputFileMap_
    // outputFileMap_ enables the application to access correct rendered channel to write samples to the corresponding wav file.
    
    outputFileMap_.clear();
    
    std::map<std::string, int32_t>::const_iterator iter;
    
    // Create one mono wav file for each physical output speaker/channel in the configuration
    for (iter = iSpeakerToOutputIndexMap.begin(); iter != iSpeakerToOutputIndexMap.end(); iter++)
    {
        OutputFileInfo outputFileInfo;
        
        // Create wav writer for the output channel
        
        WAVFile *pWav;

        oStrStream.str("");
        oStrStream << wavFileNameStem << iter->first << ".wav";     // add speaker name as file suffix and add file extension
        
        pWav = new WAVFile();
        pWav->pFile = new std::fstream(oStrStream.str().c_str(), std::ios::in | std::ios::out | std::ios::trunc | std::ios::binary);
        
        if (! pWav->pFile->good())
        {
            createFileError = true;
            delete pWav;
            break;
        }
        
        // construct header
        
#if defined(BROADCAST_WAVE)
        
        std::string originatorString;
        std::ostringstream originatorStringStream;
        
        originatorStringStream << "Xperi ProAudio IAB-renderer-example";
        originatorString = originatorStringStream.str();
        
        strncpy(pWav->bext_data.Originator, originatorString.c_str(), sizeof(pWav->bext_data.Originator));
        
        std::string originatorRefString;
        std::ostringstream originatorRefStringStream;
        
        originatorRefStringStream << "Xperi ProAudio IAB-renderer-example v1.0.0.0";
        
        originatorRefString = originatorRefStringStream.str();
        
        strncpy(pWav->bext_data.OriginatorReference, originatorRefString.c_str(), sizeof(pWav->bext_data.OriginatorReference));
        
#endif						//if defined(BROADCAST_WAVE)
        
        pWav->fileSize = WAV_RIFF_HEADER_CHUNKSIZE;
        pWav->header.fileSize = (uint32_t) WAV_RIFF_HEADER_CHUNKSIZE;
        pWav->dataSize = 0;
        pWav->header.dataChunkSize = 0;
        pWav->header.fmtChunkSize = WAV_FMT_CHUNKSIZE;
        pWav->header.wFormatTag = WAVE_FORMAT_PCM;
        pWav->header.nChannels = nWaveChannels;
        pWav->header.nSamplePerSec = iSampleRate;
        pWav->header.nAvgBytesPerSec = nWaveChannels * iSampleRate * nWaveBytesPerSample;
        pWav->header.nBlockAlign = nWaveChannels * nWaveBytesPerSample;
        pWav->header.nBitsPerSample = nWaveBytesPerSample * 8;
        pWav->header.cbSize = 0;
        pWav->header.wValidBitsPerSample = nWaveBytesPerSample * 8;
        pWav->header.dwChannelMask = 0;
        
        pWav->writeWavHeader();
        
        // A single buffer block is used to save all rendered channels and they are arranged in the order as they appear in the config file.
        // iter->second is the speaker/channel's index indicating its position in the renderer output block.
        
        // Save to output file info map
        outputFileInfo.outputWav_ = pWav;
        outputFileInfo.channelName_ = iter->first;
        outputFileMap_[iter->second] = outputFileInfo;
    }
    
    if (createFileError)
    {
        std::cerr << "!Error in creating output wav files." << std::endl;
        std::cerr << "Check the output folder specified with -o exists and has write permission." << std::endl;
        std::cerr << "Check that the disk is not full." << std::endl;
        return false;
    }

    // Check wav file count against IABRenderer output channel count for consistency
    if (outputChannelCount_ != outputFileMap_.size())
    {
        std::cerr << "Number of wav file does not match number of output channels reported by IABRenderer instance." << std::endl;
        return false;
    }
    
    // Check output index for consistency
    std::map<int32_t, OutputFileInfo>::iterator iterOutputFile;
    int32_t highestOutputIndex = static_cast<int32_t>(outputFileMap_.size()) - 1;
    for (iterOutputFile = outputFileMap_.begin(); iterOutputFile != outputFileMap_.end(); iterOutputFile++)
    {
        if (iterOutputFile->first > highestOutputIndex)
        {
            std::cerr << "Config file processing internal error: Found invalid speaker output index while setting up wav files." << std::endl;
            return false;
        }
    }
    
    return true;
}

iabError RenderIABToFiles::WriteRendererOutputToFiles()
{
    int32_t *samples;
    int32_t *ptrSample;
    
    // Note that WavIO class method writeSamplesToFile() used to write samples to wav file is designed to handle
    // multi-channel file and expect an array of pointers to the audio sample buffers
    // For this application, we use mono wav file, so the sample pointer array has just one pointer
    
    int32_t *data[1];
    float sample;
    
    samples = new int32_t[iabFrameSampleCount_];
    std::map<int32_t, OutputFileInfo>::iterator iterOutputFile;
    int32_t rendererOutputIndex = 0;
    
    // Write each output channel to its corresponding wav file
    for (iterOutputFile = outputFileMap_.begin(); iterOutputFile != outputFileMap_.end(); iterOutputFile++)
    {
        ptrSample = samples;
        rendererOutputIndex = iterOutputFile->first;
        WAVFile *pWav = iterOutputFile->second.outputWav_;
        
        for(uint32_t k = 0; k < (uint32_t)iabFrameSampleCount_; k++)
        {
            // Renderer output is in floating-point and could exceed +/- 1.0 range
            // WAVFile class accepts 32-bit integer samples as input and writes to wav as 24-bit data
            // Scale renderer output to 32-bit integer range and apply limit
            
            sample = outPointers_[rendererOutputIndex][k] * kInt32BitMaxValue;
            
            // Apply rounding
            if (sample >= 0.0f)
            {
                sample += 0.5;
            }
            else
            {
                sample -= 0.5;
            }
            
            if (sample > kInt32BitMaxValue)
            {
                *ptrSample++ = INT_MAX;
            }
            else if (sample < kInt32BitMinValue)
            {
                *ptrSample++ = INT_MIN;
            }
            else
            {
                *ptrSample++ = (int32_t) sample;
            }
        }
        
        // Update wav header parameters
        pWav->fileSize += iabFrameSampleCount_ * 3;	// wav file is 24-bit
        pWav->dataSize += iabFrameSampleCount_ * 3;
        
        pWav->header.fileSize = (int32_t) pWav->fileSize;
        pWav->header.dataChunkSize = (int32_t) pWav->dataSize;
        pWav->updateRIFFHeader();
        
        data[0] = samples;
        
        // Write audio samples to file
        if( pWav->writeSamplesToFile(1, data, iabFrameSampleCount_) != 0 )
        {
            delete [] samples;
            return kIABMemoryError;
        }
    }
    
    delete [] samples;
    
    return kIABNoError;
    
}

bool RenderIABToFiles::CreateAndInitializeRenderer(std::string iRendererConfigFilePath, bool iEnableMT, uint32_t iThreadPoolSize)
{
    
    // Create the renderer configuration
    
    // Step 1: create a stream on the config file and read contents into temporary buffer
    
    std::ifstream configFile(iRendererConfigFilePath.c_str(), std::ios::in | std::ios::binary);
    
    if (!configFile.good())
    {
        std::cerr << "!Error in opening renderer configuration file." << std::endl;
        return false;
    }
    
    std::string contents;
    configFile.seekg(0, std::ios::end);
    contents.resize((uint32_t)configFile.tellg());
    configFile.seekg(0, std::ios::beg);
    configFile.read(&contents[0], contents.size());
    configFile.close();
    
    
    // Step 2: Use the renderer configuration utility class to create a configuration from the file contents
    
    rendererConfig_ = RenderUtils::IRendererConfigurationFile::FromBuffer((char*) contents.c_str());

    if (rendererConfig_ == NULL)
    {
        std::cerr << "Problem parsing renderer configuration file." << std::endl;
        return false;
    }    
    
    // Step 3: Instantiate an IABRenderer with the configuration and setup IABRenderer data structures

#ifdef MT_RENDERER_ENABLED

	if (!iEnableMT)
	{
		iabRendererinterface_ = IABRendererInterface::Create(*rendererConfig_);

    	std::cout << "NOT using multi-thread rendering." << std::endl << std::endl;

		// Get output channel count from IAB renderer instance
		outputChannelCount_ = iabRendererinterface_->GetOutputChannelCount();

		// Get maximum sample count (per channel) from IAB renderer instance, for buffer allocation
		maxOutputSampleCount_ = iabRendererinterface_->GetMaxOutputSampleCount();

		// Make sure MT instance is not created
		iabRendererMTinterface_ = NULL;
	}
	else
	{
		iabRendererMTinterface_ = IABRendererMTInterface::Create(*rendererConfig_, iThreadPoolSize);

    	std::cout << "Using multi-thread rendering per user input." << std::endl;
    	std::cout << "For information: the host system has " << iabRendererMTinterface_->GetProcessorCoreNumber() << " processors available. " << std::endl << std::endl;
    	std::cout << "Thread pool size of " << iThreadPoolSize << " used." << std::endl << std::endl;

		// Get output channel count from MT IAB renderer instance
		outputChannelCount_ = iabRendererMTinterface_->GetOutputChannelCount();

		// Get maximum sample count (per channel) from MT IAB renderer instance, for buffer allocation
		maxOutputSampleCount_ = iabRendererMTinterface_->GetMaxOutputSampleCount();

		// Make sure non-MT instance is not created
		iabRendererinterface_ = NULL;
	}

#else

	iabRendererinterface_ = IABRendererInterface::Create(*rendererConfig_);

	// Get output channel count from IAB renderer instance
	outputChannelCount_ = iabRendererinterface_->GetOutputChannelCount();

	// Get maximum sample count (per channel) from IAB renderer instance, for buffer allocation
	maxOutputSampleCount_ = iabRendererinterface_->GetMaxOutputSampleCount();

#endif

    // Allocate output buffers for each rendered output channel with maximum sample per channel count supported by the renderer
    // Actual number of samples per channel depends on the IAB bitstream frame rate and sample rate and is
    // less than or equal to maxOutputSampleCount_
    
    outBuffer_ = new float[outputChannelCount_ * maxOutputSampleCount_];
    outPointers_ = new float*[outputChannelCount_];
    
    // Set up output buffer pointer array for passing to IAB renderer
    for(uint32_t i = 0; i < outputChannelCount_; i++)
    {
        outPointers_[i] = outBuffer_ + i * maxOutputSampleCount_;
    }
    
    // Step 4: Create output wav files
    
    // Get speaker to renderer output index map from the configuration
    std::map<std::string, int32_t> speakerToOutputIndexMap = rendererConfig_->GetSpeakerNameToOutputIndexMap();
    
    if (0 == speakerToOutputIndexMap.size())
    {
        std::cerr << "Parsing of configuration file did not return a valid speaker list." << std::endl;
        return false;
    }
    
    // Create output wav file, one for each output speaker channel
    if (!CreateOutputFilesForSpeakers(RENDERER_SAMPLERATE, speakerToOutputIndexMap))
    {
        return false;
    }
    
    return true;
}

iabError RenderIABToFiles::OpenInputFile(std::string iInputFileName)
{
    inputFile_ = new std::ifstream(iInputFileName.c_str(), std::ifstream::in | std::ifstream::binary);
    
    if (!inputFile_->good())
    {
        return kIABGeneralError;
    }
    
    return kIABNoError;
}

iabError RenderIABToFiles::CloseInputOutputFiles()
{
    // Close files
    if (inputFile_)
    {
        inputFile_->close();
        delete inputFile_;
        inputFile_ = NULL;
    }
    
    // Close output wav files and delete pointers
    std::map<int32_t, OutputFileInfo>::iterator iter;

    for (iter = outputFileMap_.begin(); iter != outputFileMap_.end(); iter++)
    {
        if (iter->second.outputWav_->pFile)
        {
            iter->second.outputWav_->pFile->close();
            delete iter->second.outputWav_->pFile;
            delete iter->second.outputWav_;
        }
    }
    
    return kIABNoError;
}

int32_t RenderIABToFiles::RenderIABitstreamToFiles(CommandLineParams& iCparams)
{
#ifdef MT_RENDERER_ENABLED
	// *************************************************
	// Added an array of time measurements for multi-threaded IABRenderer dev use
	//

	// Initialise test time variables

    // Using getTimeMS();

    unsigned long appStartTme           = 0;
    unsigned long frameParsingTotal     = 0;
    unsigned long frameRenderingTotal   = 0;
    unsigned long timeStart = 0;        // Working variable to record start time before calling a function to be measured
    unsigned long timeTaken = 0;        // Working variable to record end time after calling a function to be measured

    appStartTme = getTimeMS();
#endif

    // *************************************************
    errorCode_ = kIABNoError;
    
    // ============================================================
    // Loop through each file in frame sequence
    // a) create stream on input file
    // b) Use IAB parser to parse stream into IAB frame
    // c) use IAB renderer to render the parsed IAB frame
    // d) write rendered output to wav files
    //
    
    inputFileStem_ = iCparams.inputFileStem_;
    inputFileExt_ = iCparams.inputFileExt_;
    outputPath_ = iCparams.outputPath_;
    
    bool multiFilesInput = iCparams.multiFilesInput_;
    
    // Create IAB Renderer instance
    if (!CreateAndInitializeRenderer(iCparams.rendererConfigFilePath_, iCparams.enableMT_, iCparams.threadPoolSize_))
    {
        return 1;
    }
    
    bool noError = true;
	uint32_t unallowedFrameSubElementCount = 0;		// To track if, and the number of "unallowed" frame sub-elements
	bool unAllowedWarningIssued = false;

    if (multiFilesInput)
    {
        std::cout << "Processing bitstream frame sequence. This could take several minutes for complex or long bitstreams ........" << std::endl << std::flush;

		// Create IABParser using new API
		iabParser_ = IABParserInterface::Create();

        if (iCparams.ignoreBitStreamVersion_)
        {
            iabParser_->SetParseFailsOnVersionError(false);
        }

		char * inBuffer = NULL;

		while (1)
        {
            // Construct next input file name
            std::stringstream ss;
            ss << inputFileStem_.c_str() << std::setfill('0') << std::setw(6) << inputFrameCount_ << inputFileExt_;
            
            // open input file for processing. Break loop if there is no more input.
            // Alternatively, find out how many input files there are in the current directory and loop through them.
            if (kIABNoError != OpenInputFile(ss.str()))
            {
                if (inputFrameCount_ == 0)
                {
                    std::cerr << "!Error in opening file : " << ss.str() << ". Input file name error or missing input file)." << std::endl;
                    noError = false;
                }
                
                break;
            }
        
			// Read whole frame into buffer.
			// In GDC implementation, the buffer content will be read by the application code, likely to use asdcp read frame function
			// or something similar

			// For multi-file input of this tool, any single file contains a single frame.
			// Read in currently open file in entirely.
			inputFile_->seekg(0, inputFile_->end);
			uint32_t fileLength = static_cast<uint32_t>(inputFile_->tellg());
			inputFile_->seekg(0, inputFile_->beg);

			// allocate memory for file:
			if (inBuffer)
			{
				delete [] inBuffer;
			}
			inBuffer = new char[fileLength];

			// read data as a block:
			inputFile_->read(inBuffer, fileLength);

#ifdef MT_RENDERER_ENABLED
            timeStart = getTimeMS();
			// Parse the bitstream into IAB frame
			errorCode_ = iabParser_->ParseIABFrame(inBuffer, fileLength);
            timeTaken = (getTimeMS() - timeStart);
            frameParsingTotal += timeTaken;
#else
			// Parse the bitstream into IAB frame
			errorCode_ = iabParser_->ParseIABFrame(inBuffer, fileLength);
#endif

			// *************************************************
			if ( (kIABNoError != errorCode_) 
                && (errorCode_ != kIABParserMissingPreambleError) 
                && (!iCparams.ignoreBitStreamVersion_  || errorCode_ != kIABParserInvalidVersionNumberError))
            {
                if ((inputFrameCount_ > 0) && (errorCode_ != kIABParserEndOfStreamReached) )
                {
                    noError = false;
                }
                                
                if (errorCode_ == kIABParserInvalidVersionNumberError)
                {
                    std::cerr << "!Error from parser: Illegal Bitstream Version (" 
                              << errorCode_ << ")" << std::endl;
                }
                
                break;
            }
            else if (kIABNoError != errorCode_)
            {
                if (errorCode_ != kIABParserInvalidVersionNumberError)
                {
                    std::cerr << "!Warning from parser: " << errorCode_ << std::endl;
                }
                else
                {
                    std::cerr << "!Warning from parser: Illegal Bitstream Version (" 
                              << errorCode_ << ")" << std::endl;
                }
            }

			// Get cumulative unallowed frame sub element count encontered 
			unallowedFrameSubElementCount += iabParser_->GetUnallowedFrameSubElementCount();

			// Issue one-time warning if unallowed frame sub elements (outside of the "allowed" list
			// as defined in ST2098-2 spec.) are found.
			// unAllowedWarningIssued used to issue one-time warning (or everytime if removed from checking).
			if (!unAllowedWarningIssued && unallowedFrameSubElementCount > 0)
			{
				std::cerr << "!Warning: Unknown element ID found during frame sub-element parsing." << std::endl;
				std::cerr << "Refer to Table 4 of ST2098-2 for allowed frame sub-element ID types." << std::endl;
				std::cerr << "Note: Unknown frame sub-elements are skipped during parsing." << std::endl;
				std::cerr << "This is a one-time warning message. No further warnings will be issued during subsequent parsing." << std::endl;

				unAllowedWarningIssued = true;
			}
            
            if (inputFile_)
            {
                // For multi-files input, finish with current input file, close it
                inputFile_->close();
                delete inputFile_;
                inputFile_ = NULL;
            }
            
            const IABFrameInterface *frameInterface = NULL;
            
            if (kIABNoError != iabParser_->GetIABFrame(frameInterface) || frameInterface == NULL)
            {
                noError = false;
                break;
            }
            
            IABRenderedOutputSampleCountType renderedOutputSampleCount = 0;
            
            // Derive frame sample count from frame frame rate
            IABFrameRateType frameRate;
            IABSampleRateType sampleRate;
            
            frameInterface->GetFrameRate(frameRate);
            frameInterface->GetSampleRate(sampleRate);
            
            iabFrameSampleCount_ = GetIABNumFrameSamples(frameRate, sampleRate);
            
            if (iabFrameSampleCount_ > maxOutputSampleCount_)
            {
                noError = false;
                break;
            }
            
			// SDK v1.0 IAB renderer is constrained to render 96k to 48k output only.
			// iabFrameSampleCount_ / 2
			if (sampleRate == kIABSampleRate_96000Hz)
			{
				iabFrameSampleCount_ >>= 1;
			}

			iabError ec;

#ifdef MT_RENDERER_ENABLED
            timeStart = getTimeMS();
			if (!iCparams.enableMT_)
			{
				ec = iabRendererinterface_->RenderIABFrame(*frameInterface, outPointers_, outputChannelCount_, iabFrameSampleCount_, renderedOutputSampleCount);
			}
			else
			{
				ec = iabRendererMTinterface_->RenderIABFrame(*frameInterface, outPointers_, outputChannelCount_, iabFrameSampleCount_);
			}
            timeTaken = (getTimeMS() - timeStart);
            frameRenderingTotal += timeTaken;
#else
			ec = iabRendererinterface_->RenderIABFrame(*frameInterface, outPointers_, outputChannelCount_, iabFrameSampleCount_, renderedOutputSampleCount);
#endif

            if (IsRendererError(ec))
            {
                noError = false;
                break;
            }
            
            // write output to wav files
            
            if (kIABNoError != WriteRendererOutputToFiles())
            {
                noError = false;
                break;
            }
            
			inputFrameCount_++;
            
            // Display progress every 50 frames
            if ((inputFrameCount_ % 50) == 0)
            {
                std::cout << "Frames processed: " << inputFrameCount_ << std::endl << std::flush;
            }
        }

		if (inBuffer)
		{
			// Error condition could leave this buffer allocated
			delete [] inBuffer;
			inBuffer = NULL;
		}

		if (iabParser_)
		{
			// Finished with IAB parser, delete
			IABParserInterface::Delete(iabParser_);
			iabParser_ = NULL;
		}

		if (inputFile_)
		{
			// close any file that is still open. This could happen when exiting early on errors.
			inputFile_->close();
			delete inputFile_;
			inputFile_ = NULL;
		}
	}
    else    // not multi-files input
    {
        std::cout << "Processing the input file. This could take several minutes for complex or long bitstreams ........" << std::endl << std::flush;

        while (1)
        {
            if (inputFrameCount_ > 0)
            {
                if (inputFile_->eof())
                {
                    // Finished processing
                    break;
                }
            }
            else
            {
                // First frame, open the input file for the Parser to use
                std::string inputFile = inputFileStem_ + inputFileExt_;
                if (kIABNoError != OpenInputFile(inputFile))
                {
                    std::cerr << "!Error in opening file : " << inputFile << ". Input file name error or missing input file)." << std::endl;
                    noError = false;
                    break;
                }
            }
            
            if (inputFrameCount_ == 0)
            {
                if (iabParser_)
                {
                    IABParserInterface::Delete(iabParser_);
                }
                
                // Create an IAB Parser instance to process current file
                iabParser_ = IABParserInterface::Create(inputFile_);

                if (iCparams.ignoreBitStreamVersion_)
                {
                    iabParser_->SetParseFailsOnVersionError(false);
                }
            }

#ifdef MT_RENDERER_ENABLED
            timeStart = getTimeMS();
			// Parse the bitstream into IAB frame
			errorCode_ = iabParser_->ParseIABFrame();
            timeTaken = (getTimeMS() - timeStart);
            frameParsingTotal += timeTaken;
#else
			// Parse the bitstream into IAB frame
			errorCode_ = iabParser_->ParseIABFrame();
#endif
			// *************************************************

			// Get cumulative unallowed frame sub element count encontered
			// Accumulate in multi-file mode
			unallowedFrameSubElementCount = iabParser_->GetUnallowedFrameSubElementCount();

			// Issue one-time warning if unallowed frame sub elements (outside of the "allowed" list
			// as defined in ST2098-2 spec.) are found.
			// unAllowedWarningIssued used to issue one-time warning (or everytime if removed from checking).
			if (!unAllowedWarningIssued && unallowedFrameSubElementCount > 0)
			{
				std::cerr << "!Warning: Unknown element ID found during frame sub-element parsing." << std::endl;
				std::cerr << "Refer to Table 4 of ST2098-2 for allowed frame sub-element ID types." << std::endl;
				std::cerr << "Note: Unknown frame sub-elements are skipped during parsing." << std::endl;
				std::cerr << "This is a one-time warning message. No further warnings will be issued during subsequent parsing." << std::endl;

				unAllowedWarningIssued = true;
			}

			if ( (kIABNoError != errorCode_) 
                && (errorCode_ != kIABParserMissingPreambleError) 
                && (!iCparams.ignoreBitStreamVersion_  || errorCode_ != kIABParserInvalidVersionNumberError))
            {
                if ((inputFrameCount_ > 0) && (errorCode_ != kIABParserEndOfStreamReached) )
                {
                    noError = false;
                }

                if (errorCode_ == kIABParserInvalidVersionNumberError)
                {
                    std::cerr << "!Error from parser: Illegal Bitstream Version (" 
                              << errorCode_ << ")" << std::endl;
                }

                break;
            }
            else if (kIABNoError != errorCode_)
            {
                if (errorCode_ != kIABParserInvalidVersionNumberError)
                {
                    std::cerr << "!Warning from parser: " << errorCode_ << std::endl;
                }
                else
                {
                    std::cerr << "!Warning from parser: Illegal Bitstream Version (" 
                              << errorCode_ << ")" << std::endl;
                }
            }
            
            const IABFrameInterface *frameInterface = NULL;
            
            if (kIABNoError != iabParser_->GetIABFrame(frameInterface) || frameInterface == NULL)
            {
                noError = false;
                break;
            }
            
            IABRenderedOutputSampleCountType renderedOutputSampleCount = 0;
            
            // Derive frame sample count from frame frame rate
            IABFrameRateType frameRate;
            IABSampleRateType sampleRate;
            
            frameInterface->GetFrameRate(frameRate);
            frameInterface->GetSampleRate(sampleRate);
            
            iabFrameSampleCount_ = GetIABNumFrameSamples(frameRate, sampleRate);
            
            if (iabFrameSampleCount_ > maxOutputSampleCount_)
            {
                noError = false;
                break;
            }

			// SDK v1.0 IAB renderer is constrained to render 96k to 48k output only.
			// iabFrameSampleCount_ / 2
			if (sampleRate == kIABSampleRate_96000Hz)
			{
				iabFrameSampleCount_ >>= 1;
			}

			iabError ec;

#ifdef MT_RENDERER_ENABLED
            timeStart = getTimeMS();

			// *************************************************
			if (!iCparams.enableMT_)
			{
				ec = iabRendererinterface_->RenderIABFrame(*frameInterface, outPointers_, outputChannelCount_, iabFrameSampleCount_, renderedOutputSampleCount);
			}
			else
			{
				ec = iabRendererMTinterface_->RenderIABFrame(*frameInterface, outPointers_, outputChannelCount_, iabFrameSampleCount_);
			}

            timeTaken = (getTimeMS() - timeStart);
            frameRenderingTotal += timeTaken;
#else
			ec = iabRendererinterface_->RenderIABFrame(*frameInterface, outPointers_, outputChannelCount_, iabFrameSampleCount_, renderedOutputSampleCount);
#endif
			// *************************************************

            if (IsRendererError(ec))
            {
                noError = false;
                break;
            }
            
            // write output to wav files
            
            if (kIABNoError != WriteRendererOutputToFiles())
            {
                noError = false;
                break;
            }
            
            inputFrameCount_++;
            
            // Display progress every 50 frames
            if ((inputFrameCount_ % 50) == 0)
            {
                std::cout << "Frames processed: " << inputFrameCount_ << std::endl << std::flush;
#ifdef MT_RENDERER_ENABLED
                std::cout << "Total parsing time so far: " << (frameParsingTotal/1000.0) << std::endl << std::flush;
                std::cout << "Total rendering time so far: " << (frameRenderingTotal/1000.0) << std::endl << std::flush;
#endif
            }
        }
    }

#ifdef MT_RENDERER_ENABLED
    unsigned long totalAppTime = getTimeMS() - appStartTme;

    // Show total frames processed
    std::cout << "Total frames processed: " << inputFrameCount_ << std::endl << std::endl;
	std::cout << "Total processing time: " << (totalAppTime/1000.0) << std::endl << std::endl;
	std::cout << "Total parsing time: " << (frameParsingTotal/1000.0) << std::endl;
	std::cout << "Total rendering time: " << (frameRenderingTotal/1000.0) << std::endl << std::endl;
#else
	// Show total frames processed
	std::cout << "Total frames processed: " << inputFrameCount_ << std::endl << std::flush;
#endif

    IssueRendererWarnings();

    if (!noError)
    {
        return 1;
    }
    
    return 0;
}

bool RenderIABToFiles::IsRendererError(iabError errorCode)
{
    switch (errorCode)
    {
        case kIABNoError:
            return false;
        case kIABRendererNoLFEInConfigForBedLFEWarning:
        case kIABRendererNoLFEInConfigForRemapLFEWarning:
        case kIABRendererEmptyObjectZoneWarning:
        {
            // Keep track of issued warnings
            std::map<iabError, int>::iterator it = issuedWarnings_.find(errorCode);
            if (it == issuedWarnings_.end())
            {
                issuedWarnings_[errorCode] = 1;
            }
            else
            {
                ++it->second;
            }
            return false;
        }
        default:
            errorCode_ = errorCode;
            return true;
    }
}

void RenderIABToFiles::IssueRendererWarnings() const
{
    std::map<iabError, int>::const_iterator it;
    for (it = issuedWarnings_.begin();
         it != issuedWarnings_.end();
         ++it)
    {
        std::cerr << "Warning from renderer, Code " << it->first
                  << " seen " << it->second << " times\n";
    }
}
