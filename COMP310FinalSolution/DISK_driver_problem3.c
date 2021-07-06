//    Final Project COMP 310
//    Your Name:
//    Your McGill ID: 
//
//    You need to fill in this file for the third problem
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "DISK_driver.h"
#include "DISK_driver_problem1.h"
#include "DISK_driver_problem2.h"
#include "DISK_driver_problem3.h"


int seekInDisk(int aftID, int offset){
    if(aftID >= MAX_OPENFILES){
        printf("Input index out of bound");
        return 1;
    }
    
    if(active_file_table[aftID] == NULL){
        printf("ERROR: Open the file first");
        return 1;
    }
    
    FILE* file = active_file_table[aftID];
    
    int availableBytes = -1;
    if(offset > 0){
        availableBytes = (currentPartition.total_blocks-fat[active_file_table_owners[aftID]].current_location-1)*currentPartition.block_size - fat[active_file_table_owners[aftID]].offset;
    }else{
        availableBytes = fat[active_file_table_owners[aftID]].current_location*currentPartition.block_size + fat[active_file_table_owners[aftID]].offset;
    }
    
    if(abs(offset) > availableBytes){
        if(offset > 0){
            fseekToBlock(file, currentPartition.total_blocks);
            printf("ERROR: Out of bounds. Stopped at end of file");
            return 1;
        }else{
            fseekToBlock(file, 0);
            printf("ERROR: Out of bounds. Stopped at start of file");
            return 1;
        }
    }
    
    int blocks = abs(offset)/currentPartition.block_size;
    int offset_counter = abs(offset) - blocks*currentPartition.block_size;
    fat[active_file_table_owners[aftID]].offset += offset_counter;
    if(fat[active_file_table_owners[aftID]].offset > currentPartition.block_size){
        blocks++;
        fat[active_file_table_owners[aftID]].offset -= currentPartition.block_size;
    }
    fat[active_file_table_owners[aftID]].current_location += blocks;
    
    //fseekToOffset(file, offset);
    /*int counter = 0;
    while(counter < offset){
        //for (int i = currentPartition.block_size; i > 0; i--)
        fgetc(file);
        
        //mount partition3 10 5fseek(file, 0, SEEK_CUR );
        counter++;
        fseek(file, 0, SEEK_CUR );
    }*/
    //fseek(file, 0, SEEK_CUR );
    
    
    fflush(file);
    
    /*
    if(offset > 0){
        if(currentPartition.total_blocks - fat[active_file_table_owners[aftID]].current_location < offset){
            fseekToBlock(file, currentPartition.total_blocks);
            printf("ERROR: Out of bounds. Stopped at end of file");
            return 1;
        }
    }
    else{
        if(fat[active_file_table_owners[aftID]].current_location + offset < 0){
            fseekToBlock(file, 0);
            printf("ERROR: Out of bounds. Stopped at start of file");
            return 1;
        }
    }
    
    fseekToBlock(file, fat[active_file_table_owners[aftID]].current_location+offset);
    */
    return 0;
}
