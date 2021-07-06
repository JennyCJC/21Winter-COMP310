#ifndef PCB_H
#define PCB_H

#include <stdio.h>
#include <stdlib.h>
/*
PCB has 3 fields
PC : Points to the the current line reached for that program
start: Points to first line of that program
end: Points to last line of that program
*/
typedef struct PCB
{
    int PC;
    int pageTable[10];
    int PC_page, PC_offset, pages_max;
    char* filepath;
}PCB;

/*
Passes 2 parameters (start , end)
This method creates a PCB with fields set as this:
PC = start
start = start
end = end
*/
PCB* makePCB(int pageNumber, char* filepath);

#endif