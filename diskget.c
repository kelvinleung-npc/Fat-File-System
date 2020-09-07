#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "Constants.h"
//Student Name: Kelvin Leung
//Student Number: V00927380

typedef struct{
    char FileSystemName[9]; 
    int BlockSize; 
    int BlockCount; 
    int FatStart; 
    int FatBlocks; 
    int RootDirectoryStart; 
    int RootDirectoryBlocks;
}SuperBlock; 

    typedef struct{
        char Status;
        int StartingBlock; 
        int NumberofBlocks; 
        __uint32_t FileSize; 
        __uint32_t CreateTime[6];
        __uint32_t ModifyTime[6]; 
        unsigned char FileName[32]; 
    }RootDirectory; 

int main(int argc, char** argv)
{
    FILE *fp; 
    fp = fopen(argv[1], "rb");
    char filename[32]; 
    strncpy(filename, argv[2], 32); 
    printf("%s", filename);  
    if(!fp)
    {
        printf("yikes"); 
    }

    SuperBlock Info; 
     
    char *buffer;
    buffer = (char*)malloc(9); 

    int readvalue = fread(buffer, 1,8,fp);
    memcpy(Info.FileSystemName, buffer, 9);  
    readvalue = fread(buffer, 1, 2, fp);
    Info.BlockSize = buffer[0] << 8| buffer[1]; 
    readvalue = fread(buffer, 1, 4, fp);
    Info.BlockCount = buffer[0] << 24| buffer[1] << 16| buffer[2] << 8| buffer[3]; 
    readvalue = fread(buffer, 1, 4, fp);
    Info.FatStart = buffer[0] << 24| buffer[1] << 16| buffer[2] << 8| buffer[3];      
    readvalue = fread(buffer, 1, 4, fp);
    Info.FatBlocks = buffer[0] << 24| buffer[1] << 16| buffer[2] << 8| buffer[3];
    readvalue = fread(buffer, 1, 4, fp);
    Info.RootDirectoryStart = buffer[0] << 24| buffer[1] << 16| buffer[2] << 8| buffer[3];
    readvalue = fread(buffer, 1, 4, fp);
    Info.RootDirectoryBlocks = buffer[0] << 24| buffer[1] << 16| buffer[2] << 8| buffer[3];
    if(readvalue == 0){
        printf("something went wrong"); 
    }
    
    
    int RootNum = 100; 
    RootDirectory *Dir =  (RootDirectory*)malloc(sizeof(RootDirectory)* RootNum); 
    fseek(fp, (Info.RootDirectoryStart * Info.BlockSize), SEEK_SET); 
    int RootBytes = Info.RootDirectoryBlocks * Info.BlockSize; 
    int DirectoryEntries = RootBytes/64; 
    int j = 0;
    unsigned char buffer1[50]; 
    __uint32_t buffer2[8]; 
    int combine = 0; 
    int chartest; 
    int foundname = 1; 
    for(int i = 0; i < DirectoryEntries; i++){
        int statusread = fread(buffer1, 1,1, fp); 
        if((int) buffer1[0] == 0){
            fseek(fp, 63, SEEK_CUR); 
        }
        else{
            chartest = (int)buffer1[0]; 
            if(chartest == 3){
            Dir[j].Status = 'F'; 
            }
            if(chartest == 5){
                Dir[j].Status = 'D';
            }
            int rv = fread(buffer1, 1, 4, fp);
            Dir[j].StartingBlock = buffer1[0] << 24| buffer1[1] << 16| buffer1[2] << 8| buffer1[3];
            rv = fread(buffer1, 1, 4, fp); 
            Dir[j].NumberofBlocks = buffer1[0] << 24| buffer1[1] << 16| buffer1[2] << 8| buffer1[3];
            rv = fread(buffer1, 1, 4, fp); 
            Dir[j].FileSize = (__uint32_t ) buffer1[0] << 24| buffer1[1] << 16| buffer1[2] << 8| (buffer1[3] & 0b11111111);
            rv = fread(buffer1, 1, 7, fp); 
            combine = buffer1[0] << 8 | buffer1[1]; 
            buffer2[0] = combine; 
            buffer2[1] = buffer1[2];
            buffer2[2] = buffer1[3];
            buffer2[3] = buffer1[4];
            buffer2[4] = buffer1[5];
            buffer2[5] = buffer1[6];
            memcpy(Dir[j].CreateTime, buffer2, 6); 
            rv = fread(buffer1, 1, 7, fp); 
            combine = buffer1[0] << 8 | buffer1[1]; 
            buffer2[0] = combine; 
            buffer2[1] = buffer1[2];
            buffer2[2] = buffer1[3];
            buffer2[3] = buffer1[4];
            buffer2[4] = buffer1[5];
            buffer2[5] = buffer1[6];
            for(int i = 0; i < 6; i++){
                Dir[j].ModifyTime[i] = buffer2[i];
            }
            
             
            rv = fread(buffer1, 1, 31, fp);  
            if(rv != 0){
            }
            buffer1[31] = '\0'; 
            memcpy(Dir[j].FileName, buffer1, 32);
            Dir[j].FileName[31] = '\0'; 
            int filenamelength = strlen(filename); 
            foundname = memcmp(Dir[j].FileName, filename, filenamelength);
            fseek(fp, 6, SEEK_CUR); 
            if(foundname == 0){
                break; 
            }
            j++; 

        }
    }
    if(foundname != 0)
    {
        printf("File not found\n"); 
    }
    else{
        FILE *DirectoryBlocks = fopen(argv[1], "rb");
        FILE *FatBlocks = fopen(argv[1], "rb"); 
        fseek(DirectoryBlocks, Info.BlockSize * Dir[j].StartingBlock, SEEK_SET);
        fseek(FatBlocks,Info.FatStart * Info.BlockSize, SEEK_SET); 
        uint fatfilestart = 4 * Dir[j].StartingBlock; 
        fseek(FatBlocks, fatfilestart, SEEK_CUR); 
        unsigned char FatIndex[4]; 
        unsigned char FileBlockToRead[Info.BlockSize]; 
        __uint32_t blockoffset = 0; 
        int filebytes = Dir[j].FileSize; 
        int filebyteswritten = 0; 
        fread(FatIndex, 1, 4, FatBlocks);
        fread(FileBlockToRead, 1, Info.BlockSize, DirectoryBlocks); 
        FILE *NewFile = fopen(filename, "wb"); 
        while(memcmp(FatIndex, "\xFF\xFF\xFF\xFF", 4) != 0){
            blockoffset = FatIndex[0] << 24| FatIndex[1] << 16| FatIndex[2] << 8| FatIndex[3];
            fwrite(FileBlockToRead,1, Info.BlockSize, NewFile);
            fread(FatIndex,1,4,FatBlocks);  
            fseek(DirectoryBlocks, Info.BlockSize * blockoffset, SEEK_SET);
            fread(FileBlockToRead, 1, Info.BlockSize, DirectoryBlocks); 
            filebyteswritten += 512; 
        }
        filebytes = filebytes - filebyteswritten; 
        fwrite(FileBlockToRead,1, filebytes, NewFile);
        fclose(DirectoryBlocks);
        fclose(FatBlocks);

    }
    free(buffer); 
    fclose(fp); 
}