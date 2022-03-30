#include "inc/wavReader.h"
#include "audio/fxprogram/fxProgram.h"
#include <stdio.h>
#include <stdlib.h> 
#include <string.h>

#define SELECTED_FX_PROGRAM 0
#define FX_PROGRAM_PARAM1_VAL 4000
#define FX_PROGRAM_PARAM2_VAL 3800
#define FX_PROGRAM_PARAM3_VAL 3000
#define TAIL_TIME 48000
int main(int argc, char ** argv)
{
    uint32_t byteCnt=0;
    WavFileType wavFileIn;
    WavFileType wavFileOut;
    int16_t sample[2];
    int16_t dataOut;
    char filenameOut[256];
    char filenameIn[256];
    uint16_t param1, param2, param3;
    uint8_t fxProgramNr;
    printf("Offline FxProgram Processor v0.1\r\n\r\n");
    if (argc == 1)
    {
        printf("Useage: processThroughFxProgram <fileIn.wav> <fxProgram> <param1> <param2> <param3> \r\n");
        printf("processing hard-coded values\r\n");
        param1=FX_PROGRAM_PARAM1_VAL;
        param2=FX_PROGRAM_PARAM2_VAL;
        param3=FX_PROGRAM_PARAM3_VAL;
        fxProgramNr=SELECTED_FX_PROGRAM;
        strcpy(filenameIn,"./audiosamples/guit_riff_16bit.wav");
        strcpy(filenameOut,"./audiosamples/guit_riff_16bit.wav");
    }
    else
    {
        strcpy(filenameIn,argv[1]);
        fxProgramNr = (uint8_t)atoi(argv[2]);
        param1 = (uint8_t)atoi(argv[3]);
        param2 = (uint8_t)atoi(argv[4]);
        param3 = (uint8_t)atoi(argv[5]);

    }
    printf("processing file %s\r\n",filenameIn);
    printf("\tFx Program: %s\r\n",fxPrograms[fxProgramNr]->name);
    printf("\tParameter 1: %d\r\n",param1);
    printf("\tParameter 2: %d\r\n",param2);
    printf("\tParameter 3: %d\r\n",param3);
    size_t fnamelen = strlen(filenameIn);
    strcpy(filenameOut+fnamelen-4,"_proc.wav");
    openWavFile(filenameIn,&wavFileIn);
    createWavFile(filenameOut,&wavFileOut,wavFileIn.dataSize+TAIL_TIME);
    for (uint16_t c=0;c<N_FX_PROGRAMS;c++)
    {
        fxPrograms[c]->setup(fxPrograms[c]->data);
    }
    fxPrograms[fxProgramNr]->param1Callback(param1,fxPrograms[fxProgramNr]->data);
    fxPrograms[fxProgramNr]->param2Callback(param2,fxPrograms[fxProgramNr]->data);
    fxPrograms[fxProgramNr]->param3Callback(param3,fxPrograms[fxProgramNr]->data);
    while(byteCnt < wavFileIn.dataSize+TAIL_TIME)
    {
        if (byteCnt < wavFileIn.dataSize)
        {
            sample[0] = wavFileIn.data[byteCnt>>1];
            if (wavFileIn.wavFormat.wChannels == 2)
            {
                sample[1] = wavFileIn.data[(byteCnt>>1)+1];
            }
        }
        else
        {
            sample[0]=0;
        }
        dataOut = fxPrograms[fxProgramNr]->processSample(sample[0],fxPrograms[fxProgramNr]->data);
        if (wavFileIn.wavFormat.wChannels==2)
        {
            wavFileOut.data[byteCnt >> 2]=dataOut;
            byteCnt+=4;
        }
        else
        {
            wavFileOut.data[byteCnt>>1]=dataOut;
            byteCnt+=2;
        }
    }
    writeWavFile(&wavFileOut);
    fclose(wavFileIn.filePointer);
    fclose(wavFileOut.filePointer);
}