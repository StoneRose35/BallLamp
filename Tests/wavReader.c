#include "inc/wavReader.h"
#include <stdio.h>
#include <string.h>

int main(int argc,char** argv)
{
    WavFileType inFile;
    WavFileType outFile;
    uint16_t sample[2];
    
    openWavFile("testin.wav",&inFile);
    createWavFile("testout.wav",&outFile);

}

int openWavFile(char* filename,WavFileType*wavFile)
{
    wavFile->filePointer = fopen(filename,"rb");
    fread(&wavFile->wavHeader,sizeof(WavHeaderType),1,wavFile->filePointer);
    fread(&wavFile->wavFormat,sizeof(WavFormatType),1,wavFile->filePointer);
    // skip the "data" tag
    fseek(wavFile->filePointer,4,SEEK_CUR);
    // read the file size
    fread(&wavFile->fileSize,4,1,wavFile->filePointer);
    if (strcmp(wavFile->wavHeader.chunkID,"RIFF")!=0)
    {
        return WAVREADER_FORMAT_ERROR;
    }
    if (strcmp(wavFile->wavHeader.riffType,"WAVE")!=0)
    {
        return WAVREADER_FORMAT_ERROR;
    }    
    if (wavFile->wavFormat.wFormatTag != 1)
    {
        return WAVREADER_FORMAT_ERROR;
    }
    if (wavFile->wavFormat.wChannels < 1 || wavFile->wavFormat.wChannels > 2)
    {
        return WAVREADER_FORMAT_ERROR;
    }    
    if (strcmp(wavFile->wavFormat.id,"fmt ") !=0)
    {
        return WAVREADER_FORMAT_ERROR;
    } 
    if (wavFile->wavFormat.wBitsPerSample != 16)
    {
        return WAVREADER_FORMAT_ERROR;
    }
    return 0;
}

int createWavFile(char*filename,WavFileType*wavFile)
{
    const uint32_t zeroSize=0;
    wavFile->filePointer = fopen(filename,"wb");
    wavFile->wavHeader.chunkID[0] = 'R';
    wavFile->wavHeader.chunkID[1] = 'I';
    wavFile->wavHeader.chunkID[2] = 'F';
    wavFile->wavHeader.chunkID[3] = 'F';
    wavFile->wavHeader.riffType[0] = 'W';
    wavFile->wavHeader.riffType[1] = 'A';
    wavFile->wavHeader.riffType[2] = 'V';
    wavFile->wavHeader.riffType[3] = 'E';

    wavFile->wavFormat.dwAvgBytesPerSec = 4*48000;
    wavFile->wavFormat.dwSamplesPerSec=48000;
    wavFile->wavFormat.id[0] = 'f';
    wavFile->wavFormat.id[1] = 'm';    
    wavFile->wavFormat.id[2] = 't';
    wavFile->wavFormat.id[3] = ' ';
    wavFile->wavFormat.wBitsPerSample = 16;
    wavFile->wavFormat.wBlockAlign = 4;
    wavFile->wavFormat.wChannels = 2;
    wavFile->wavFormat.wFormatTag = 1;

    fwrite(&wavFile->wavHeader,sizeof(WavHeaderType),1,wavFile->filePointer);
    fwrite(&wavFile->wavFormat,sizeof(WavFormatType),1,wavFile->filePointer);
    const char dataId[4]={'d','a','t','a'};
    fwrite(dataId,4,1,wavFile->filePointer);
    fwrite(&zeroSize,4,1,wavFile->filePointer); // set size 0 initially
    
}

void getNextSample(uint16_t*sample,WavFileType*wavFile)
{
    if(wavFile->wavFormat.wChannels == 2)
    {
        fread(sample,2,2,wavFile->filePointer);
    }
    else
    {
        fread(sample,2,1,wavFile->filePointer);
    }
}

void writeNextSample(uint16_t*sample,WavFileType*wavFile)
{
    long currentPos;
    uint32_t chunkSize;
    if(wavFile->wavFormat.wChannels == 2)
    {
        wavFile->fileSize += 4;
        fwrite(sample,2,2,wavFile->filePointer);
    }
    else
    {
        wavFile->fileSize +=2;
        fwrite(sample,2,1,wavFile->filePointer);
    }
    currentPos = ftell(wavFile->filePointer);
    rewind(wavFile->filePointer);
    chunkSize = wavFile->fileSize + sizeof(WavHeaderType) + sizeof(WavFormatType);
    wavFile->wavHeader.ChunkSize = chunkSize;
    fwrite(&wavFile->wavHeader,sizeof(WavHeaderType),1,wavFile->filePointer);
    fseek(wavFile->filePointer,sizeof(WavFormatType)+4,SEEK_CUR);
    fwrite(&wavFile->fileSize,4,1,wavFile->filePointer);
    fseek(wavFile->filePointer,currentPos,SEEK_SET);
}