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
    int RootFiles = 0; 
    RootDirectory *Dir =  (RootDirectory*)malloc(sizeof(RootDirectory)* RootNum); 
    /*if(RootFiles == RootNum){
        Dir = (RootDirectory*)realloc(Dir, sizeof(RootDirectory) * RootNum);
        Rootnum *= 2; 
    }*/ 
    fseek(fp, (Info.RootDirectoryStart * Info.BlockSize), SEEK_SET); 
    int RootBytes = Info.RootDirectoryBlocks * Info.BlockSize; 
    int DirectoryEntries = RootBytes/64; 
    int j = 0;
    int looptest = 0; 
    unsigned char buffer1[50]; 
    __uint32_t buffer2[8]; 
    int combine = 0; 
    int chartest; 
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
            fread(buffer1, 1, 4, fp);
            Dir[j].StartingBlock = buffer1[0] << 24| buffer1[1] << 16| buffer1[2] << 8| buffer1[3];
            fread(buffer1, 1, 4, fp); 
            Dir[j].NumberofBlocks = buffer1[0] << 24| buffer1[1] << 16| buffer1[2] << 8| buffer1[3];
            fread(buffer1, 1, 4, fp); 
            Dir[j].FileSize = (__uint32_t ) buffer1[0] << 24| buffer1[1] << 16| buffer1[2] << 8| (buffer1[3] & 0b11111111);
            fread(buffer1, 1, 7, fp); 
            combine = buffer1[0] << 8 | buffer1[1]; 
            buffer2[0] = combine; 
            buffer2[1] = buffer1[2];
            buffer2[2] = buffer1[3];
            buffer2[3] = buffer1[4];
            buffer2[4] = buffer1[5];
            buffer2[5] = buffer1[6];
            memcpy(Dir[j].CreateTime, buffer2, 6); 
            //Dir[j].CreateTime[7] = '\0'; 
            fread(buffer1, 1, 7, fp); 
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
            
             
            fread(buffer1, 1, 31, fp);  
            buffer1[31] = '\0'; 
            memcpy(Dir[j].FileName, buffer1, 32); 
            Dir[j].FileName[31] = '\0'; 
            fseek(fp, 6, SEEK_CUR); 
            j++; 
        }
    }
    for(int i = 0; i < j; i++){
        printf("%c", Dir[i].Status); 
        printf(" %10u %30s %d/%02d/%02d %02d:%02d:%02d \n", Dir[i].FileSize, Dir[i].FileName, Dir[i].ModifyTime[0], Dir[i].ModifyTime[1], Dir[i].ModifyTime[2], Dir[i].ModifyTime[3], Dir[i].ModifyTime[4], Dir[i].ModifyTime[5]);
        //Dir[i].CreateTime, Dir[i].ModifyTime, Dir[j].FileName
    }
 


    free(buffer); 
}