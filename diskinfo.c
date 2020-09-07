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



int main(int argc, char** argv)
{
    FILE *fp; 
    fp = fopen(argv[1], "rb"); 
    if(!fp)
    {
        printf("yikes"); 
    }

    SuperBlock Info; 
     
    char *buffer;
    buffer = (char*)malloc(9); 
 

    int readvalue = fread(buffer, 1,8,fp);
    memcpy(Info.FileSystemName, buffer, 9);  
    
    printf("Super block information:\n"); 
    readvalue = fread(buffer, 1, 2, fp);
    Info.BlockSize = buffer[0] << 8| buffer[1]; 
    printf("Block Size: %d\n",Info.BlockSize);
    readvalue = fread(buffer, 1, 4, fp);
    Info.BlockCount = buffer[0] << 24| buffer[1] << 16| buffer[2] << 8| buffer[3]; 
    printf("BlockCount: %d\n",Info.BlockCount);
    readvalue = fread(buffer, 1, 4, fp);
    Info.FatStart = buffer[0] << 24| buffer[1] << 16| buffer[2] << 8| buffer[3];
    printf("FAT starts: %d\n",Info.FatStart);        
    readvalue = fread(buffer, 1, 4, fp);
    Info.FatBlocks = buffer[0] << 24| buffer[1] << 16| buffer[2] << 8| buffer[3];
    printf("FAT blocks: %d\n",Info.FatBlocks);
    readvalue = fread(buffer, 1, 4, fp);
    Info.RootDirectoryStart = buffer[0] << 24| buffer[1] << 16| buffer[2] << 8| buffer[3];
    printf("Root Directory start: %d\n",Info.RootDirectoryStart);
    readvalue = fread(buffer, 1, 4, fp);
    Info.RootDirectoryBlocks = buffer[0] << 24| buffer[1] << 16| buffer[2] << 8| buffer[3];
    printf("Root directory blocks: %d\n",Info.RootDirectoryBlocks);
    if(readvalue == 0){
        printf("something went wrong"); 
    }
    printf("\n"); 

    //fat entries are 4 bytes long 
    int fatend = (Info.BlockSize * Info.FatBlocks)/4; 
    //printf("%d\n", fatend); 
    FILE *fp1; 
    fp1 = fopen(argv[1], "rb");
    //printf("%d\n", Info.FatStart * Info.BlockSize);
    fseek(fp1,  (Info.FatStart * Info.BlockSize) , SEEK_CUR); 
    int available = 0; 
    int reserved = 0; 
    int allocated = 0; 
    char fatblock[4]; 
    for(int i = 0; i < fatend; i++)
    {
        int check = fread(fatblock, 1, 4, fp1); 
        if(check != 4){
            printf("yikes"); 
        }
        if(memcmp(fatblock, "\x00\x00\x00\x01", 4) == 0){
            reserved++; 
        }
        else if(memcmp(fatblock, "\x00\x00\x00\x00", 4) == 0){
            available++; 
        }
        else{
            allocated++; 
        }
    }
    printf("FAT Information:\n");
    printf("Free Blocks: %d\n", available);
    printf("Reserved Blocks: %d\n", reserved);
    printf("Allocated Blocks: %d\n", allocated);
    
    free(buffer); 

}