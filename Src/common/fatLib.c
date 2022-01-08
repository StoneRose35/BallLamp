#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "systemChoice.h"
#include "fatLib.h"
#include "stringFunctions.h"

#ifdef HARDWARE
#include "spi_sdcard_display.h"
typedef uint32_t ptr;
#else
static FILE * fid;
uint8_t * fakeFilesys;
#include "string.h"
typedef uint64_t ptr;
#endif

static SdCardInfoType sdCardInfo;


#ifndef HARDWARE
uint8_t readSector(uint8_t* res,uint64_t offset)
{
    for(uint16_t c=0;c<512;c++)
    {
        *(res + c) = *(fakeFilesys + (offset << 9) + c);
    }
    return 0;
}

uint8_t writeSector(uint8_t* res,uint64_t offset)
{
    for(uint16_t c=0;c<512;c++)
    {
        *(fakeFilesys + (offset << 9) + c)=*(res + c);
    }
    return 0;
}
#endif


#ifndef HARDWARE
void main(int argc,char ** argv)
{
    uint8_t sector[512];
    uint8_t retcode;
    PartitionInfoType partInfo;
    VolumeIdType volumeId;
    DirectoryEntryType * filesOfSector; 
    char fnameDisplay[12];
    uint8_t fcnt = 0x10;
    FilePointerType fp;
    DirectoryPointerType * dparent;
    DirectoryPointerType * dnew;
    const char * devicePath = "./fattestsmall.img";
    fp.dirEntry=0;
    
    // setup the fake filesys
    fid = fopen(devicePath,"rb");
    fseek(fid,0L,SEEK_END);
    uint64_t size = ftell(fid);
    fseek(fid,0L,SEEK_SET);
    fakeFilesys=(uint8_t*)malloc(size);
    fread(fakeFilesys,1,size,fid);
    fclose(fid);

    initFatSDCard();
    printf("Welcome to the FAT Access Library\r\n");

    printf("trying out Napa's rocking boot code\r\n");
    readSector(sector,0);
    char * napa="Kohl Rock Breisser King";
    uint8_t c=0;
    while(*(napa + c) != 0)
    {
        *(sector + c + 100)=*(napa + c);
        c++;
    }
    writeSector(sector,0);
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

    createDirectoryPointer(&dparent);
    openRootDirectory(dparent);
    uint16_t sec_cnt = 0;
    for (uint16_t c=0;c<dparent->entriesLength;c++)
    {
        if (((*dparent->entries + c)->attrib & 0x0F) != 0x0F)
        {
            uint8_t uu=0;
            for(uint8_t u=0;u<8;u++)
            {
                if((*dparent->entries + c)->filename[u] != ' ')
                {
                    fnameDisplay[uu++] = (*dparent->entries + c)->filename[u];
                }
            }
            if ((*dparent->entries + c)->attrib == 0x20)
            {
                fnameDisplay[uu++] = '.';
                for(uint8_t u=0;u<3;u++)
                {
                    if((*dparent->entries + c)->fileext[u] != ' ')
                    {
                        fnameDisplay[uu++] = (*dparent->entries + c)->fileext[u];
                    }
                }
            }
            fnameDisplay[uu]=0;
            if(strncmp(fnameDisplay,"TEST1",5)==0)
            {
                openFile(dparent,"TEST1.TXT",&fp);
            }
            printf("\tFilename: %s,\t Attrib: %i,\t Size %i,\t first Cluster: %i\r\n",fnameDisplay,(*dparent->entries + c)->attrib,(*dparent->entries + c)->size,(*dparent->entries + c)->firstCluster);
        }
    }
    if(fp.dirEntry != 0)
    {
        printf("\r\nPrinting File TEST1.TXT\r\n\r\n");
        while (readFile(&fp) > 0 )
        {
            printf("%s",fp.sectorBuffer);
        }
        printf("\r\n\r\n");
    }
    else
    {
        printf("file TEST1.TXT hasn't been found\r\n\r\n");
    }
    uint32_t nextCluster = 0;
    uint32_t prevCluster = volumeId.rootDirectoryFirstCluster;
    // test read the fat to see if the directory really is only contained in one cluster
    printf("testing cluster walk: starting off at cluster %X\r\n",prevCluster);
    while ((nextCluster & 0x0FFFFFF0) != 0x0FFFFFF0 )
    {
        nextCluster = getNextCluster(sector, prevCluster);
        printf("next cluster %X\r\n",nextCluster);
        prevCluster = nextCluster;
    }

    printf("testing library functions: openRootDirectory(DirectoryPointerType * fp)\r\n");
    createDirectoryPointer(&dnew);
    printf("opening subdirectory NDIR\r\n");
    openDirectory(dparent,"NDIR",dnew);

    copyDirectoryPointer(&dnew,&dparent);
    retcode = createDirectory(dparent,"MYDIR");
    if(retcode > 0)
    {
        printf("directory already exists\r\n");
        //openDirectory(dparent,"MYDIR",dnew);
        //deleteDirectory(dparent,dnew);
    }
    openDirectory(dparent,"MYDIR",dnew);
    copyDirectoryPointer(&dnew,&dparent);
    retcode = createFile(dparent,"log4.net");
    if (retcode == 0)
    {
        openFile(dparent,"log4.net",&fp);
        char * exampletext="This is not Log4J\r\nAs instead ... its a c-based recreation of the end of the world";
        uint16_t c1=0;
        while (*(exampletext + c1) != 0)
        {
            *(fp.sectorBuffer + c1) =  *(exampletext + c1);
            c1++;
        }
        *(fp.sectorBuffer + c1) = 0;
        writeFile(dparent,&fp,c1);
    }
    
    // copy back to drive
    fid = fopen(devicePath,"wb");
    fwrite(fakeFilesys,1,size,fid);
    fclose(fid);
}
#endif

/**
 * @brief Get the partition info from a sector containing the partition table
 * Sector-Read 
 * 
 * @param sect the sector (byte array of length 512) 
 * @param partNr partition number (from 0 to 3)
 * @param res the structure to be filled
 */
void getPartitionInfo(uint8_t *sect,uint8_t partNr,PartitionInfoType* res)
{
    res->partitionType = *((uint8_t*)(sect + 446 + (partNr << 6 ) + 4));
    res->lbaStart = *((uint32_t*)(sect + 446 + (partNr << 6) + 8));
    res->nrSectors = *((uint32_t*)(sect + 446 + (partNr << 6) + 12));
}

/**
 * @brief Get the partition info from a sector containing the volume info (fist sector of the partition)
 * 
 * @param sect the sector (byte array of length 512) 
 * @param res the structure to be filled
 */
void getVolumeId(uint8_t * sect,VolumeIdType* res)
{
    res->bytesPerSector = *((uint16_t*)(sect + 0x0B));
    res->sectorsPerCluster = *(sect + 0x0D);
    res->numberOfReservedSectors = *((uint16_t*)(sect + 0xE));
    res->numberOfFats = *(sect + 0x10);
    res->sectorsPerFat = *((uint32_t*)(sect + 0x24));
    res->rootDirectoryFirstCluster = *((uint32_t*)(sect + 0x2C));
}

/**
 * @brief get the LBA Adress of the Beginning of the FAT
 * 
 * @param partInfo partition info object, must be initialized
 * @param volumeId volume info object, must be initialized
 * @return uint32_t the absolute offset in sectors of the FAT
 */
uint32_t getFatLbaBegin(PartitionInfoType* partInfo,VolumeIdType* volumeId)
{
    return partInfo->lbaStart + volumeId->numberOfReservedSectors;
}

/**
 * @brief get the LBA Adress of the Beginning of the Cluster (Data area)
 * 
 * @param partInfo partition info object, must be initialized
 * @param volumeId volume info object, must be initialized
 * @return uint32_t the absolute offset in sectors of Cluster / Data area
 */
uint32_t getClusterLbaBegin(PartitionInfoType* partInfo,VolumeIdType* volumeId)
{
    return partInfo->lbaStart + volumeId->numberOfReservedSectors + (volumeId->numberOfFats * volumeId->sectorsPerFat);
}

/**
 * @brief Get the absolute LBA Adress of a given Cluster in the currently accessed partition
 * 
 * @param clusterNr the number of the cluster for which the lba address should be computed, start at 2
 * @return uint32_t the absolute lba address of the start of the cluster
 */
uint32_t getClusterLba(uint32_t clusterNr)
{
    return sdCardInfo.derivedFATData.clusterLbaBegin + (clusterNr - 2)*sdCardInfo.volumeId.sectorsPerCluster;
}

/**
 * @brief read the directory entries from a sector containing a directory of a fraction of a directory
 * 
 * @param sect the sector to read from (byte array of size 512)
 * @param entries pointer to the array containing the directory entreis, must be of size 16 at least
 * @return uint16_t the number of entries read
 */
uint16_t getDirectoryEntries(uint8_t * sect,DirectoryEntryType** entries)
{
    uint16_t c=0,cread=0;
    while(*(sect + (c<<5))!=0 && cread < 16)
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
 * @brief Get the Next Free Cluster object
 * 
 * @return uint32_t the cluster number if successful, 0xFFFFFFFF if no cluster could be found, meaning: the disk ist full
 */
uint32_t getNextFreeCluster()
{
    uint16_t sectorCnt=0;
    uint32_t res=0;
    uint8_t sector[512];
    uint32_t* fatEntries;
    readSector(sector,sdCardInfo.derivedFATData.fatLbaBegin);
    while(res==0 && sectorCnt < sdCardInfo.volumeId.sectorsPerFat)
    {
        readSector(sector,sdCardInfo.derivedFATData.fatLbaBegin + sectorCnt);
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
    return FATLIB_DISK_FULL; // error: disk full
}


uint32_t getNextCluster(uint8_t * sector, uint32_t clusterNr)
{
    readSector(sector,sdCardInfo.derivedFATData.fatLbaBegin + (clusterNr >> 7));
    return *((uint32_t*)sector + (clusterNr & 0x7F));
}

uint32_t getPreviousCluster(uint8_t * sector,uint32_t clusterNr)
{
    uint32_t c=0;
    uint32_t c2;
    uint32_t backwardsIdx=2;
    while (c < sdCardInfo.volumeId.sectorsPerFat)
    {
        readSector(sector,c);
        if (c==0)
        {
            c2=2;
        }
        else
        {
            c2=0;
        }
        while(c2 < 128)
        {
            if (*((uint32_t*)sector + c2) == clusterNr)
            {
                return backwardsIdx;
            }
            c2++;
            backwardsIdx++;
        }
        c++;
    }
    return 0;
}


/**
 * @brief deep-copies a directory information structure. Useful for directory walks after a directory has been opened
 * 
 * @param src copy source, the data pointed to remains untouched
 * @param dest copy destination
 * @return uint8_t 0 if successful, error code otherwise
 */
uint8_t copyDirectoryPointer(DirectoryPointerType ** src,DirectoryPointerType ** dest)
{
    (*dest)->clusterLbaPtr = (*src)->clusterLbaPtr;
    if((*dest)->entriesLength < (*src)->entriesLength)
    {
        *((*dest)->entries)=realloc(*((*dest)->entries),(*src)->entriesLength*sizeof(DirectoryEntryType));
        if(*((*dest)->entries)==NULL)
        {
            return FATLIB_MEMORY_ALLOC_FAILURE;
        }
    }
    for(uint8_t c=0;c<(*src)->entriesLength;c++)
    {
        (*((*dest)->entries) + c)->attrib = (*((*src)->entries) + c)->attrib;
        (*((*dest)->entries) + c)->fileext[0]=(*((*src)->entries) + c)->fileext[0];
        (*((*dest)->entries) + c)->fileext[1]=(*((*src)->entries) + c)->fileext[1];
        (*((*dest)->entries) + c)->fileext[2]=(*((*src)->entries) + c)->fileext[2];
        for(uint8_t c2=0;c2<8;c2++)
        {
            (*((*dest)->entries) + c)->filename[c2]=(*((*src)->entries) + c)->filename[c2];
        }
        (*((*dest)->entries) + c)->firstCluster=(*((*src)->entries) + c)->firstCluster;
        (*((*dest)->entries) + c)->size = (*((*src)->entries) + c)->size;
    } 
    (*dest)->entriesLength = (*src)->entriesLength;
    if((*src)->dirEntry != 0)
    {
        if ((*dest)->dirEntry == 0)
        {
            (*dest)->dirEntry = (DirectoryEntryType*)malloc(sizeof(DirectoryEntryType));
        }
        (*dest)->dirEntry->attrib = (*src)->dirEntry->attrib;
        (*dest)->dirEntry->fileext[0]=(*src)->dirEntry->fileext[0];
        (*dest)->dirEntry->fileext[1]=(*src)->dirEntry->fileext[1];
        (*dest)->dirEntry->fileext[2]=(*src)->dirEntry->fileext[2];
        for(uint8_t c=0;c<8;c++)
        {
            (*dest)->dirEntry->filename[c]=(*src)->dirEntry->filename[c];
        }
        (*dest)->dirEntry->firstCluster=(*src)->dirEntry->firstCluster;
        (*dest)->dirEntry->size = (*src)->dirEntry->size;
    }
    else
    {
        if((*dest)->dirEntry != 0)
        {
            free((*dest)->dirEntry);
            (*dest)->dirEntry=0;
        }
    }
    return 0;
}

/**
 * @brief allocates space for a directory pointer, aka constructor
 * 
 * @param dp directory pointer passed by reference
 * @return uint8_t always zero
 */
uint8_t createDirectoryPointer(DirectoryPointerType ** dp)
{
    *dp=(DirectoryPointerType*)malloc(sizeof(DirectoryPointerType));
    (*dp)->entries = (DirectoryEntryType**)malloc(sizeof(ptr));
    (*dp)->entriesLength=0;
    (*dp)->clusterLbaPtr=0;
    (*dp)->dirEntry = (DirectoryEntryType*)malloc(sizeof(DirectoryEntryType));
    return 0;
}

/**
 * @brief frees the space used by a directory pointer, aka destructor
 * 
 * @param dp directory pointer passed by reference
 * @return uint8_t always zero
 */
uint8_t destroyDirectoryPointer(DirectoryPointerType ** dp)
{
    if((*dp)->entriesLength > 0)
    {
        free(*(*dp)->entries);
    }
    free((*dp)->entries);
    free((*dp)->dirEntry);
    free(*dp);
    *dp=0;
    return 0;
}

/**
 * @brief copies a directory entry to a sector of the directory fp
 * Sector-Write
 * @param sect sector buffer passed by reference to avoid stack growths
 * @param position absolute position within the directory
 * @param fp the directory in which the entry should be save
 * @param entry the entry to save
 */
void entryToSector(uint8_t * sect, uint8_t position,DirectoryPointerType * fp,DirectoryEntryType * entry)
{
    uint8_t c,c2;
    readSector(sect,fp->clusterLbaPtr + (position >> 4));
    c=position & 0x0F;
    for(c2=0;c2<32;c2++) // clear directory entry
    {
        *(sect + (c<<5)+c2)=0;
    } 
    // fill in values
    for(c2=0;c2<8;c2++)
    {
        *((char*)sect + (c<<5)+c2) = entry->filename[c2];
    }
    *(sect + (c<<5)+8) = entry->fileext[0];
    *(sect + (c<<5)+9) = entry->fileext[1];
    *(sect + (c<<5)+10) = entry->fileext[2];
    *(sect + (c<<5)+11) = entry->attrib;
    *((uint16_t*)(sect + (c<<5)+0x14)) = entry->firstCluster >> 16;
    *((uint16_t*)(sect + (c<<5)+0x1A)) = (uint16_t)(entry->firstCluster & 0x0000FFFF);
    *((uint32_t*)(sect + (c<<5) + 0x1C)) = entry->size;
    writeSector(sect,fp->clusterLbaPtr + (position >> 4));

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
    resBfr = (DirectoryEntryType*)malloc(0x20*sizeof(DirectoryEntryType));
    *res = (DirectoryEntryType*)malloc(0x20*sizeof(DirectoryEntryType));
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

/**
 * @brief returns the number of files and subfolders in a directory exclusiding the dot-entries
 * 
 */
uint16_t getDirectoryContentSize(DirectoryPointerType * fp)
{
    uint16_t contentCnt=0;
    uint16_t c=0;
    for(c=0;c<fp->entriesLength;c++)
    {
        if ((*fp->entries + c)->filename[0]!=0xE5)
        {
            if ((*fp->entries + c)->attrib == 0x10 ) // directory
            {
                if((*fp->entries + c)->filename[0] == '.')
                {
                    uint8_t onlyDots=1;
                    for(uint8_t c2=1;c2<8;c2++)
                    {
                        if ((*fp->entries + c)->filename[c2]=='.')
                        {
                            onlyDots++;
                        }
                        else if ((*fp->entries + c)->filename[c2]!=' ')
                        {
                            onlyDots = 0;
                        }
                    }
                    if(onlyDots==0)
                    {
                        contentCnt++;
                    }
                }
                else
                {
                    contentCnt++;
                }
            }
            else if ((*fp->entries + c)->attrib == 0x20 ) // directory
            {
                contentCnt++;
            }
        }
    }
    return contentCnt;
}

/**
 * @brief opens a file, stores the information pointer in fp
 * 
 * @param parentDir the directory which contains the file to be opened
 * @param filename the name of the file to be opened
 * @param fp the file pointer structure used to handle the file (read, write, delete)
 * @return uint8_t 0 if successful, an errorcode otherwise
 */
uint8_t openFile(DirectoryPointerType * parentDir,char * filename,FilePointerType * fp)
{
    uint16_t fileIndex=0xFFFF;
    for (uint16_t c=0;c<(parentDir)->entriesLength;c++)
    {
        if ((*(parentDir)->entries+c)->attrib == 0x20)
        {
            if (filenameEquals(*(parentDir)->entries+c,filename)==1)
            {
                fileIndex = c;
            }
        }
    }
    if (fileIndex == 0xFFFF)
    {
        return FATLIB_FILE_NOT_FOUND;
    }

    fp->dirEntry = (*(parentDir)->entries + fileIndex);
    fp->sectorPtr = 0;
    fp->clusterPtr = fp->dirEntry->firstCluster;
    fp->clusterCntr = 0;
    return 0;
}

/**
 * @brief opens the root directory, the sc card must be initialized to do this
 * 
 * @param fp the root folder, note that dirEntry remains NULL since the root folder is not a child of any other folder
 * @return uint8_t always 0
 */
uint8_t openRootDirectory(DirectoryPointerType * fp)
{
    fp->clusterLbaPtr = getClusterLba(sdCardInfo.volumeId.rootDirectoryFirstCluster);
    fp->entriesLength = getFilesInDirectory(fp->clusterLbaPtr,fp->entries);
    fp->dirEntry = 0;
    return 0;
}

/**
 * @brief opens a directory givens it's name and the parent directory context
 * 
 * @param parentDir the directory to be searched for the folder name
 * @param dirname the name of the folder to be opened
 * @param fp pointer to the opened folder (if successful)
 * @return uint8_t 0 of successful, an errorcode otherwise
 */
uint8_t openDirectory(DirectoryPointerType * parentDir,char * dirname,DirectoryPointerType * fp)
{
    uint8_t dirFound=0;
    for (uint8_t c=0;c<parentDir->entriesLength;c++)
    {
        if(((*parentDir->entries + c)->attrib & 0x10) == 0x10 && startsWith((*parentDir->entries + c)->filename,dirname)== 1)
        {
            if(fp->dirEntry == 0)
            {
                fp->dirEntry = (DirectoryEntryType*)malloc(sizeof(DirectoryEntryType));
            }
            //fp->dirEntry = *parentDir->entries + c;
            fp->dirEntry->attrib = (*parentDir->entries + c)->attrib;
            fp->dirEntry->fileext[0] = (*parentDir->entries + c)->fileext[0];
            fp->dirEntry->fileext[1] = (*parentDir->entries + c)->fileext[1];
            fp->dirEntry->fileext[2] = (*parentDir->entries + c)->fileext[2];
            for(uint8_t c2=0;c2<8;c2++)
            {
                fp->dirEntry->filename[c2] = (*parentDir->entries + c)->filename[c2];
            }
            fp->dirEntry->firstCluster = (*parentDir->entries + c)->firstCluster;
            fp->dirEntry->size = (*parentDir->entries + c)->size;
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
        fp->entriesLength = getFilesInDirectory(fp->clusterLbaPtr,fp->entries);
        return 0;
    }
}

/**
 * @brief Create a Directory in the given parent directory
 * 
 * @param fp the directory in which the new directory should be create
 * @param directoryName name of the directory to be created, must be unique within the parent directory
 * @return uint8_t 0 if successful, error code otherwise
 */
uint8_t createDirectory(DirectoryPointerType * fp,char * directoryName)
{
    uint8_t sect[512];
    uint32_t freeCluster;
    DirectoryEntryType *entriesBfr;
    uint8_t c=0;
    uint8_t fnameEnd=0;

    //check if filename already exists
    uint8_t fnameEqual=0;
    uint16_t c5=0;
    while(c5<fp->entriesLength && fnameEqual==0)
    {
        if( ((*fp->entries + c5)->attrib & 0x10) == 0x10 && (*fp->entries + c5)->attrib != 0x0F)
        {
            fnameEqual = filenameEquals((*fp->entries + c5),directoryName);
        }
        c5++;
    }
    if(fnameEqual==1)
    {
        return FATLIB_CREATEDIR_ALREADY_EXISTS;
    }

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
    readSector(sect,sdCardInfo.derivedFATData.fatLbaBegin + (freeCluster >> 7));
    *((uint32_t*)sect + (freeCluster & 0x7F))=0x0FFFFFFF; 
    writeSector(sect,sdCardInfo.derivedFATData.fatLbaBegin +  (freeCluster >> 7));

    // add a directory entry for the newly created directory
    (*fp->entries + fp->entriesLength)->attrib =0x10;
    (*fp->entries + fp->entriesLength)->fileext[0]=' ';
    (*fp->entries + fp->entriesLength)->fileext[1]=' ';
    (*fp->entries + fp->entriesLength)->fileext[2]=' ';
    for (c=0;c<8;c++)
    {
        if(*((uint8_t*)directoryName + c) == 0)
        {
            fnameEnd=1;
        }
        if(fnameEnd==0)
        {
            (*fp->entries + fp->entriesLength)->filename[c] = *(directoryName + c);
        }
        else
        {
            (*fp->entries + fp->entriesLength)->filename[c] = ' ';
        }

    }
    (*fp->entries + fp->entriesLength)->firstCluster = freeCluster;
    (*fp->entries + fp->entriesLength)->size = 0;
    
    
    // write the sector of the parent directory containing the entry of the new directory
    entryToSector(sect,fp->entriesLength,fp,(*fp->entries + fp->entriesLength));

    fp->entriesLength++;

    // generate the . and .. entries in the new directory
    DirectoryPointerType * ndir;
    createDirectoryPointer(&ndir);
    openDirectory(fp,directoryName,ndir);
    DirectoryEntryType dotEntry;
    dotEntry.attrib = 0x10;
    dotEntry.fileext[0]=' ';
    dotEntry.fileext[1]=' ';
    dotEntry.fileext[2]=' ';
    dotEntry.filename[0]='.';
    for(uint8_t c3=1;c3<8;c3++)
    {
        dotEntry.filename[c3]=' ';
    }
    dotEntry.firstCluster = freeCluster;
    dotEntry.size=0;
    entryToSector(sect,0,ndir,&dotEntry);
    dotEntry.attrib = 0x10;
    dotEntry.fileext[0]=' ';
    dotEntry.fileext[1]=' ';
    dotEntry.fileext[2]=' ';
    dotEntry.filename[0]='.';
    dotEntry.filename[1]='.';
    for(uint8_t c3=2;c3<8;c3++)
    {
        dotEntry.filename[c3]=' ';
    }
    dotEntry.firstCluster = fp->dirEntry->firstCluster;
    dotEntry.size=0;
    entryToSector(sect,1,ndir,&dotEntry);
    destroyDirectoryPointer(&ndir);
    return 0;
}

uint8_t createFile(DirectoryPointerType * fp,char * fileName)
{
    uint8_t sect[512];
    uint32_t freeCluster;
    DirectoryEntryType *entriesBfr;
    uint8_t c=0;
    uint8_t fnameEnd=0;
    uint8_t dotpos=0;

    //check if filename already exists
    uint8_t fnameEqual=0;
    uint16_t c5=0;
    while(c5<fp->entriesLength && fnameEqual==0)
    {
        fnameEqual = filenameEquals((*fp->entries+c5++),fileName);
    }
    if(fnameEqual==1)
    {
        return FATLIB_CREATEDIR_ALREADY_EXISTS;
    }

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
    readSector(sect,sdCardInfo.derivedFATData.fatLbaBegin + (freeCluster >> 7));
    *((uint32_t*)sect + (freeCluster & 0x7F))=0x0FFFFFFF; 
    writeSector(sect,sdCardInfo.derivedFATData.fatLbaBegin +  (freeCluster >> 7));

    // add a file entry for the newly created file
    
    for (c=0;c<8;c++)
    {
        if(*((uint8_t*)fileName + c) == 0)
        {
            fnameEnd=1;
        }
        else if (*(fileName + c) == '.')
        {
            fnameEnd = 1;
            dotpos = c;
        }
        if(fnameEnd==0)
        {
            (*fp->entries + fp->entriesLength)->filename[c] = *(fileName + c);
        }
        else
        {
            (*fp->entries + fp->entriesLength)->filename[c] = ' ';
        }
    }

    (*fp->entries + fp->entriesLength)->attrib =0x20;
    if (fileName[dotpos+1] != 0)
    {
        (*fp->entries + fp->entriesLength)->fileext[0]=fileName[dotpos+1];
    }
    else
    {
        (*fp->entries + fp->entriesLength)->fileext[0]=' ';
    }
    if (fileName[dotpos+2] != 0)
    {
        (*fp->entries + fp->entriesLength)->fileext[1]=fileName[dotpos+2];
    }
    else
    {
        (*fp->entries + fp->entriesLength)->fileext[1]=' ';
    }
    if (fileName[dotpos+3] != 0)
    {
        (*fp->entries + fp->entriesLength)->fileext[2]=fileName[dotpos+3];
    }
    else
    {
        (*fp->entries + fp->entriesLength)->fileext[2]=' ';
    }


    (*fp->entries + fp->entriesLength)->firstCluster = freeCluster;
    (*fp->entries + fp->entriesLength)->size = 0;
    
    // write the sector of the parent directory containing the entry of the new file
    entryToSector(sect,fp->entriesLength,fp,(*fp->entries + fp->entriesLength));

    fp->entriesLength++;
    return 0;
}

/**
 * @brief deletes a directory, only deletes it if empty
 * 
 * @param parentDir the directory containing the file to delete
 * @param fp the directory to delete
 * @return uint8_t 0 if successful, an error code otherwise
 */
uint8_t deleteDirectory(DirectoryPointerType * parentDir,DirectoryPointerType * fp)
{
    uint8_t sect[512];
    uint8_t nClusters=0,nSectors=0;
    uint32_t clusterList[FATLIB_FOLDER_MAX_CLUSTERS];
    uint32_t sectorsList[FATLIB_FOLDER_MAX_CLUSTERS];

    if (getDirectoryContentSize(fp)!=0)
    {
        return FATLIB_DIRECTORY_NOT_EMPTY;
    }
    // write 0x00000000 to all clusters of the directory
    uint32_t prevCluster = fp->dirEntry->firstCluster;
    uint32_t nextCluster = 0;
    clusterList[nClusters++]=prevCluster;
    while ((nextCluster & 0x0ffffff0) != 0x0ffffff0)
    {
        nextCluster = getNextCluster(sect, prevCluster);

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
    for(uint8_t c2=0;c2<nClusters-1;c2++)
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
        readSector(sect,sdCardInfo.derivedFATData.fatLbaBegin + sectorsList[c4]);
        for(uint8_t c5=0;c5<nClusters-1;c5++)
        {
            if(clusterList[c5] >> 7 == sectorsList[c4])
            {
                *((uint32_t*)sect + (clusterList[c5] & 0x7F)) = 0x0;
            }
        }
        writeSector(sect,sdCardInfo.derivedFATData.fatLbaBegin + sectorsList[c4]);
    }

    // zero-out the entry to delete
    uint16_t delIdx=0;
    for (uint16_t c2=0;c2<parentDir->entriesLength;c2++)
    {
        uint8_t fnameEqual=1;
        if ((*parentDir->entries + c2)->attrib == 0x10)
        {
            for(uint8_t c3=0;c3<8;c3++)
            {
                if ((*parentDir->entries + c2)->filename[c3] != fp->dirEntry->filename[c3])
                {
                    fnameEqual=0;
                }
            }
        }
        if (fnameEqual == 1)
        {
            delIdx=c2;
        }
    }

    (*parentDir->entries + delIdx)->attrib = 0;
    (*parentDir->entries + delIdx)->fileext[2] = 0;
    (*parentDir->entries + delIdx)->fileext[1] = 0;
    (*parentDir->entries + delIdx)->fileext[0] = 0;

    (*parentDir->entries + delIdx)->filename[0]=0xE5; //mark as deleted
    for(uint8_t c2=1;c2<8;c2++)
    {
        (*parentDir->entries + delIdx)->filename[c2] = 0;
    }

    (*parentDir->entries + delIdx)->firstCluster = 0;
    (*parentDir->entries + delIdx)->size = 0;
    
    // write the sector of the parent directory affected by the change
    entryToSector(sect,delIdx,parentDir,(*parentDir->entries + delIdx));
    return 0;
}


uint8_t deleteFile(DirectoryPointerType * parentDir,FilePointerType * fp)
{
    uint8_t sect[512];
    uint8_t nClusters=0,nSectors=0;
    uint32_t clusterList[FATLIB_FOLDER_MAX_CLUSTERS];
    uint32_t sectorsList[FATLIB_FOLDER_MAX_CLUSTERS];

    // write 0x00000000 to all clusters of the file
    uint32_t prevCluster = fp->dirEntry->firstCluster;
    uint32_t nextCluster = 0;
    clusterList[nClusters++]=prevCluster;
    while ((nextCluster & 0x0ffffff0) != 0x0ffffff0)
    {
        nextCluster = getNextCluster(sect, prevCluster);

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
    for(uint8_t c2=0;c2<nClusters-1;c2++)
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
        readSector(sect,sdCardInfo.derivedFATData.fatLbaBegin + sectorsList[c4]);
        for(uint8_t c5=0;c5<nClusters-1;c5++)
        {
            if(clusterList[c5] >> 7 == sectorsList[c4])
            {
                *((uint32_t*)sect + (clusterList[c5] & 0x7F)) = 0x0;
            }
        }
        writeSector(sect,sdCardInfo.derivedFATData.fatLbaBegin + sectorsList[c4]);
    }

    // zero-out the entry to delete
    uint16_t delIdx=0;
    for (uint16_t c2=0;c2<parentDir->entriesLength;c2++)
    {
        uint8_t fnameEqual=1;
        if ((*parentDir->entries + c2)->attrib == 0x20)
        {
            for(uint8_t c3=0;c3<8;c3++)
            {
                if ((*parentDir->entries + c2)->filename[c3] != fp->dirEntry->filename[c3])
                {
                    fnameEqual=0;
                }
            }
        }
        if (fnameEqual == 1)
        {
            if (((*parentDir->entries + c2)->fileext[0] != fp->dirEntry->fileext[0]) ||
                ((*parentDir->entries + c2)->fileext[1] != fp->dirEntry->fileext[1]) ||
                ((*parentDir->entries + c2)->fileext[2] != fp->dirEntry->fileext[2])
               )
            {
                fnameEqual = 0;
            }
            if (fnameEqual == 1)
            {
                delIdx=c2;
            }
        }
    }

    (*parentDir->entries + delIdx)->attrib = 0;
    (*parentDir->entries + delIdx)->fileext[2] = 0;
    (*parentDir->entries + delIdx)->fileext[1] = 0;
    (*parentDir->entries + delIdx)->fileext[0] = 0;

    (*parentDir->entries + delIdx)->filename[0]=0xE5; //mark as deleted
    for(uint8_t c2=1;c2<8;c2++)
    {
        (*parentDir->entries + delIdx)->filename[c2] = 0;
    }

    (*parentDir->entries + delIdx)->firstCluster = 0;
    (*parentDir->entries + delIdx)->size = 0;
    
    // write the sector of the parent directory affected by the change
    entryToSector(sect,delIdx,parentDir,(*parentDir->entries + delIdx));
    return 0;
}

/**
 * @brief reads a file sector-wise, returns 0 if the file pointer is at the end
 * 
 * @param fp the file to read
 * @return uint16_t the number of bytes read, is smaller that 512 if the last sector has been read and 0 if the file has already been read
 */
uint16_t readFile(FilePointerType * fp)
{
    uint8_t sector[512];
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
        if(fp->sectorPtr > sdCardInfo.volumeId.sectorsPerCluster)
        {
            fp->clusterPtr =  getNextCluster(sector,fp->clusterPtr);
            fp->clusterCntr++;
            fp->sectorPtr = 0;
        }
    }
    return retcode;
}

/**
 * @brief write the content of the file-internal sector buffer into file at the given sector
 * meant to be used to append content to a file
 * 
 * @param fp 
 * @return uint16_t 
 */
uint16_t writeFile(DirectoryPointerType * parentDir,FilePointerType * fp,uint16_t nrbytes)
{
    uint8_t sector[512];
    uint16_t retcode = 0;
    writeSector(fp->sectorBuffer,getClusterLba(fp->clusterPtr) + fp->sectorPtr);
    fp->sectorPtr ++;
    if(fp->sectorPtr > sdCardInfo.volumeId.sectorsPerCluster)
    {
        uint32_t oldcluster = fp->clusterPtr;
        fp->clusterPtr =  getNextFreeCluster(sector,fp->clusterPtr);
        fp->clusterCntr++;
        fp->sectorPtr = 0;

        readSector(sector,sdCardInfo.derivedFATData.fatLbaBegin + (fp->clusterPtr >> 7));
        *((uint32_t*)sector + (fp->clusterPtr & 0x7F))=0x0FFFFFFF; 
        writeSector(sector,sdCardInfo.derivedFATData.fatLbaBegin +  (fp->clusterPtr >> 7));

        readSector(sector,sdCardInfo.derivedFATData.fatLbaBegin + (oldcluster >> 7));
        *((uint32_t*)sector + (oldcluster & 0x7F))=fp->clusterPtr; 
        writeSector(sector,sdCardInfo.derivedFATData.fatLbaBegin +  (oldcluster >> 7));
    }
    // update the file directory entry
    uint8_t updIdx=0;
    for(uint8_t c=0;c<parentDir->entriesLength;c++)
    {
        uint8_t fnameEqual=1;
        if ((*parentDir->entries + c)->attrib == 0x20)
        {
            for(uint8_t c3=0;c3<8;c3++)
            {
                if (((*parentDir->entries + c))->filename[c3] != fp->dirEntry->filename[c3])
                {
                    fnameEqual=0;
                }
            }
            if (fnameEqual == 1)
            {
                if ((((*parentDir->entries + c))->fileext[0] != fp->dirEntry->fileext[0]) ||
                    (((*parentDir->entries + c))->fileext[1] != fp->dirEntry->fileext[1]) ||
                    (((*parentDir->entries + c))->fileext[2] != fp->dirEntry->fileext[2])
                )
                {
                    fnameEqual = 0;
                }
                if (fnameEqual == 1)
                {
                    updIdx=c;
                }
            }
        }

    }
    ((*parentDir->entries + updIdx))->size += nrbytes;
    // write the sector of the parent directory affected by the change
    entryToSector(sector,updIdx,parentDir,(*parentDir->entries + updIdx));
    return retcode;
}

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
    sdCardInfo.derivedFATData.fatLbaBegin = getFatLbaBegin(&sdCardInfo.partitionInfo,&sdCardInfo.volumeId);
    sdCardInfo.derivedFATData.fat2LbaBegin = sdCardInfo.derivedFATData.fatLbaBegin + sdCardInfo.volumeId.sectorsPerFat;
    sdCardInfo.derivedFATData.clusterLbaBegin = getClusterLbaBegin(&sdCardInfo.partitionInfo,&sdCardInfo.volumeId);
    return 0;
}



uint8_t filenameEquals(DirectoryEntryType * entry,char * fileName)
{
    uint8_t hasEnded=0;
    uint8_t dotpos=8;
    uint8_t fnameEqual;
    fnameEqual=1;
    for(uint8_t c=0;c<8;c++)
    {
        if (fileName[c]==0)
        {
            hasEnded=1;
        }
        else if (fileName[c]=='.')
        {
            hasEnded=1;
            dotpos = c;
        }
        if ((entry->filename[c] != fileName[c] && hasEnded==0) ||
            (hasEnded==1 && entry->filename[c] != ' '))
        {
            fnameEqual=0;
        }
    }
    if (fnameEqual == 1 && dotpos < 8)
    {
        uint8_t c6=dotpos+1;
        uint8_t c7=0;
        while(fileName[c6] !=0 && c7 < 3)
        {
            if (entry->fileext[c7] != fileName[c6])
            {
                fnameEqual = 0;
            }
            c6++;
            c7++;
        }
    }
    return fnameEqual;
}

