//    Final Project COMP 310
//    Your Name:
//    Your McGill ID: 
//
//    You need to fill in this file for the second problem
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "DISK_driver.h"
#include "DISK_driver_problem1.h"
#include "DISK_driver_problem2.h"

//return 1 if error
//return 0 if success
int closeInDisk(int aftIndex){
    // Checking if the file is not already open
    if(active_file_table_owners[aftIndex] == -1 ){
        printf("ERROR: Index number not in use.");
        return 1;
    }
    
    fclose(active_file_table[aftIndex]);
    active_file_table_owners[aftIndex] = -1;
    
    free(blockBuffer);

    return 0;
}
