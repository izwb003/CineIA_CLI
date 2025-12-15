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

#include <fstream>
#include <ctime>
#include <iostream>
#include <sstream>
#include <string>
#include <iomanip>
#include <cstring>
#include "WaveIO.h"
#include <stdint.h>

// Write a 4-byte integer value to file
void WAVFile::writeIntValue(unsigned int nValue) {

	unsigned char buffer[WAV_HEADER_TAG_SIZE+1];

	buffer[0] = (unsigned char)(nValue & 0xFF);
	buffer[1] = (unsigned char)((nValue >> 8) & 0xFF);
	buffer[2] = (unsigned char)((nValue >> 16) & 0xFF);
	buffer[3] = (unsigned char)((nValue >> 24) & 0xFF);
	pFile->write((const char *) &buffer[0], 4);

}

// write 2-byte integer value to file
void WAVFile::writeShortValue(unsigned int nValue) {

	unsigned char buffer[2];

	buffer[0] = (unsigned char)(nValue & 0xFF);
	buffer[1] = (unsigned char)((nValue >> 8) & 0xFF);
	pFile->write((const char *) &buffer[0], 2);

}

// write 8-byte 64-bit integer value to file
void WAVFile::writeInt64Value(long long value) {

	unsigned char buffer[8];

	// lower 4 bytes
	buffer[0] = (unsigned char)(value & 0xFF);
	buffer[1] = (unsigned char)((value >> 8) & 0xFF);
	buffer[2] = (unsigned char)((value >> 16) & 0xFF);
	buffer[3] = (unsigned char)((value >> 24) & 0xFF);

	// higher 4 bytes
	buffer[4] = (unsigned char)((value >> 32) & 0xFF);
	buffer[5] = (unsigned char)((value >> 40) & 0xFF);
	buffer[6] = (unsigned char)((value >> 48) & 0xFF);
	buffer[7] = (unsigned char)((value >> 56) & 0xFF);
	pFile->write((const char *) &buffer[0], 8);

}

// wave header writer, supports wave extensible format only, RIFF & RF64
void WAVFile::writeWavHeader() {

	unsigned char tmpData[28] = {
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00
	};

	pFile->seekp(0, std::ios::beg);

	if (!is_RF64)
	{
		// ckID
		pFile->write("RIFF", WAV_HEADER_TAG_SIZE);

		// chunk size (total bytes)
		writeIntValue(header.fileSize);

		// WAVE tag
		pFile->write("WAVE", WAV_HEADER_TAG_SIZE);	

		// JUNK tag
//		pFile->write("JUNK", WAV_HEADER_TAG_SIZE);

		// junk chunk size = 28
//		writeIntValue(28);
//		pFile->write((const char *) &tmpData[0], 28);

#if defined(BROADCAST_WAVE)

		// bext chunk ID
		pFile->write("bext", WAV_HEADER_TAG_SIZE);

		// bext chunk size
		writeIntValue(WAV_BEXT_CHUNKSIZE);

		// Init 4 fields of broadcast extension chunk data
		initBextData();

		// Write broadcast extension chunk data
		writeBextChunk();

#endif						// if defined(BROADCAST_WAVE)

	}
	else
	{
		// ckID
		pFile->write("RF64", WAV_HEADER_TAG_SIZE);

		// chunksize = -1
		writeIntValue(-1);

		// WAVE tag
		pFile->write("WAVE", WAV_HEADER_TAG_SIZE);	

		// JUNK tag
		pFile->write("ds64", WAV_HEADER_TAG_SIZE);

		// ds64 chunk size = 28
		writeIntValue(28);
		writeInt64Value(fileSize);
		writeInt64Value(dataSize);
		pFile->write((const char *) &tmpData[0], 12);
	}

	// chunk ID
	pFile->write("fmt ", WAV_HEADER_TAG_SIZE);	// chunk ID

	// fmt chunk size
	writeIntValue(header.fmtChunkSize);		// for wave extensible	

	// format tag
	writeShortValue(header.wFormatTag);		// format tag

	// number of interleave channels
	writeShortValue(header.nChannels);			

	// sample per sec
	writeIntValue(header.nSamplePerSec);		

	// bytes per sec
	writeIntValue(header.nAvgBytesPerSec);		

	// block align
	writeShortValue(header.nBlockAlign);		

	// bit per sample
	writeShortValue(header.nBitsPerSample);		

	// cbSize
//	writeShortValue(header.cbSize);		

	// valid bit per sample
//	writeShortValue(header.wValidBitsPerSample);		

	// channel mask
//	writeIntValue(header.dwChannelMask);		

	// GUID
//	pFile->write((const char *) &GUID[0], 16);

	/*
	// fact chunk
	pFile->write("fact", WAV_HEADER_TAG_SIZE);	

	// fact chunk size = 4 (minimum)
	writeIntValue(4);

	int numSamples = (int) (dataSize/(header.nChannels * (header.nBitsPerSample/8)));
	writeIntValue(numSamples);

	*/

	// data chunk
	pFile->write("data", WAV_HEADER_TAG_SIZE);	

	if (!is_RF64)
	{

	// data chunk size
		writeIntValue(header.dataChunkSize);		
	}
	else
	{
		writeIntValue(-1);		
	}

	
};

// Update RIFF file size & data sixe fields only
void WAVFile::updateRIFFHeader() {

		pFile->seekp(WAV_FILE_CHUNKSIZE_OFFSET, std::ios::beg);
		writeIntValue(header.fileSize);		

		pFile->seekp(WAV_DATA_CHUNKSIZE_OFFSET, std::ios::beg);
		writeIntValue(header.dataChunkSize);		
}

// update ds64 file size & data size fields only
void WAVFile::updateRF64Header() {

		pFile->seekp(WAV_DS64_FILESIZE_OFFSET, std::ios::beg);
		writeInt64Value(fileSize);		

		pFile->seekp(WAV_DS64_DATACHUNK_OFFSET, std::ios::beg);
		writeInt64Value(dataSize);		
}

// write multi-channel samples to wave file
int WAVFile::writeSamplesToFile(int numChannels, int *sampleBuffer[], int numSamples) {

	int bytesPerSample;
	
	int * ptrCh;
	char *tempBuf;

	tempBuf = new char[numChannels * numSamples * 4];

	int nVal;

	char *ptrDest = tempBuf;
	char temp1, temp2, temp3;

	if (header.nBitsPerSample == 24)
	{
		bytesPerSample = 3;
	}
	else if (header.nBitsPerSample == 32)
	{
		bytesPerSample = 4;
	} else	// 16-bit sample
	{
		bytesPerSample = 2;
	}

	// interleave channels
	for (int j = 0; j < numSamples; j++)	
	{
		for (int k = 0; k < numChannels; k++)	// channel
		{
			ptrCh = sampleBuffer[k];

			nVal = *ptrCh++;
			sampleBuffer[k] = ptrCh;

			temp1 = (nVal >> 24) & 0xFF;
			temp2 = (nVal >> 16) & 0xFF;

			if (bytesPerSample == 3)
			{
				*ptrDest++ = (nVal >> 8) & 0xFF;;
			}
			else if (bytesPerSample == 4)
			{
				temp3 = (nVal >> 8) & 0xFF;;
				*ptrDest++ = nVal & 0xFF;;
				*ptrDest++ = temp3;
			}
			*ptrDest++ = temp2;
			*ptrDest++ = temp1;
		}
	}
	
	int nBytes = numChannels * numSamples * bytesPerSample;

		// write samples to file
	pFile->seekp(0, std::ios::end);
	pFile->write((const char *) &tempBuf[0], nBytes);
	pFile->flush();

	delete[] tempBuf;

	if(pFile->fail() == true)
		return 1;

	return 0;
}

// write zero-samples to wave file
int WAVFile::writeZeroSamplesToFile(int numChannels, int numSamples) {

	int bytesPerSample;
	
	char *tempBuf;

	tempBuf = new char[numChannels * numSamples * 4]();

	if (header.nBitsPerSample == 24)
	{
		bytesPerSample = 3;
	}
	else if (header.nBitsPerSample == 32)
	{
		bytesPerSample = 4;
	} else	// 16-bit sample
	{
		bytesPerSample = 2;
	}

	int nBytes = numChannels * numSamples * bytesPerSample;

		// write samples to file
	pFile->seekp(0, std::ios::end);
	pFile->write((const char *) &tempBuf[0], nBytes);
	pFile->flush();

	delete[] tempBuf;

	if(pFile->fail() == true)
		return 1;

	return 0;

}

/// Traits supplied by a wav file header
///
struct wavFileTraits
{
    wavFileTraits() {
        audioBegin_ = 0;
        audioEnd_ = 0;
        sampleRate_ = 0;
        channels_ = 0;
        bits_ = 0;
        isFloat_ = false;
    }
    int32_t  audioBegin_;
    int32_t  audioEnd_;
    uint32_t sampleRate_;
    uint32_t channels_;
    uint32_t bits_;
    bool     isFloat_;
};

// ============================================================================
// WAV File Format Stuff (Not Broadcast Wave) - this was pulled from a very
// simple reader used in tool code at SRS Labs
//
// !!! This probably should be replaced with WaveIO implementation in the
// !!! MDA tools folder
//
typedef struct tagIFFHEAD
{
    int32_t group;
    int32_t chksize;
    int32_t riff;
    
} IFFHEAD; // 12 bytes

#pragma pack(push,2)
typedef struct tagWAVHEAD
{
    int32_t  chunkID;
    int32_t  chunkSize;
    
    int16_t  wFormatTag;
    uint16_t wChannels;
    uint32_t dwSamplesPerSec;
    uint32_t dwAvgBytesPerSec;
    uint16_t wBlockAlign;
    uint16_t wBitsPerSample;
    
} WAVHEAD; // 28 bytes
#pragma pack(pop)

typedef struct tagSNDHEAD
{
    int32_t           chunkID;
    int32_t           chunkSize;
    
} SNDHEAD; // 8 bytes
/// \endcond

#define wavhelp_groupID  'FFIR'
#define wavhelp_riffType 'EVAW'
#define wavhelp_formatID ' tmf'
#define wavhelp_dataID   'atad'

// This is a _very_ simple parser that works best with the standard 44-byte header
bool ParseBasicWAVHeader(std::fstream& file, wavFileTraits& traits)
{
    // file must already be opened and good
    if (!file.is_open() && !file.bad()) {
        return false;
    }
    
    file.seekg(0);
    
    const size_t chunkheadsize = 8;
    size_t bytesRead = 0;
    WAVHEAD wavHead;
    SNDHEAD sndHead;
    IFFHEAD iffHead;
    size_t headsize = 0;
    size_t wavsize = 0;
    size_t offsettowavehead = 0;
    size_t sndsize = 0, offsettosndhead = 0;
    traits.audioBegin_ = 0;
    traits.audioEnd_ = 0;
    
    headsize = file.read(reinterpret_cast<char*>(&iffHead), sizeof(IFFHEAD)).gcount();
    
    if ( iffHead.group != wavhelp_groupID )
        return 0;
    
    if ( iffHead.riff != wavhelp_riffType )
        return 0;
    
    // Hunt down the wave header
    
    while ( (wavsize = file.read(reinterpret_cast<char*>(&wavHead), chunkheadsize).gcount()) > 0)
    {
        if ( wavHead.chunkID == wavhelp_formatID && (wavsize == chunkheadsize) )
        {
            // We found it!  Already read the chunk header, now we
            // need the rest
            bytesRead = file.read(reinterpret_cast<char*>(&wavHead) + chunkheadsize, sizeof(WAVHEAD)-chunkheadsize).gcount();
            wavsize += bytesRead;
            break;
        }
        else
        {
            // This wasn't it, need to move on
            offsettowavehead += wavsize + wavHead.chunkSize;
            file.seekg(offsettowavehead + headsize,std::fstream::beg);
        }
    }
    
    if ( wavHead.chunkID != wavhelp_formatID )
        return 0;
    
    // Hrm, is the file too short to contain any data
    
    if ( headsize < sizeof(IFFHEAD) || wavsize < sizeof(WAVHEAD) )
        return 0;
    
    // Is it bigger than we expected?
    
    if ( wavsize < ((unsigned)wavHead.chunkSize+chunkheadsize) )
    {
        file.seekg(headsize + offsettowavehead + wavHead.chunkSize + chunkheadsize, std::fstream::beg);
        wavsize += (wavHead.chunkSize-sizeof(WAVHEAD)+8);
    }
    
    // Hunt down the sound header
    
    while ( (sndsize = file.read(reinterpret_cast<char*>(&sndHead), chunkheadsize).gcount()) > 0)
    {
        if ( sndHead.chunkID == wavhelp_dataID )
        {
            // We found it!  Already read the chunk header, now we
            // need the rest
            bytesRead = file.read(reinterpret_cast<char*>(&sndHead) + chunkheadsize, sizeof(SNDHEAD)-chunkheadsize).gcount();
            sndsize += bytesRead;
            
            break;
        }
        else
        {
            // This wasn't it, need to move on
            offsettosndhead += sndsize + sndHead.chunkSize;
            file.seekg(offsettosndhead + offsettowavehead + wavsize + headsize, std::fstream::beg);
        }
    }
    
    if ( sndHead.chunkID != wavhelp_dataID )
        return 0;
    
    traits.audioBegin_ = (unsigned int)(headsize+wavsize+sndsize+offsettowavehead + offsettosndhead);
    
    // Where does the audio data end
    
    if ( sndHead.chunkSize == 0 )
    {
        // Assume there is SOME data based on the file length
        file.seekg(0, std::fstream::end);
        traits.audioEnd_ = static_cast<int32_t>(file.tellg()) - traits.audioBegin_;
    }
    else
    {
        traits.audioEnd_ = traits.audioBegin_ + static_cast<int32_t>(sndHead.chunkSize);
    }
    
    // go to sample 0
    file.seekg(traits.audioBegin_,std::fstream::beg);
    
    traits.sampleRate_ = static_cast<uint32_t>(wavHead.dwSamplesPerSec);
    traits.channels_   = wavHead.wChannels;
    traits.bits_       = wavHead.wBitsPerSample;
    traits.isFloat_    = wavHead.wFormatTag == 0x3 ? true : false; // i.e. WAVE_FORMAT_IEEE_FLOAT;
    
    return (traits.audioEnd_-traits.audioBegin_) >= 0 ? true : false;
}

int WAVFile::readWavHeader() {

    int totalSamplesPerChannel = 0;

    wavFileTraits traits;
    if (ParseBasicWAVHeader(*pFile, traits))
    {
        if (!traits.isFloat_ && (traits.channels_ == 1) && (traits.bits_ == 24))
        {
            totalSamplesPerChannel = (traits.audioEnd_ - traits.audioBegin_)/3;
        }
    }
    else
    {
        std::cout << "readWavHeader fail over using original implementation" << std::endl;
        pFile->seekg((WAV_DATA_CHUNKSIZE_OFFSET + 4), std::ios::beg);
    }
    
    return totalSamplesPerChannel;
}

void mdaPCM24BEToInt32_LE(int *pDest, char *pSrc, unsigned int pCount) {
    
    unsigned int i;
    
    unsigned char *lSrc = (unsigned char *) pSrc;
    
    for(i = 0; i < pCount; i++)
    {
        
        *pDest  = *(lSrc++) << 8;
        *pDest |= *(lSrc++) << 16;
        *pDest |= *(lSrc++) << 24;
        
        pDest++;
        
    }
}

static const double div_2147483648_ = 1.0 / 2147483648.0;

static void Int24_To_Float32(void *destinationBuffer,
                             const void *sourceBuffer,
                             unsigned int count )
{
    unsigned char *src = (unsigned char*)sourceBuffer;
    float *dest = (float*)destinationBuffer;
    int temp = 0;
    
    while( count-- )
    {
        temp = (((unsigned int)src[0]) << 8);
        temp = temp | (((unsigned int)src[1]) << 16);
        temp = temp | (((unsigned int)src[2]) << 24);
        
        *dest = (float) ((double)temp * div_2147483648_);
        
        src += 3;
        dest += 1;
    }
}


int WAVFile::readWavSamplesFromFile(int numChannels, float *sampleBuffer, int numSamples)
{
    int nTotalBytes = numChannels * numSamples * 3;
    char *tempBuf;
    
    tempBuf = new char[nTotalBytes];
    
    std::streamsize br;
    
    pFile->read(tempBuf, nTotalBytes);
    
    br = pFile->gcount();
    
    int nSamplesRead = ((int) br)/3;
    
    Int24_To_Float32(sampleBuffer, tempBuf, nSamplesRead);
    
    delete[] tempBuf;
    
    return nSamplesRead;
    
}

// write multi-channel samples to wave file
int WAVFile::readWavSamplesFromFile(int numChannels, int *sampleBuffer, int numSamples) {


	int nTotalBytes = numChannels * numSamples * 3;
	char *tempBuf;

	tempBuf = new char[nTotalBytes];

	std::streamsize br;

	pFile->read(tempBuf, nTotalBytes);

	br = pFile->gcount();

	int nSamplesRead = ((int) br)/3;

	mdaPCM24BEToInt32_LE(sampleBuffer,tempBuf, nSamplesRead);

	delete[] tempBuf;

	return nSamplesRead;

}

#if defined(BROADCAST_WAVE)

// initBextData() implementation
void WAVFile::initBextData() {

	// get current time and date
	time_t now = time(0);
	struct tm* timeInfo = localtime(&now);

#if 0

	tm *gmtm = gmtime(&now);
	char* utcDateTime = asctime(gmtm);							// Or UTC time							

	// Debug code. Disable for release
	std::cout << "\nTime now:\t" << dateTime;
	std::cout << "\nUTC Time now:\t" << utcDateTime;
	std::cout << "\nyear now:\t" << timeInfo->tm_year;
	std::cout << "\nmonth now:\t" << timeInfo->tm_mon;
	std::cout << "\nday now:\t" << timeInfo->tm_mday;
	std::cout << "\nhour now:\t" << timeInfo->tm_hour;
	std::cout << "\nminute now:\t" << timeInfo->tm_min;
	std::cout << "\nsecond now:\t" << timeInfo->tm_sec;
	std::cout << std::endl << std::endl << std::endl << std::endl;
#endif

//	std::string originatorString;
//	originatorString = "DTS MDAExporter";
//
//	strncpy(bext_data.Originator, originatorString.c_str(), sizeof(bext_data.Originator));
//
//	std::string originatorRefString;
//	originatorRefString = "DTS MDAExporter";
//
//	strncpy(bext_data.OriginatorReference, originatorRefString.c_str(), sizeof(bext_data.OriginatorReference));

	std::string timeString;
	std::ostringstream timeStringStream;

//	timeString = std::to_string(static_cast<long long>(timeInfo->tm_hour))
//		+ ":" + std::to_string(static_cast<long long>(timeInfo->tm_min))
//		+ ":" + std::to_string(static_cast<long long>(timeInfo->tm_sec));

    timeStringStream << std::setw(2) << std::setfill('0') << timeInfo->tm_hour << ":" 
		<< std::setw(2) << std::setfill('0') << timeInfo->tm_min << ":" 
		<< std::setw(2) << std::setfill('0') << timeInfo->tm_sec;

	timeString = timeStringStream.str();

	std::strncpy(bext_data.OriginationTime, timeString.c_str(), sizeof(bext_data.OriginationTime));
//	bext_data.OriginationTime[sizeof(bext_data.OriginationTime) - 1] = 0;						// full 8 char needed to represent time

	std::string dateString;
	std::ostringstream dateStringStream;

//	dateString = std::to_string(static_cast<long long>(timeInfo->tm_year + 1900))
//		+ "-" + std::to_string(static_cast<long long>(timeInfo->tm_mon + 1))
//		+ "-" + std::to_string(static_cast<long long>(timeInfo->tm_mday));

    dateStringStream << std::setw(4) << (timeInfo->tm_year + 1900) << "-" 
		<< std::setw(2) << std::setfill('0') << (timeInfo->tm_mon + 1) << "-" 
		<< std::setw(2) << std::setfill('0') << timeInfo->tm_mday;

	dateString = dateStringStream.str();

	std::strncpy(bext_data.OriginationDate, dateString.c_str(), sizeof(bext_data.OriginationDate));
//	bext_data.OriginationDate[sizeof(bext_data.OriginationDate) - 1] = 0;						// full 10 char needed to represent date

//	std::cout << bext_data.Originator << std::endl;
//	std::cout << bext_data.OriginatorReference << std::endl;
//	std::cout << bext_data.OriginationTime << std::endl;
//	std::cout << bext_data.OriginationDate << std::endl;

	return;

}

// writeBextChunk() implementation
void WAVFile::writeBextChunk() {

	// Write "Description"
	pFile->write((const char *)bext_data.Description, sizeof(bext_data.Description));

	// Write "Originator"
	pFile->write((const char *)bext_data.Originator, sizeof(bext_data.Originator));

	// Write "OriginatorReference"
	pFile->write((const char *)bext_data.OriginatorReference, sizeof(bext_data.OriginatorReference));

	// Write "OriginationDate"
	pFile->write((const char *)bext_data.OriginationDate, sizeof(bext_data.OriginationDate));

	// Write "OriginationTime"
	pFile->write((const char *)bext_data.OriginationTime, sizeof(bext_data.OriginationTime));

	// Write "TimeReferenceLow"
	writeIntValue(bext_data.TimeReferenceLow);

	// Write "TimeReferenceHigh"
	writeIntValue(bext_data.TimeReferenceHigh);

	// Write "Version"
	writeShortValue(bext_data.Version);

	// Write "UMID"
	pFile->write((const char *)bext_data.UMID, sizeof(bext_data.UMID));

	// Write "LoudnessValue"
	writeShortValue(bext_data.LoudnessValue);

	// Write "LoudnessRange"
	writeShortValue(bext_data.LoudnessRange);

	// Write "MaxTruePeakLevel"
	writeShortValue(bext_data.MaxTruePeakLevel);

	// Write "MaxMomentaryLoudness"
	writeShortValue(bext_data.MaxMomentaryLoudness);

	// Write "MaxShortTermLoudness"
	writeShortValue(bext_data.MaxShortTermLoudness);

	// Write "Reserved"
	pFile->write((const char *)bext_data.Reserved, sizeof(bext_data.Reserved));

	return;

}

#endif						// if defined(BROADCAST_WAVE)






