//    Final Project COMP 310
//    Your Name: Jenny Cao
//    Your McGill ID: 260917694
//
//    You need to fill in this file for the fourth random problem



//	  Fill in the name of the command that you are implementing here 
//    ------------------>Chmod<-----------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "DISK_driver.h"
#include "DISK_driver_problem1.h"
#include "DISK_driver_problem2.h"
#include "DISK_driver_problem3.h"
#include "DISK_driver_problem4.h"

int chmodInDisk(int aftID, char* readPermission, char* writePermission){
    if(aftID >= MAX_OPENFILES){
        printf("Input index out of bound");
        return 1;
    }
    
    if(active_file_table[aftID] == NULL){
        printf("ERROR: Open the file first");
        return 1;
    }
    
    
    if(readPermission != NULL && strcmp(readPermission, "r+") == 0){
        fat[active_file_table_owners[aftID]].readPermission = 1;
    }
    else if (readPermission != NULL && strcmp(readPermission, "r-") == 0){
        fat[active_file_table_owners[aftID]].readPermission = 0;
    }
    else{
        printf("ERROR: Bad syntax.");
        return 1;
    }
    
    if(writePermission != NULL && strcmp(writePermission, "w+") == 0){
        fat[active_file_table_owners[aftID]].writePermission = 1;
    }
    else if (writePermission != NULL && strcmp(writePermission, "w-") == 0){
        fat[active_file_table_owners[aftID]].writePermission = 0;
    }
    else{
        printf("ERROR: Bad syntax.");
        return 1;
    }
    
    updateFATOnDisk();
    
    return 0;
}
