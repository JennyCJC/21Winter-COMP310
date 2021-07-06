//    Final Project COMP 310
//    Your Name:Jenny Cao
//    Your McGill ID: 260917694
//
//    You need to fill in this file for the first problem
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "DISK_driver.h"
#include "DISK_driver_problem1.h"
#include "shellmemory.h"
char writebuffer[50000];
char writebuffer2[50000];

void fseekToDataSection1(FILE *file){
    
    fseek(file, 0, SEEK_SET);
    
    for (int i = MAX_FILES*2 +1; i > 0; i--)
        fgets(writebuffer, 50000, file);
    return;
}

void fseekToBlock1(FILE *file, int blockID){
    
    fseekToDataSection1(file);
    
    for (int i = blockID * currentPartition.block_size; i > 0; i--)
        fgetc(file);
    
    fseek(file, 0, SEEK_CUR );
    return;
}

//return 1 if error
//return 0 if success
int openInDisk(int aftID, char* name){
    if(aftID >= MAX_OPENFILES){
        printf("Input index out of bound");
        return 1;
    }
    
    if(active_file_table_owners[aftID] != -1){
        printf("ERROR: Index number in use");
        return 1;
    }
    
    int FATindex = 0;
    int empty_FAT = -1;
    for (; FATindex < MAX_FILES; FATindex++){
        if (fat[FATindex].filename != NULL && strcmp(fat[FATindex].filename, name) == 0){
            break;
        }
        else if(fat[FATindex].filename == NULL && empty_FAT == -1){
            empty_FAT = FATindex;
        }
    }
    
    //char path[200];
    //snprintf(path, sizeof(path), "PARTITION/%s.txt", filename);
    
    FILE* f = fopen(path, "r+");
    int file_index;
    if(FATindex < MAX_FILES){
        file_index = FATindex;
    }
    else if(empty_FAT != -1){
        file_index = empty_FAT;
        fat[empty_FAT].filename = malloc(sizeof(500));
        strcpy(fat[empty_FAT].filename, name);
        fat[empty_FAT].current_location = 0;
        fat[empty_FAT].file_length = 0;
    }
    else{
        printf("ERROR: Partition Full");
        return 1;
    }
    
    partitionFile = fopen(path, "r+");
    //mount partition2 10 5partitionFile = fopen(path, "r+");
    
    if(partitionFile == NULL){
        printf("Partition Doesn't exist");
        return 1;
    }
    updateFATOnDisk();
    fclose(partitionFile);
    
    fseekToBlock1(f, file_index);
    active_file_table[aftID] = f;
    active_file_table_owners[aftID] = file_index;
    
    return 0;
}


int writeInDisk(int aftID, char* strings){
    if(aftID >= MAX_OPENFILES){
        printf("Input index out of bound");
        return 1;
    }
    
    if(active_file_table[aftID] == NULL){
        printf("ERROR: Open the file first");
        return 1;
    }
    
    //permission check
    if(fat[active_file_table_owners[aftID]].writePermission != 1){
        printf("ERROR: Write permission denied.");
        return 1;
    }
    
    
    float strLenght = strlen(strings);
    float blockSize = getBlockSize();
    int blocksToWrite = (strLenght / blockSize) + 1;
    
    
    for (int i = 0; i < blocksToWrite; i++) {
        int ptrMovement = i * getBlockSize();
        char *ptr = (strings + ptrMovement);
        writeBlock(active_file_table_owners[aftID], ptr);
        
    }
    
    fat[active_file_table_owners[aftID]].current_location = 0;
    
    return 0;
}


int readInDisk(int aftID, char* var){
        if(aftID >= MAX_OPENFILES){
            printf("Input index out of bound");
            return 1;
        }
        
        if(active_file_table[aftID] == NULL){
            printf("ERROR: Open the file first");
            return 1;
        }
    
        //permission check
        if(fat[active_file_table_owners[aftID]].readPermission != 1){
            printf("ERROR: Read permission denied.");
            return 1;
        }
        
        //fseek(active_file_table[aftID], 0, SEEK_SET);
        
        
        // Read the string from the file
        char *ptr = readBlock(active_file_table_owners[aftID]);
        if(ptr ==  NULL){
            printf("Read command failed.\n");
            return 2; // Read block failed
        }
        
        char *finalStr = malloc(20000);
        strcpy(finalStr, ptr);
        
        while(ptr != NULL){
            ptr = readBlock(aftID);
            if (ptr == NULL)
                break;
            strcat(finalStr, ptr);
        }
        
        
        // Write the string to the shell variable
        setVariable(var, finalStr);
        //free(finalStr);
        
        fat[active_file_table_owners[aftID]].current_location = 0;
        fflush(active_file_table[aftID]);
        
        return 0;
    
    /*if(aftID >= MAX_OPENFILES){
        printf("Input index out of bound");
        return 1;
    }
    
    if(active_file_table[aftID] == NULL){
        printf("ERROR: Open the file first");
        return 1;
    }
    
    //permission check
    if(fat[active_file_table_owners[aftID]].readPermission != 1){
        printf("ERROR: Read permission denied.");
        return 1;
    }
    
    //fseek(active_file_table[aftID], 0, SEEK_SET);
    
    int MAX_BUFFER = 20000;
    
    // Read the string from the file
    char *ptr = readBlock(active_file_table_owners[aftID]);
    if(ptr ==  NULL){
        printf("Read command failed.\n");
        return 2; // Read block failed
    }
    
    char *finalStr = malloc(MAX_BUFFER);
    strcpy(finalStr, ptr);
    
    while(ptr != NULL){
        ptr = readBlock(aftID);
        if (ptr == NULL)
            break;
        strcat(finalStr, ptr);
    }
    
    
    // Write the string to the shell variable
    setVariable(var, finalStr);
    free(finalStr);
    
    fat[active_file_table_owners[aftID]].current_location = 0;
    
    return 0;*/
}
