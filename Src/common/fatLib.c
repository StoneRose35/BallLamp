#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "systemChoice.h"
#include "fatLib.h"
#include "stringFunctions.h"

#ifdef HARDWARE
#include "spi_sdcard_display.h"

#else
static FILE * fid;
#include "string.h"
#endif

static SdCardInfoType sdCardInfo;



#ifndef HARDWARE
void main(int argc,char ** argv)
{
    uint8_t sector[512];
    PartitionInfoType partInfo;
    VolumeIdType volumeId;
    //DirectoryEntryType filesOfSectorArray[16];
    DirectoryEntryType * filesOfSector; // = filesOfSectorArray;
    char fnameDisplay[12];
    uint8_t fcnt = 0x10;
    FilePointerType fp;

    const char * devicePath = "/dev/sdb"; //"/home/philipp/testlog.txt";
    printf("Welcome to the FAT Access Library\r\n");
    fid = fopen(devicePath,"rb");

    initFatSDCard();
    readSector(sector,0);
    printf("the signature is: %x %x\r\n",sector[510],sector[511]);

    printf("reading the start lba address of the first partition\r\n");
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
    uint32_t rootDirClusterLba = getClusterLba(volumeId.rootDirectoryFirstCluster);

    uint16_t sec_cnt = 0;
    while(sec_cnt < volumeId.sectorsPerCluster && fcnt == 0x10)
    {
        printf("**** Reading Sector %d\r\n", sec_cnt);
        readSector(sector,rootDirClusterLba + sec_cnt);
        fcnt = getFilesInDirectory(rootDirClusterLba,&filesOfSector); // getDirectoryEntries(sector,&filesOfSector);
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
                if(strncmp(fnameDisplay,"TEST1",5)==0)
                {
                    openFile((filesOfSector + q),&fp);
                }
                printf("\tFilename: %s,\t Attrib: %i,\t Size %i,\t first Cluster: %i\r\n",fnameDisplay,filesOfSector[q].attrib,filesOfSector[q].size,filesOfSector[q].firstCluster);
            }
        }
        sec_cnt++;
    }

    printf("\r\nPrinting File TEST1.TXT\r\n\r\n");
    while (readFile(&fp) > 0 )
    {
        printf("%s",fp.sectorBuffer);
    }
    printf("\r\n\r\n");

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
        nextCluster = getNextCluster(prevCluster);
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

uint32_t getClusterLba(uint32_t clusterNr)
{
    return sdCardInfo.derivedFATData.clusterLbaBegin + (clusterNr - 2)*sdCardInfo.volumeId.sectorsPerCluster;
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
 * @param directoryClusterLba the lba address of the current cluster
 * @param res the array containing the raw file information, is allocated dynamically
 * @return uint16_t the nnumber of raw directory entries found
 */
uint16_t getFilesInDirectory(uint32_t directoryClusterLba,DirectoryEntryType ** res)
{
    uint8_t sector[512];
    uint8_t fcnt =0x10;
    uint16_t sec_cnt = 0;
    uint16_t total_cnt = 0;
    DirectoryEntryType * resBfr;
    *res = (DirectoryEntryType*)malloc(0x20*sizeof(DirectoryEntryType));
    resBfr = (DirectoryEntryType*)malloc(0x20*sizeof(DirectoryEntryType));
    while(sec_cnt < sdCardInfo.volumeId.sectorsPerCluster && fcnt == 0x10)
    {
        readSector(sector,directoryClusterLba + sec_cnt);
        fcnt = getDirectoryEntries(sector,res);
        total_cnt += fcnt;
        if (fcnt==0x10) // increase size if 16 entries have been read
        {
            free(resBfr);
            resBfr = (DirectoryEntryType*)malloc(total_cnt*sizeof(DirectoryEntryType));
            for (uint16_t c2=0;c2<total_cnt;c2++)
            {
                *(resBfr+c2)=*(*res+c2);
            }
            free(res);
            *res = (DirectoryEntryType*)malloc((total_cnt+0x10)*sizeof(DirectoryEntryType));
        }
        sec_cnt++;
    }
    free(resBfr);
    return total_cnt;
}

uint8_t openFile(DirectoryEntryType * dirEntry,FilePointerType * fp)
{
    if (((dirEntry->attrib & 0xF)==0xF) || dirEntry->size == 0)
    {
        return FATLIB_NO_FILE;
    }
    else
        {
        fp->dirEntry = dirEntry;
        fp->sectorPtr = 0;
        fp->clusterPtr = fp->dirEntry->firstCluster;
        fp->clusterCntr = 0;
        return 0;
    }
}

uint8_t openDirectory(DirectoryPointerType * parentDir,char * dirname,DirectoryPointerType * fp)
{
    uint8_t dirFound=0;
    for (uint8_t c=0;c<parentDir->entriesLength;c++)
    {
        if((parentDir->entries[c]->attrib & 0x10) == 0x10 && startsWith(parentDir->entries[c]->filename,dirname)== 1)
        {
            fp->dirEntry = parentDir->entries[c];
            dirFound = 1;
        }
    }

    if (dirFound == 0)
    {
        return FATLIB_DIRECTORY_NOT_FOUND;
    }
    else
    {
        fp->clusterLbaPtr = getClusterLba(fp->dirEntry->firstCluster);
        fp->entries = 0;
        fp->entriesLength = getFilesInDirectory(fp->clusterLbaPtr,fp->entries);
        return 0;
    }
}

/**
 * @brief Create a Directory in the given parent directory
 * 
 * @param fp 
 * @param directoryName 
 * @return uint8_t 
 */
uint8_t createDirectory(DirectoryPointerType * fp,char * directoryName)
{
    uint8_t sect[512];
    uint32_t freeCluster;
    DirectoryEntryType *entriesBfr;
    uint8_t c=0,c2=0;
    // get next free cluster
    freeCluster = getNextFreeCluster();
    
    // increase allocated entries vector by 16 if entries Length is a multiple of 16
    if ((fp->entriesLength & 0x0F) == 0)
    {

        entriesBfr = realloc(*fp->entries,fp->entriesLength+16);
        if (entriesBfr == 0)
        {
            return FATLIB_MEMORY_ALLOC_FAILURE;
        }
        *fp->entries = entriesBfr;
    }

    // write 0xFFFFFF0F to sector of the fat where the free cluster is
    readSector(sect,sdCardInfo.derivedFATData.fatLbaBgin + (freeCluster >> 7));
    *((uint32_t*)sect + (freeCluster & 0x7F))=0xFFFFFF0F; // store as little endian
    writeSector(sect,sdCardInfo.derivedFATData.fatLbaBgin +  (freeCluster >> 7));

    // add a directory entry for the newly created directory
    fp->entries[fp->entriesLength]->attrib =0x10;
    fp->entries[fp->entriesLength]->fileext[0]=' ';
    fp->entries[fp->entriesLength]->fileext[1]=' ';
    fp->entries[fp->entriesLength]->fileext[2]=' ';
    for (c=0;c<8;c++)
    {
        if(*((uint8_t*)directoryName + c) != 0)
        {
            fp->entries[fp->entriesLength]->filename[c] = *(directoryName + c);
        }
        else
        {
            fp->entries[fp->entriesLength]->filename[c] = ' ';
        }
    }
    fp->entries[fp->entriesLength]->firstCluster = freeCluster;
    fp->entries[fp->entriesLength]->size = 0;
    
    
    // write the sector of the parent directory containing the entry of the new directory
    readSector(sect,fp->clusterLbaPtr + (fp->entriesLength >> 4));
    c=fp->entriesLength & 0x0F;
    for(c2=0;c2<32;c2++) // clear directory entry
    {
        *(sect + (c<<5)+c2)=0;
    } 
    // fill in values
    for(c2=0;c2<8;c2++)
    {
        *((char*)sect + (c<<5)+c2) = fp->entries[fp->entriesLength]->filename[c2];
    }
    *(sect + (c<<5)+8) = fp->entries[fp->entriesLength]->fileext[0];
    *(sect + (c<<5)+9) = fp->entries[fp->entriesLength]->fileext[1];
    *(sect + (c<<5)+10) = fp->entries[fp->entriesLength]->fileext[2];
    *(sect + (c<<5)+11) = fp->entries[fp->entriesLength]->attrib;
    *((uint16_t*)(sect + (c<<5)+0x14)) = fp->entries[fp->entriesLength]->firstCluster >> 16;
    *((uint16_t*)(sect + (c<<5)+0x1A)) = (uint16_t)(fp->entries[fp->entriesLength]->firstCluster & 0x0000FFFF);
    *((uint32_t*)(sect + (c<<5) + 0x1C)) = fp->entries[fp->entriesLength]->size;
    writeSector(sect,fp->clusterLbaPtr + (fp->entriesLength >> 4));

    fp->entriesLength++;
    return 0;
}

uint8_t deleteDirectory(DirectoryPointerType * parentDir,DirectoryPointerType * fp)
{
    uint8_t sect[512];
    uint8_t nClusters=0,nSectors=0;
    uint32_t clusterList[FATLIB_FOLDER_MAX_CLUSTERS];
    uint32_t sectorsList[FATLIB_FOLDER_MAX_CLUSTERS];

    // write 0x00000000 to all clusters of the directory
    uint32_t prevCluster = fp->dirEntry->firstCluster;
    uint32_t nextCluster = 0;
    clusterList[nClusters++]=prevCluster;
    while ((nextCluster & 0x0fffffff) != 0x0fffffff)
    {
        nextCluster = getNextCluster(prevCluster);

        prevCluster = nextCluster;
        clusterList[nClusters++]=prevCluster;
        if(nClusters >= FATLIB_FOLDER_MAX_CLUSTERS)
        {
            return FATLIB_FOLDERDELETE_TOO_MANY_CLUSTERS;
        }
    }
    // gather list of all sectors to be updated
    for (uint8_t c=0;c<FATLIB_FOLDER_MAX_CLUSTERS;c++)
    {
        sectorsList[c]=0;
    }
    for(uint8_t c2=0;c2<nClusters;c2++)
    {
        uint8_t contains = 0;
        for(uint8_t c3=0;c3<nSectors;c3++)
        {
            if(sectorsList[c3] == (clusterList[c2] >> 7))
            {
                contains = 1;
            }
        }
        if (contains == 0)
        {
            sectorsList[nSectors++]=(clusterList[c2] >> 7);
        }
    }
    for(uint8_t c4=0;c4<nSectors;c4++)
    {
        readSector(sect,sdCardInfo.derivedFATData.fatLbaBgin + sectorsList[c4]);
        for(uint8_t c5=0;c5<nClusters;c5++)
        {
            if(clusterList[c5] >> 7 == sectorsList[c4])
            {
                *((uint32_t*)sect + (clusterList[c5] & 0x7F)) = 0x0;
            }
        }
        writeSector(sect,sdCardInfo.derivedFATData.fatLbaBgin + sectorsList[c4]);
    }

    // copy the last directory entry into the position of the directory entry to delete, if not the last
    // zero-out the last directory entry
    uint16_t delIdx=0;
    for (uint16_t c2=0;c2<parentDir->entriesLength;c2++)
    {
        if((uint32_t)parentDir->entries[c2] == (uint32_t)fp->dirEntry)
        {
            parentDir->entries[c2]->attrib = parentDir->entries[parentDir->entriesLength-1]->attrib;
            parentDir->entries[c2]->fileext[2] = parentDir->entries[parentDir->entriesLength-1]->fileext[2];
            parentDir->entries[c2]->fileext[1] = parentDir->entries[parentDir->entriesLength-1]->fileext[1];
            parentDir->entries[c2]->fileext[0] = parentDir->entries[parentDir->entriesLength-1]->fileext[0];

            for(uint8_t c2=0;c2<8;c2++)
            {
                parentDir->entries[c2]->filename[c2] = parentDir->entries[parentDir->entriesLength-1]->filename[c2];
            }

            parentDir->entries[c2]->firstCluster = parentDir->entries[parentDir->entriesLength-1]->firstCluster;
            parentDir->entries[c2]->size = parentDir->entries[parentDir->entriesLength-1]->size;
            delIdx=c2;
        }
    }
    parentDir->entries[parentDir->entriesLength-1]->attrib = 0;
    parentDir->entries[parentDir->entriesLength-1]->fileext[2] = 0;
    parentDir->entries[parentDir->entriesLength-1]->fileext[1] = 0;
    parentDir->entries[parentDir->entriesLength-1]->fileext[0] = 0;

    for(uint8_t c2=0;c2<8;c2++)
    {
        parentDir->entries[parentDir->entriesLength-1]->filename[c2] = 0;
    }

    parentDir->entries[parentDir->entriesLength-1]->firstCluster = 0;
    parentDir->entries[parentDir->entriesLength-1]->size = 0;
    
    // write the sector of the parent directory affected by the change
    uint8_t c=delIdx & 0x0F;
    readSector(sect,delIdx >> 4);
    for(uint8_t c2=0;c2<8;c2++)
    {
        *((char*)sect + (c<<5)+c2) = parentDir->entries[delIdx]->filename[c2];
    }
    *(sect + (c<<5)+8) = parentDir->entries[delIdx]->fileext[0];
    *(sect + (c<<5)+9) = parentDir->entries[delIdx]->fileext[1];
    *(sect + (c<<5)+10) = parentDir->entries[delIdx]->fileext[2];
    *(sect + (c<<5)+11) = parentDir->entries[delIdx]->attrib;
    *((uint16_t*)(sect + (c<<5)+0x14)) = parentDir->entries[delIdx]->firstCluster >> 16;
    *((uint16_t*)(sect + (c<<5)+0x1A)) = (uint16_t)(parentDir->entries[delIdx]->firstCluster & 0x0000FFFF);
    *((uint32_t*)(sect + (c<<5) + 0x1C)) = parentDir->entries[delIdx]->size;
    writeSector(sect,delIdx >> 4);
    return 0;
}

/**
 * @brief reads a file sector-wise, returns 0 if the file pointer is at the end
 * 
 * @param fp 
 * @param partitionInfo 
 * @param volumeId 
 * @return uint16_t 
 */
uint16_t readFile(FilePointerType * fp)
{
    uint16_t retcode = 0;
    readSector(fp->sectorBuffer,getClusterLba(fp->clusterPtr) + fp->sectorPtr);
    uint32_t bytes_read = (fp->sectorPtr + fp->clusterCntr * sdCardInfo.volumeId.sectorsPerCluster) << 9;
    if (bytes_read < fp->dirEntry->size)
    {
        if (bytes_read + 512 > fp->dirEntry->size)
        {
            for (uint16_t c = fp->dirEntry->size - (fp->sectorPtr << 9);c< 512;c++)
            {
                fp->sectorBuffer[c] = 0;
            }
            retcode = fp->dirEntry->size - (fp->sectorPtr << 9);
        }
        else
        {
            retcode = 512;
        }
        fp->sectorPtr ++;
        if(fp->sectorPtr >= sdCardInfo.volumeId.sectorsPerCluster)
        {
            fp->clusterPtr =  getNextCluster(fp->clusterPtr);
            fp->clusterCntr++;
            fp->sectorPtr = 0;
        }
    }
    return retcode;
}

uint32_t getNextFreeCluster()
{
    uint16_t sectorCnt=0;
    uint32_t res=0;
    uint8_t sector[512];
    uint32_t* fatEntries;
    readSector(sector,sdCardInfo.derivedFATData.fatLbaBgin);
    while(res==0 && sectorCnt < sdCardInfo.volumeId.sectorsPerFat)
    {
        readSector(sector,sdCardInfo.derivedFATData.fatLbaBgin + sectorCnt);
        fatEntries = (uint32_t*)sector;
        for(uint8_t c=0;c<128;c++)
        {
            if (*(fatEntries + c) == 0)
            {
                return res;
            }
            res++;
        }
    }
    return 0xFFFFFFFF; // error: disk full
}

#ifdef HARDWARE
uint32_t getNextCluster(uint32_t clusterNr)
{
    uint8_t sector[512];
    readSector(sector,sdCardInfo.derivedFATData.fatLbaBgin + (clusterNr >> 7));
    if ((clusterNr >> 7) == 0)
    {
        return *((uint32_t*)sector + (1 + (clusterNr & 0x7F)));
    }
    else
    {
        return *((uint32_t*)sector + (clusterNr & 0x7F));
    }
}
#else
uint32_t getNextCluster(uint32_t clusterNr)
{
    uint8_t sector[512];
    UInt32Type fatEntry;
    readSector(sector,sdCardInfo.derivedFATData.fatLbaBgin + (clusterNr >> 7));
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
uint8_t initFatSDCard()
{
    uint8_t retcode;
    uint8_t sector[512];
    for (uint16_t c=0;c<512;c++)
    {
        sector[c]=0;
    }
    retcode = readSector(sector,0);
    if (retcode != 0)
    {
        return retcode;
    }
    if(sector[510] != 0x55 || sector[511] != 0xAA)
    {
        return  FATLIB_WRONG_MBR_SIG;
    }
    getPartitionInfo(sector,0,&sdCardInfo.partitionInfo);
    if(sdCardInfo.partitionInfo.partitionType != 0x0C)
    {
        return FATLIB_WRONG_PART_TYPE;
    }
    readSector(sector,sdCardInfo.partitionInfo.lbaStart);
    if(sector[510] != 0x55 || sector[511] != 0xAA)
    {
        return  FATLIB_WRONG_VOLUMEID_SIG;
    }
    getVolumeId(sector,&sdCardInfo.volumeId);
    sdCardInfo.derivedFATData.fatLbaBgin = getFatLbaBegin(&sdCardInfo.partitionInfo,&sdCardInfo.volumeId);
    sdCardInfo.derivedFATData.clusterLbaBegin = getClusterLbaBegin(&sdCardInfo.partitionInfo,&sdCardInfo.volumeId);
    return 0;
}
