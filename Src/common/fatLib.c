#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "systemChoice.h"

#ifdef HARDWARE
#include "spi_sdcard.h"
#endif

typedef struct  
{
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



#ifndef HARDWARE
void readSector(uint8_t*,FILE*,uint64_t);
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
    FILE*  fid;
    fid = fopen(devicePath,"rb");

    readSector(sector,fid,0);
    printf("the signature is: %x %x\r\n",sector[510],sector[511]);

    printf("reading the start lba address of the first partition\r\r");
    getPartitionInfo(sector,0,&partInfo);
    printf("Partition 0 starts at %d, size: %d\r\n",partInfo.lbaStart,partInfo.nrSectors);
    printf("reading volume id of first partition\r\n");
    readSector(sector,fid,partInfo.lbaStart);
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
        readSector(sector,fid,rootDirClusterLba + sec_cnt);
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
void readSector(uint8_t* res,FILE* fid,uint64_t offset)
{
    fseek(fid,offset << 9,SEEK_SET);
    fread(res,512,1,fid);
}
#endif

void getPartitionInfo(uint8_t *sect,uint8_t partNr,PartitionInfoType* res)
{
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

#ifdef HARDWARE
uint32_t getNextCluster(uint32_t clusterNr,PartitionInfoType* partInfo,VolumeIdType* volumeId)
{
    uint8_t sector[512];
    uint32_t fatbegin = getFatLbaBegin(partInfo,volumeId);
    readSector(sector,fatbegin + (clusterNr >> 7));
    return *((uint32_t*)sector + clusterNr);
}
#else
uint32_t getNextCluster(FILE * fid,uint32_t clusterNr,PartitionInfoType* partInfo,VolumeIdType* volumeId)
{
    uint8_t sector[512];
    uint32_t fatbegin = getFatLbaBegin(partInfo,volumeId);
    readSector(sector,fid,fatbegin + (clusterNr >> 7));
    return *((uint32_t*)sector + clusterNr);
}
#endif
