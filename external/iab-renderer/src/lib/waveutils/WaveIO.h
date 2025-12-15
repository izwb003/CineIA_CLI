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

#ifndef WAVEIO_H_
#define WAVEIO_H_

#include <fstream>
#include <ctime>
#include <iostream>
#include <sstream>
#include <string>
#include <iomanip>


#define BROADCAST_WAVE

#if defined(BROADCAST_WAVE)

#define WAV_HEADER_TAG_SIZE			(4)
#define WAVE_FORMAT_PCM				0x0001
#define WAVE_FORMAT_EXTENSIBLE		0xFFFE
#define WAV_FILE_CHUNKSIZE_OFFSET	(4)
#define WAV_RIFF_HEADER_CHUNKSIZE	(36 + 8 + 602)
#define WAV_FMT_CHUNKSIZE			(16)
#define WAV_BEXT_CHUNKSIZE			(602)
#define WAV_DATA_CHUNKSIZE_OFFSET	(WAV_RIFF_HEADER_CHUNKSIZE + 4)
#define WAV_DS64_FILESIZE_OFFSET	(20)
#define WAV_DS64_DATACHUNK_OFFSET	(28)

#define MONO_CHANNEL				(1)
#define BITWIDTH_3BYTES				(3)
#define WAVEFILE_LIMIT				(0xFFFFFFFF)

#else

#define MONO_CHANNEL				(1)
#define WAVEFILE_LIMIT				(0xFFFFFFFF)
#define BITWIDTH_2BYTES				(2)
#define BITWIDTH_3BYTES				(3)
#define BITWIDTH_4BYTES				(4)

#define WAV_HEADER_TAG_SIZE			(4)
#define WAVE_FORMAT_PCM				0x0001
#define WAVE_FORMAT_EXTENSIBLE		0xFFFE
#define WAV_FILE_CHUNKSIZE_OFFSET	(4)
#define WAV_DS64_SIZE				(36)
#define WAV_RIFF_HEADER_CHUNKSIZE	(36)
//#define WAV_RIFF_HEADER_CHUNKSIZE	(60)
#define WAV_FMT_CHUNKSIZE			(16)
//#define WAV_FMT_CHUNKSIZE			(40)
#define WAV_EXTENSIBLE_CBSIZE		(22)	// depends on ?
//#define WAV_FACT_CHUNKSIZE			(12)
#define WAV_FACT_CHUNKSIZE			(0)
#define WAV_DATA_CHUNKSIZE_OFFSET	(40)
//#define WAV_DATA_CHUNKSIZE_OFFSET	(64 + WAV_DS64_SIZE + WAV_FACT_CHUNKSIZE)		// to be updated when additional chunks are supported
#define WAV_DS64_FILESIZE_OFFSET	(20)
#define WAV_DS64_DATACHUNK_OFFSET	(28)

#endif								// if defined(BROADCAST_WAVE)

struct WAVHeader {

//									// offset	size
//	std::string		"RIFF";			// 0		4 
	unsigned int	fileSize;		// 4		4
//	std::string		"WAVE";			// 8		4
//	union
//	{
//		junkChunk	junk;			// 12		36
//		ds64Chunk	ds64;			// 12		36
		// ds64_riffChunkSize		// 20		8
		// ds64_dataChunkSize		// 28		8
//	};

//	std::string		"fmt ";			// 48		4
	unsigned int	fmtChunkSize;	// 52		4
	unsigned int	wFormatTag;		// 56		2
	unsigned int	nChannels;		// 58		2
	unsigned int	nSamplePerSec;	// 60		4
	unsigned int	nAvgBytesPerSec;// 64		4
	unsigned int	nBlockAlign;	// 68		2
	unsigned int	nBitsPerSample;	// 70		2
	unsigned int	cbSize;			// 72		2
	unsigned int	wValidBitsPerSample;// 74	2
	unsigned int	dwChannelMask;	// 76		4
//	unsigned char	GUID[16];		// 80		16
//					factChunk;		// 96		12		// not used
//	std::string		"data";			// 96		4
	unsigned int	dataChunkSize;	// 100		4
};

#if defined(BROADCAST_WAVE)

struct BEXT_data {
	
	char Description[256];									// total = 256, ASCII : Description of the sound sequence
	char Originator[32];									// = 288, ASCII : Name of the originator
	char OriginatorReference[32];							// = 320, ASCII : Reference of the originator
	char OriginationDate[10];								// = 330, ASCII : yyyy:mm:dd
	char OriginationTime[8];								// = 338, ASCII : hh:mm:ss
	unsigned int TimeReferenceLow;							// = 342, First sample count since midnight, low word
	unsigned int TimeReferenceHigh;							// = 346, First sample count since midnight, high word
	unsigned short Version;									// = 348, Version of the BWF; unsigned binary number
	char UMID[64];											// = 412, Binary bytes 0-63 of SMPTE UMID
	unsigned short LoudnessValue;							// = 414, WORD : Integrated Loudness Value of the file in LUFS (multiplied by 100)
	unsigned short LoudnessRange;							// = 416, WORD : Loudness Range of the file in LU (multiplied by 100)
	unsigned short MaxTruePeakLevel;						// = 418, WORD : Maximum True Peak Level of the file expressed as dBTP (multiplied by 100)
	unsigned short MaxMomentaryLoudness;					// = 420, WORD : Highest value of the Momentary Loudness Level of the file in LUFS (multiplied by 100)
	unsigned short MaxShortTermLoudness;					// = 422, WORD : Highest value of the Short-Term Loudness Level of the file in LUFS (multiplied by 100)
	char Reserved[180];										// = 602, 180 bytes, reserved for future use, set to ��NULL��
//	char CodingHistory[];									// = ?,  ASCII : History coding

};

#endif


/*
 * Wav utility class for 48kHz/96kHz, mono 24-bit RIFF/BWF wav file I/O
 */

class WAVFile {

public:

	bool					is_RF64;
	long long				fileSize;
	long long				dataSize;
	std::fstream			*pFile;		
	WAVHeader				header;

#if defined(BROADCAST_WAVE)
	BEXT_data				bext_data;
#endif

	// Constructor

	WAVFile() : is_RF64(false), fileSize(0), dataSize(0), 
#if defined(BROADCAST_WAVE)
		bext_data(),
#endif
		header() {

	};

	// Destructor

	~WAVFile() {

	};

	void writeIntValue(unsigned int nValue);
	void writeShortValue(unsigned int nValue);
	void writeInt64Value(long long value);
	void writeWavHeader();
	void updateRIFFHeader();
	void updateRF64Header();
	int  writeSamplesToFile(int numChannels, int *sampleBuffer[], int numSamples);
	int  writeZeroSamplesToFile(int numChannels, int numSamples);
    
    /**
     * Attempts to locate and reads the wav header.
     * After the wav header has been successfully read, file pointer will be positioned at the first sample of the first channel and
     * the client can use readWavSamplesFromFile() to read samples from file.
     *
     * Returns total number of samples per channel in the wav file. A return value of <= 0 indicates a file error or the file is not mono, 24-bit.
     */     
	int readWavHeader();
    
    int readWavSamplesFromFile(int numChannels, float *sampleBuffer, int numSamples);
	int readWavSamplesFromFile(int numChannels, int *sampleBuffer, int numSamples);

#if defined(BROADCAST_WAVE)

	void initBextData();
	void writeBextChunk();

#endif

};

struct waveFileInfo {
	WAVFile*	pWave;			
	bool		hasSamples;		
}; 


#endif /* WAVEIO_H_ */
