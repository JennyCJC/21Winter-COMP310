#include <stdio.h>
#include <stdlib.h>
/*
PCB has 3 fields
PC : Points to the the current line reached for that program
pageTable: Array which contains max 10 pages with default settings to -1
*/
typedef struct PCB
{
    int PC;
    int pageTable[10];
    int PC_page, PC_offset, pages_max;
    char* filepath;
}PCB;


/*
Passes 1 parameters pageNumber
This method creates a PCB with fields set as this:

*/
PCB* makePCB(int pageNumber, char* filepath){
    PCB* pcb = (PCB*)malloc(sizeof(PCB));
    pcb->pages_max = pageNumber;
    pcb->PC_offset = 0;
    pcb->PC_page = 0;
    pcb->PC = pcb->pageTable[pcb->PC_page];
    pcb->filepath = filepath;
    int counter = 0;
    while(counter <10){
        pcb->pageTable[counter] = -1;
        counter++;
    }
    return pcb;
}




