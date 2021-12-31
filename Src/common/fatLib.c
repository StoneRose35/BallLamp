#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "systemChoice.h"

#ifdef HARDWARE
#include "spi_sdcard.h"
#else
static FILE * fid;
#endif

typedef struct  
{
    uint8_t partitionType;
    uint32_t lbaStart;
    uint32_t nrSectors;
} PartitionInfoType;

typedef struct 
{
    uint16_t bytesPerSector;
    uint8_t sectorsPerCluster;
    uint16_t numberOfReservedSectors;
    uint8_t numberOfFats;
    uint32_t sectorsPerFat;
    uint32_t rootDirectoryFirstCluster;
} VolumeIdType;


typedef struct 
{
    char filename[8];
    char fileext[3];
    uint8_t attrib;
    uint32_t firstCluster;
    uint32_t size;
} DirectoryEntryType;

typedef struct 
{
    uint32_t clusterLbaBegin;
    uint32_t fatLbaBgin;
} DerivedFATDataType;

typedef struct 
{
    PartitionInfoType partitionInfo;
    VolumeIdType volumeId;
    DerivedFATDataType derivedFATData;

} SdCardInfoType;

typedef union 
{
    uint8_t bytesData[4];
    uint32_t wordValue;
} UInt32Type;

typedef struct 
{
    DirectoryEntryType dirEntry;
    uint16_t clusterPtr;
    uint16_t sectorPtr; // in sectors, should be increased at every read/write Operation
    uint16_t clusterCntr;
    uint8_t sectorBuffer[512];
} FilePointerType;




#ifndef HARDWARE
void readSector(uint8_t*,uint64_t);
#endif
void getPartitionInfo(uint8_t *,uint8_t,PartitionInfoType*);
void getVolumeId(uint8_t *,VolumeIdType* );

uint32_t getFatLbaBegin(PartitionInfoType*,VolumeIdType*);
uint32_t getClusterLbaBegin(PartitionInfoType* ,VolumeIdType*);
uint32_t getClusterLba(uint32_t,PartitionInfoType*,VolumeIdType*);
uint16_t getDirectoryEntries(uint8_t*,DirectoryEntryType**);
#ifdef HARDWARE
uint32_t getNextCluster(uint32_t clusterNr,PartitionInfoType* partInfo,VolumeIdType* volumeId);
#else
uint32_t getNextCluster(FILE * fid,uint32_t clusterNr,PartitionInfoType* partInfo,VolumeIdType* volumeId);
#endif

#define FATLIB_WRONG_MBR_SIG 1
#define FATLIB_WRONG_VOLUMEID_SIG 2
#define FATLIB_WRONG_PART_TYPE 3

#ifndef HARDWARE
void main(int argc,char ** argv)
{
    uint8_t sector[512];
    PartitionInfoType partInfo;
    VolumeIdType volumeId;
    DirectoryEntryType filesOfSectorArray[16];
    DirectoryEntryType * filesOfSector = filesOfSectorArray;
    char fnameDisplay[12];
    uint8_t fcnt = 0x10;

    const char * devicePath = "/dev/sda"; //"/home/philipp/testlog.txt";
    printf("Welcome to the FAT Access Library\r\n");
    fid = fopen(devicePath,"rb");

    readSector(sector,0);
    printf("the signature is: %x %x\r\n",sector[510],sector[511]);

    printf("reading the start lba address of the first partition\r\r");
    getPartitionInfo(sector,0,&partInfo);
    printf("Partition 0 starts at %d, size: %d\r\n",partInfo.lbaStart,partInfo.nrSectors);
    printf("reading volume id of first partition\r\n");
    readSector(sector,partInfo.lbaStart);
    printf("the signature is: %x %x\r\n",sector[510],sector[511]);
    getVolumeId(sector,&volumeId);
    printf("**** Volume ID ****\r\n");
    printf("Bytes per Sector: %d\r\n",volumeId.bytesPerSector);
    printf("Sectors per Cluster: %d\r\n",volumeId.sectorsPerCluster);
    printf("Number of Reserved Sectors: %d\r\n",volumeId.numberOfReservedSectors);
    printf("Sectors per FAT: %d\r\n",volumeId.sectorsPerFat);
    printf("Root Directory First Cluster: %d\r\n",volumeId.rootDirectoryFirstCluster);
    uint32_t rootDirClusterLba = getClusterLba(volumeId.rootDirectoryFirstCluster,&partInfo,&volumeId);

    uint16_t sec_cnt = 0;
    while(sec_cnt < volumeId.sectorsPerCluster && fcnt == 0x10)
    {
        printf("**** Reading Sector %d\r\n", sec_cnt);
        readSector(sector,rootDirClusterLba + sec_cnt);
        fcnt = getDirectoryEntries(sector,&filesOfSector);
        for (uint8_t q=0;q<fcnt;q++)
        {
            if ((filesOfSector[q].attrib & 0x0F) != 0x0F)
            {
                uint8_t uu=0;
                for(uint8_t u=0;u<8;u++)
                {
                    if(filesOfSector[q].filename[u] != 0x20)
                    {
                        fnameDisplay[uu++] = filesOfSector[q].filename[u];
                    }
                }
                fnameDisplay[uu++] = '.';
                for(uint8_t u=0;u<3;u++)
                {
                    if(filesOfSector[q].fileext[u] != 0x20)
                    {
                        fnameDisplay[uu++] = filesOfSector[q].fileext[u];
                    }
                }
                fnameDisplay[uu]=0;
                printf("\tFilename: %s,\t Attrib: %i,\t Size %i,\t first Cluster: %i\r\n",fnameDisplay,filesOfSector[q].attrib,filesOfSector[q].size,filesOfSector[q].firstCluster);

            }
        }
        sec_cnt++;
    }
    if (sec_cnt == volumeId.sectorsPerCluster)
    {
        // read next cluster location from fat
    }
    uint32_t nextCluster = 0;
    uint32_t prevCluster = volumeId.rootDirectoryFirstCluster;
    // test read the fat to see if the directory really is only contained in one cluster
    printf("testing cluster walk: starting off at cluster %X\r\n",prevCluster);
    while ((nextCluster & 0x0FFFFFFF) != 0x0FFFFFFF )
    {
        nextCluster = getNextCluster(fid,prevCluster,&partInfo,&volumeId);
        printf("next cluster %X\r\n",nextCluster);
        prevCluster = nextCluster;
    }
    fclose(fid);

}
#endif

#ifndef HARDWARE
void readSector(uint8_t* res,uint64_t offset)
{
    fseek(fid,offset << 9,SEEK_SET);
    fread(res,512,1,fid);
}
#endif

void getPartitionInfo(uint8_t *sect,uint8_t partNr,PartitionInfoType* res)
{
    res->partitionType = *((uint8_t*)(sect + 446 + (partNr << 6 ) + 4));
    res->lbaStart = *((uint32_t*)(sect + 446 + (partNr << 6) + 8));
    res->nrSectors = *((uint32_t*)(sect + 446 + (partNr << 6) + 12));
}

void getVolumeId(uint8_t * sect,VolumeIdType* res)
{
    res->bytesPerSector = *((uint16_t*)(sect + 0x0B));
    res->sectorsPerCluster = *(sect + 0x0D);
    res->numberOfReservedSectors = *((uint16_t*)(sect + 0xE));
    res->numberOfFats = *(sect + 0x10);
    res->sectorsPerFat = *((uint32_t*)(sect + 0x24));
    res->rootDirectoryFirstCluster = *((uint32_t*)(sect + 0x2C));
}

uint32_t getFatLbaBegin(PartitionInfoType* partInfo,VolumeIdType* volumeId)
{
    return partInfo->lbaStart + volumeId->numberOfReservedSectors;
}

uint32_t getClusterLbaBegin(PartitionInfoType* partInfo,VolumeIdType* volumeId)
{
    return partInfo->lbaStart + volumeId->numberOfReservedSectors + (volumeId->numberOfFats * volumeId->sectorsPerFat);
}

uint32_t getClusterLba(uint32_t clusterNr,PartitionInfoType* partInfo,VolumeIdType* volumeId)
{
    uint32_t clusterLbaBegin = getClusterLbaBegin(partInfo,volumeId);
    return clusterLbaBegin + (clusterNr - 2)*volumeId->sectorsPerCluster;
}

uint16_t getDirectoryEntries(uint8_t * sect,DirectoryEntryType** entries)
{
    uint16_t c=0;
    while(*(sect + (c<<5))!=0 && c < 16)
    {
        
        for(uint8_t c2=0;c2<8;c2++)
        {
            (*entries + c)->filename[c2] =  *((char*)sect + (c<<5)+c2);
        }
        (*entries + c)->fileext[0] = *(sect + (c<<5)+8);
        (*entries + c)->fileext[1] = *(sect + (c<<5)+9);
        (*entries + c)->fileext[2] = *(sect + (c<<5)+10);
        (*entries + c)->attrib = *(sect + (c<<5)+11);
        (*entries + c)->firstCluster = (*((uint16_t*)(sect + (c<<5)+0x14)) << 16) + *((uint16_t*)(sect + (c<<5)+0x1A));
        (*entries + c)->size = *((uint32_t*)(sect + (c<<5) + 0x1C));
        
        c++;
    }
    return c;
}


/**
 * @brief Get the Files In Directory 
 * 
 * @param fid the file identifier for the raw file object (e.b. /dev/sda)
 * @param volumeId information on the volume
 * @param directoryClusterLba the lba address of the current cluster
 * @param res the array containing the raw file information, is allocated dynamically
 * @return uint16_t the nnumber of raw directory entries found
 */
uint16_t getFilesInDirectory(FILE * fid, VolumeIdType * volumeId,uint32_t directoryClusterLba,DirectoryEntryType * res)
{
    uint8_t sector[512];
    uint8_t fcnt =0x10;
    uint16_t sec_cnt = 0;
    uint16_t total_cnt;
    DirectoryEntryType * resBfr;
    res = (DirectoryEntryType*)malloc(0x20*sizeof(DirectoryEntryType));
    while(sec_cnt < volumeId->sectorsPerCluster && fcnt == 0x10)
    {
        readSector(sector,directoryClusterLba + sec_cnt);
        fcnt = getDirectoryEntries(sector,&res);
        total_cnt += fcnt;
        if (fcnt==0x10) // increase size if 16 entries have been read
        {
            for (uint16_t c2=0;c2<total_cnt;c2++)
            {
                *(resBfr+c2)=*(res+c2);
            }
            free(res);
            res = (DirectoryEntryType*)malloc((total_cnt+0x10)*sizeof(DirectoryEntryType));
        }
        sec_cnt++;
    }
    return total_cnt;
}

uint8_t openFile(DirectoryEntryType * dirEntry,FilePointerType * fp)
{
    fp->dirEntry = *dirEntry;
    fp->sectorPtr = 0;
    fp->clusterPtr = fp->dirEntry.firstCluster;
}


uint16_t readFile(FilePointerType * fp,PartitionInfoType *partitionInfo,VolumeIdType * volumeId)
{
    uint16_t retcode = 0;
    readSector(fp->sectorBuffer,getClusterLba(fp->dirEntry.firstCluster,partitionInfo,volumeId) + fp->sectorPtr);
    uint32_t bytes_read = (fp->sectorPtr + fp->clusterCntr * volumeId->sectorsPerCluster) << 8;
    if (bytes_read < fp->dirEntry.size)
    {
        if (bytes_read + 512 > fp->dirEntry.size)
        {
            uint16_t c2 = 0;
            for (uint16_t c = fp->dirEntry.size - (fp->sectorPtr << 8);c< fp->dirEntry.size;c++)
            {
                fp->sectorBuffer[c2++] = 0;
            }
            retcode = c2;
        }
        else
        {
            retcode = 512;
        }
        fp->sectorPtr ++;
        if(fp->sectorPtr >= volumeId->sectorsPerCluster)
        {
            getNextCluster(fp->clusterPtr,partitionInfo,volumeId);
            fp->clusterCntr++;
            fp->sectorPtr = 0;
        }
    }
    return retcode;
}

#ifdef HARDWARE
uint32_t getNextCluster(uint32_t clusterNr,PartitionInfoType* partInfo,VolumeIdType* volumeId)
{
    uint8_t sector[512];
    uint32_t fatbegin = getFatLbaBegin(partInfo,volumeId);
    readSector(sector,fatbegin + (clusterNr >> 7));
    return *((uint32_t*)sector + ((1 + clusterNr) << 0));
}
#else
uint32_t getNextCluster(FILE * fid,uint32_t clusterNr,PartitionInfoType* partInfo,VolumeIdType* volumeId)
{
    uint8_t sector[512];
    UInt32Type fatEntry;
    uint32_t fatbegin = getFatLbaBegin(partInfo,volumeId);
    readSector(sector,fatbegin + (clusterNr >> 7));
    //fatEntry.bytesData[0] = *((uint8_t*)sector + ((1 + clusterNr) << 2));
    //fatEntry.bytesData[1] = *((uint8_t*)sector + ((1 + clusterNr) << 2) + 1);
    //fatEntry.bytesData[2] = *((uint8_t*)sector + ((1 + clusterNr) << 2) + 2);
    //fatEntry.bytesData[3] = *((uint8_t*)sector + ((1 + clusterNr) << 2) + 3);
    return *((uint32_t*)sector + ((1 + clusterNr) << 0));//fatEntry.wordValue;
}
#endif



/**
 * @brief initializes a SD-Card having a FAT32-Partition as a first partition. Returns an error if this is not the case.
 * 
 * @return uint8_t 0 if successful, an error code otherwise
 */
uint8_t initFatSDCard(SdCardInfoType * sdCard)
{

    uint8_t sector[512];
    readSector(sector,0);
    if(sector[510] != 0x55 || sector[511] != 0xAA)
    {
        return  FATLIB_WRONG_MBR_SIG;
    }
    getPartitionInfo(sector,0,&sdCard->partitionInfo);
    if(sdCard->partitionInfo.partitionType != 0x0B)
    {
        return FATLIB_WRONG_PART_TYPE;
    }
    readSector(sector,sdCard->partitionInfo.lbaStart);
    if(sector[510] != 0x55 || sector[511] != 0xAA)
    {
        return  FATLIB_WRONG_VOLUMEID_SIG;
    }
    getVolumeId(sector,&sdCard->volumeId);
    sdCard->derivedFATData.fatLbaBgin = getFatLbaBegin(&sdCard->partitionInfo,&sdCard->volumeId);
    sdCard->derivedFATData.clusterLbaBegin = getClusterLbaBegin(&sdCard->partitionInfo,&sdCard->volumeId);
    return 0;
}
