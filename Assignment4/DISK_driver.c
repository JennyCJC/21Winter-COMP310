#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
#include <stdbool.h>
#include "DISK_driver.h"

struct PARTITION {
    int total_blocks;
    int block_size;
    char* partitionName;
} PARTITION;


struct FAT {
       char *filename;
       int file_length;
       int blockPtrs[10];
       int current_location;
} fat[20];

FILE *active_file_table[5];
char *filename_of_aft[5];

char * block_buffer;
char writebuffer[100000];
char writebuffer2[100000];


int getBlockSize(){
    return PARTITION.block_size;
}

void initIO(){
    
    PARTITION.total_blocks = 0;
    PARTITION.block_size = 0;

    int counter = 0;
    while(counter<20){
        fat[counter].filename = NULL;
        fat[counter].file_length = 0;
        for(int i = 0; i < 10; i+=1){
            fat[counter].blockPtrs[i] = -1;
        }
        fat[counter].current_location = -1;
        counter+=1;
    }
    
    for(int i = 0; i<5; i+=1){
        filename_of_aft[i] = "NULL";
    }
}

char* getPath(char *name){
    char dir[13] = ("./PARTITION/");
    char extenstion[5] = (".txt");
    char *path = malloc(strlen(name) + strlen(dir) + 100);
    strcpy(path, dir);
    strcat(path, name);
    strcat(path, extenstion);
    
    return path;
}

void updateFATOnDisk(FILE* partitionFile){
    fseek(partitionFile, 0, SEEK_SET);
    FILE* tempFile = fopen("./PARTITION/temp.txt","w");
    
    sprintf(writebuffer, "%i, %i\n", PARTITION.block_size, PARTITION.total_blocks);
    fputs(writebuffer, tempFile);
    
    // Writting the contents of fat[MAX_FILES] to the file
    for (int i = 0; i < 20; i+=1) {
        struct FAT currentFile = fat[i];
        sprintf(writebuffer, "%i", currentFile.blockPtrs[0]);
        for (int j = 1; j < 10; j+=1)
            sprintf(writebuffer, "%s; %i", writebuffer, currentFile.blockPtrs[j]);
        sprintf(writebuffer2, "%s, %i\n %s\n", currentFile.filename, currentFile.file_length, writebuffer);
        fputs(writebuffer2, tempFile);
    }
    fseek(partitionFile, -1*PARTITION.block_size*PARTITION.total_blocks, SEEK_END);
    char *buffer;
    size_t bufsize = 50000;
    buffer = (char *)malloc(bufsize * sizeof(char));
    getline(&buffer,&bufsize,partitionFile);
    fputs(buffer,tempFile);
    fseek(tempFile, 0, SEEK_SET);
    fclose(tempFile);
    
    //delete old partition file
    char* command2 = malloc(200);
    strcpy(command2, "rm ");
    strcat(command2, getPath(PARTITION.partitionName));
    system(command2);
    
    //rename the temp file to the old partition file name
    char* command = malloc(200);
    strcpy(command, "mv ");
    strcat(command, getPath("temp"));
    strcat(command, " ");
    strcat(command, getPath(PARTITION.partitionName));
    system(command);
}


int partition(const char *name,int totalblocks, int blocksize){
    int errorCode = 1;

    struct stat st = {0};
    //if directory does not exist, create it!
    if (stat("PARTITION", &st) != 0) {
        system("mkdir PARTITION");
    }
   
    char *path = getPath(name);
    
    FILE* file;
    if (!(file = fopen(path, "r"))){
        file = fopen(path, "w");
    }
    fclose(file);
    file = fopen(path, "w");
    
    PARTITION.block_size = blocksize;
    PARTITION.total_blocks = totalblocks;
    PARTITION.partitionName = name;
    
    
    //initialize FAT on disk
    fseek(file, 0, SEEK_SET);
    
    // Writting the contents of currentPartition onto the file
    sprintf(writebuffer, "%i, %i\n", PARTITION.block_size, PARTITION.total_blocks);
    fputs(writebuffer, file);
    
    // Writting the contents of fat[20] to the file
    for (int i = 0; i < 20; i+=1) {
        sprintf(writebuffer, "%i", -1);
        for (int j = 1; j < 10; j+=1)
            sprintf(writebuffer, "%s; %i", writebuffer, -1);
        sprintf(writebuffer2, "%s, %i\n %s\n", NULL, 0, writebuffer);
        fputs(writebuffer2, file);
    }
    
    // put empty data in, format it as blocksize * totalblocks number of '0's
    for (int zeroes = totalblocks*blocksize; zeroes > 0; zeroes--)
        fputc('0', file);
    
    fclose(file);

    block_buffer = (char*)malloc((blocksize) * sizeof(char));
    
    return errorCode;
}



// returns 1 if successful
// returns 0 if not
int mountFS(char *name){
    PARTITION.partitionName = name;
    int errorCode = 1;

    char *path = getPath(name);

    FILE* file;
    if (!(file = fopen(path, "r+"))){
        errorCode = 0;
        return errorCode;
    }
    
    /*
    fscanf(file, "%d", &PARTITION.total_blocks);
    fscanf(file, "%d", &PARTITION.block_size);
    printf("%d\n", PARTITION.total_blocks);
    printf("%d\n", PARTITION.block_size);
    
    int fat_counter = 0;
    char buff[100] = "";
    while(fat_counter < 20){
        fscanf(file, "%s", buff);
        memcpy(fat[fat_counter].filename, buff, 101);

        fscanf(file,"%d", &fat[fat_counter].file_length);
        
        int array_counter = 0;
        while(array_counter < 10){
            fscanf(file,"%d", &fat[fat_counter].blockPtrs[array_counter]);
            array_counter++;
        }
        
        fscanf(file, "%d", &fat[fat_counter].current_location);
        fat_counter++;
    }
    */
    // Reading the contents of currentPartition onto the file by getting the first line into write buffer
    fgets(writebuffer, 50000, file);
    sscanf(writebuffer, "%i, %i\n", &PARTITION.block_size, &PARTITION.total_blocks);
    
    // Writting the contents of fat[MAX_FILES] to the file
    for (int i = 0; i < 20; i+=1) {
        struct FAT currentFile;

        fgets(writebuffer, 50000, file);
        char *token;
        char const *nullFilename = "(null)";
        char const comma = ',';
        char const semicolon = ';';
        
        // Getting the filenames
        token = strtok(writebuffer, &comma);
        if(token == NULL)
            return 0;
        currentFile.filename = malloc(sizeof(&token));
        strcpy(currentFile.filename, token);
        if(!strcmp(currentFile.filename, nullFilename))
            currentFile.filename = NULL;
        
        // Getting the file sizes
        token = strtok(NULL, &comma);
        currentFile.file_length = atoi(token);
        
        // Getting the file pointers
        fgets(writebuffer, 50000, file);
        token = strtok(writebuffer, &semicolon);
        currentFile.blockPtrs[0] = atoi(token);
       
        for (int j = 1; j < 10; j+=1){
            token = strtok(NULL, &semicolon);
            currentFile.blockPtrs[j] = atoi(token);
        }

        // Finishing off
        currentFile.current_location = 0;
        fat[i] = currentFile;
    }
    
    
    fclose(file);

    return errorCode;
}

// searches for the string name argument in the filename fields
// return -1 if error
// return fat index if success
int openfile(char *filename){
    int fat_counter = 0;
    int aft_counter = 0;
    int empty_fat = -1;
    int empty_aft = -1;
    int already_opened = 0;
    
    while(aft_counter < 5){
        if(strcmp(filename_of_aft[aft_counter], filename) == 0){
            already_opened = 1;
            break;
            
        }
        aft_counter+=1;
    }
    
    if(already_opened == 0){
        aft_counter = 0;
        while(aft_counter < 5){
            if(active_file_table[aft_counter] == NULL){
                empty_aft = aft_counter;
                break;
            }
            aft_counter+=1;
        }
    }
    
    //check if file exists and records empty block index
    while(fat_counter < 20){
        if(fat[fat_counter].filename != NULL && strcmp(fat[fat_counter].filename, filename) == 0){   //file found in FAT
            if(already_opened == 1){
                return fat_counter; // file already opened
            }
            else if(empty_aft == -1){
                return empty_aft;   // active file table full
            }
            else{
                FILE* f = fopen(getPath(PARTITION.partitionName), "r+");
                
                
                if(fat[fat_counter].file_length > 0){
                    fseek(f, fat[fat_counter].blockPtrs[0]*PARTITION.block_size, SEEK_CUR);
                }
                
                fat[fat_counter].current_location = 0;
                active_file_table[empty_aft] = f;
                filename_of_aft[empty_aft] = filename;
               
            }
            return fat_counter; // return found-file FAT
        }
        else if (empty_fat == -1 && fat[fat_counter].filename == NULL){   //empty FAT cell found
            empty_fat = fat_counter;
        }
        
        fat_counter+=1;
    }

    //if the file we try to find doesn't exist, create it!
    if(empty_fat != -1){//fat not full
        fat[empty_fat].filename = malloc(sizeof(500));
        strcpy(fat[empty_fat].filename, filename);
        //strcpy(fat[empty_fat].filename, filename);
        fat[empty_fat].file_length = 0;
        
        FILE* f = fopen(getPath(PARTITION.partitionName), "r+");
        
        fat[empty_fat].filename = malloc(sizeof(500));
        strcpy(fat[empty_fat].filename, filename);
        fat[empty_fat].current_location = 0;
        fat[empty_fat].file_length = 0;
        
        fclose(f);
    }
    
    return empty_fat;
}

char* loadBlock(){
    return block_buffer;
}

void fseekToBlock(FILE *file, int blockID){
    
    fseek(file, 0, SEEK_SET);
    
    for (int i = 41; i > 0; i--){
        fgets(writebuffer, 50000, file);
    }
    
    for (int i = blockID * PARTITION.block_size; i > 0; i--){
        fgetc(file);
    }
    
    fseek(file, 0, SEEK_CUR );
}

//return 0 if success
//return -1 if file has no content
//return -2 if fat out of bound
//return -3 if not in aft
//return -4 if invalid input
//return -5 if the block to read is invalid
int readBlock(int file){
    if(file <0 || file >19){
        return -4;
    }
    if(fat[file].file_length <= 0 ){
        return -1;
    }
    
    if(fat[file].current_location > fat[file].file_length){
        return -2;
    }
    
    int aft_index = -1;
    int aft_counter = 0;
    while(aft_counter < 5){
        if(strcmp(filename_of_aft[aft_counter], fat[file].filename) == 0){
            aft_index = aft_counter;
            break;
        }
        aft_counter+=1;
    }
    
    if(aft_index == -1){
        return -3;
    }
    
    int blockToRead = fat[file].blockPtrs[fat[file].current_location];
    if(blockToRead == -1)
        return -5;
    
    FILE* f = active_file_table[aft_index];
    
    fseekToBlock(f, blockToRead);
    
    //read from the block
    int block_counter = 0;
    while(block_counter < PARTITION.block_size) {
        char c = fgetc(f);
        if(c == '0'){
            block_buffer[block_counter] = '\0';
            break;
        }
        block_buffer[block_counter] = c;
        block_counter+=1;
    }
    block_buffer[block_counter] = '\0';
    
    fat[file].current_location+=1;
    
    return 0;
}

int findFreeBlock(FILE* f){
    fseekToBlock(f, 0);
    int block_counter = 0;
    int block_index = -1;
   
    while(block_counter < PARTITION.block_size) {
        char c = fgetc(f);
        if(c == '0'){
            block_index = block_counter;
            break;
        }
        int counter = 0;
        while(counter < PARTITION.block_size-1){
            fgetc(f);
            counter+=1;
        }
        block_counter+=1;
    }
    
    return block_counter;
    
}

//return 0 if success
//return -1 if invalid input
//return -2 if out of bound/file full
//return -3 if not in aft
//return -4 if partition full
int writeBlock(int file, char* data){
    
    if(file <0 || file > 19){
        return -1;
    }
    
    if(fat[file].current_location >= 9 || fat[file].file_length >= 10){
        return -2;
    }
    
    int aft_index = -1;
    int aft_counter = 0;
    while(aft_counter < 5){
        if(strcmp(filename_of_aft[aft_counter], fat[file].filename) == 0){
            aft_index = aft_counter;
            break;
        }
        aft_counter+=1;
    }
    
    if(aft_index == -1){
        return -3;
    }
    
    FILE* f = active_file_table[aft_index];
    
    // find a free block
    
    int block_index = findFreeBlock(f);
    
    fseekToBlock(f, block_index);

    //partition full
    if(block_index == -1){
        return -4;
    }
    
    // store data in free block
    for (int i = 0; i < PARTITION.block_size; i+=1) {
        char c = *(data+i);
        if(c == 0){
            break;
        }
        fputc(c, f);
    }
    
    // update FAT
    fat[file].file_length += 1;
    for(int k =0; k<10; k++){
        if(fat[file].blockPtrs[k] == -1){
            fat[file].blockPtrs[k] = block_index;
            break;
        }
    }
    //fat[file].blockPtrs[fat[file].current_location] = block_index;
    //fat[file].current_location+=1;
    fflush(f);
    
    updateFATOnDisk(f);
    return 0;
}

//return 0 if success
//return -1 if invalid input
//return -2 if file empty
int resetLocation(int file){
    if(file < 0 || file > 19){
        return -1;
    }
    
    if(fat[file].file_length <= 0){
        return -2;
    }
    fat[file].current_location = 0;
    return 0;
}

//return 0 if success
//return -1 if invalid input
//return -2 if file not in aft
int closeFile(int file){
    if(file < 0 || file > 19){
        return -1;
    }
    
    // Checking if the file is not already open
    int aft_index = -1;
    int aft_counter = 0;
    while(aft_counter < 5){
        if(filename_of_aft[aft_counter] == fat[file].filename){
            aft_index = aft_counter;
            break;
        }
        aft_counter+=1;
    }
    
    
    if(aft_index == -1)
        return -2; // File not in aft
    
    fclose(active_file_table[aft_index]);
    active_file_table[aft_index] = NULL;
    filename_of_aft[aft_index] = "NULL";
  
    return 0;
}


/*
int main(){
    
    initIO();
    
    partition("PAR1", 10, 5);
    
//    char* path;
//    path = "./PARTITION/PAR1.txt";
//    FILE* f = fopen(path, "r");
//    //char buffer[100];
//    //char* c = NULL;
//    //fgets(c, 10, f);
//    //printf("%s", c);
//    //fseek(f, 0, SEEK_SET);
//    //fread(buffer, 10, 1, f);
//    int buffer = 0;
//
//    fscanf(f, "%d", &buffer);
//
//    //printf("%s\n", buffer);
//    printf("%d\n", buffer);
//    fclose(f);


    mountFS("PAR1");
    openfile("test", "PAR1");
    openfile("test", "PAR1");
    writeBlock(0, "hi");



    writeBlock(0, "bill");
    
    resetLocation(0);
    readBlock(0);
    readBlock(0);
    
    return 1;
}*/
