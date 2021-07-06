// Programmer: Jenny Cao, 260917694
// Date: 2021/03/10
// Purpose: launching the programs that execute via the exec command , managing frames, and paging.

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<time.h>
#include<unistd.h>
#include"pcb.h"
#include"kernel.h"
#include"ram.h"

//counts total pages of a file
//return -1 if file isn't valid
//return page number when normal
int countTotalPages(FILE *f){
    int linesCount=1;
    int pageNumber=0;
    char ch = '0';

    if(f == NULL){
        return -1;
    }
    
    //read character by character and check for new line
    while((ch=fgetc(f))!=EOF) {
        if(ch=='\n')
        linesCount++;
    }
    
    //caculate page number based on line counts
    pageNumber = linesCount/4;
    if(linesCount%4 > 0){
        pageNumber += 1;
    }
    
    return pageNumber;
}

// load the corresponding page of a file into the corresponding frame in RAM
void loadPage(int pageNumber, FILE *f, int frameNumber){
    char line[1000];    // store current line of code
    int lineNumber = pageNumber*4;  // where page starts
    int lineNumberRam = frameNumber*4;  // where RAM starts
    int linecounter = 0;    // keeps track of where we are in the file
    int counter = 0;    //counts how many lines have been copied
    
    // move to desired line of code
    while (!feof(f) && linecounter <= lineNumber)
    {
        linecounter +=1;
        fgets(line,999,f);
    }

    // copy until page is finished or EOF
    ram[lineNumberRam] = strdup(line);;
    counter++;
    lineNumberRam++;
    
    while (!feof(f) && counter < 4)
    {
        fgets(line,999,f);
        if(*line != '\n'){
            ram[lineNumberRam] = strdup(line);
        }
        //memset(line, 0, 1000);
        counter++;
        lineNumberRam++;
    }
    
    //if there is room left in the frame, fill the holes using NULL
    while(counter < 4){
        ram[lineNumberRam] = NULL;
        counter++;
        lineNumberRam++;
    }
}

// find free frame if possible
// return -1 if all frames have been occupied
// return frame number if a frame is free
int findFrame(){
    int counter = 0;    // keeps track of which frame we are on
    
    // check if ram block is free from the beginning
    while(counter < 40){
        if(ram[counter] == NULL){
            break;
        }
        counter += 4;
    }
    
    if(counter == 40){
        return -1;
    }
    else{
        return counter/4;
    }
}

// find victim frame if no frame is free
// select a random victim and check if its pageTable uses this frame
// if it does, move on to the next frame; else we have found our victim
// return victim frame number
int findVictim(PCB *p){
    int randomNumber = rand() % 10; // random frameNumber
    int counter = 0;    // counts which page we are at when checking if frame is used in pageTable
    int indicator = 0;  // indicates whether we have found victim frame
    
    while(1){
        while(counter < p->pages_max){
            if(p->pageTable[counter] == randomNumber){ // if this frame will be used by the current pcb, reset and move to next frame
                randomNumber = (randomNumber+1) % 10;
                counter = 0;
                indicator = 1;
                break;
            }
            indicator = 0;  // this frame not used by this slot in pageTable
            counter++;
        }
        if(indicator == 0){ // victim found
            return randomNumber;
        }
    }
}

//update PageTable
// if victimFrame == -1, then no frame is replaced
// if not, then we have to find the victim pcb and update that as well
int updatePageTable(PCB *p, int pageNumber, int frameNumber, int victimFrame){
    if(victimFrame == -1){
        p->pageTable[pageNumber] = frameNumber;
    }
    else{
        p->pageTable[pageNumber] = frameNumber;

        int counter = 0;    // keep track of the sequence of queue
        int pageCounter = 0;    // keep track of what page we are on
        int indicator = 0;  // indicate whether we have found victim pcb, 1 = found, 0 = otherwise

        while(size() > 0){
            PCB* pcb = pop();

            while(pageCounter <= pcb->pages_max){
                if(pcb->pageTable[pageCounter] == victimFrame){ // found the victim pcb and frame
                    pcb->pageTable[pageCounter] = -1;
                    indicator = 1;
                    break;
                }
                pageCounter++;
            }

            addToReady(pcb);
            counter++;

            if(indicator == 1){
                break;
            }

        }

        while(size()!= 0 && counter % size() != 0){   // the sequence of queue has changed, we need to fix as before
            PCB* pcb = pop();
            addToReady(pcb);
            counter++;
        }
    }
    return 0;
}

// launch a program and create copy in BackingStore
// return 0 if error
// return 1 if success
int launcher(FILE *p, char *originalFileName){
    // check if file pointer has been initialized
    if(p == NULL){
        return 0;
    }

    // put together file path in BackingStore with original name
    char dir[16] = "./BackingStore/";
    char *path = malloc(strlen(originalFileName) + strlen(dir) + 100);
    strcpy(path, dir);
    strcat(path, originalFileName);
    
    // check if we can name this copy as the original filename; if not, add suffix and edit path
    FILE *tempCheck;
    int counter = 48;

    while((tempCheck = fopen(path, "r"))){
        char suffix = counter;
        strcat(path, &suffix);
        counter++;
    }
    
    // copy original file into BackingStore with the path we have determined
    FILE *copy = fopen(path, "w"); // create copy
    if(copy == NULL){
        printf("error");
        return 0;
    }
    
    // copy content character by character
    char c;
    
    c = fgetc(p);
    
    while (c != EOF)
    {
        fputc(c, copy);
        c = fgetc(p);
    }
    
    fclose(p);
    fclose(copy);

    // load at most two pages of the program into RAM
    int pageNumber = 0;
    int frameNumber = 0;
    
    FILE *f = fopen(path, "r");
    pageNumber = countTotalPages(f);
    fclose(f);

    if(pageNumber == -1){
        return 0;
    }
    
    PCB* pcb = makePCB(pageNumber, path);

    int pageCounter = 0;
    int victimFrame = -1;
    
    do{
        frameNumber = findFrame();

        if (frameNumber == -1){
            victimFrame = findVictim(pcb);
            frameNumber = victimFrame;
        }

        FILE *f = fopen(path, "r");
        loadPage(pageCounter, f, frameNumber);
        fclose(f);
        
        updatePageTable(pcb, pageCounter, frameNumber, victimFrame);

        pageCounter++;
    }while(pageCounter < 2 && pageNumber > 1);

    //add pcb to ready queue
    addToReady(pcb);
    
    return 1;
}

/*
Return an errorCode:
ERRORCODE 0 : NO ERROR
ERRORCODE 1 : PROGRAM TERMINATES
*/
int pageFault(struct PCB* pcb){
    int frameNumber = -1;
    int victimFrame = -1;
    
    //update pc page
    pcb->PC_page++;
    
    if(pcb->PC_page >= pcb->pages_max){
        return 1;
    }

    if(pcb->pageTable[pcb->PC_page] == -1){  // page has not been loaded into RAM thus invalid, we need to find frame and update page
        frameNumber = findFrame();
        if(frameNumber == -1){
            victimFrame = findVictim(pcb);
            frameNumber = victimFrame;
        }
        FILE *f = fopen(pcb->filepath, "rw");
        loadPage(pcb->PC_page, f, frameNumber);
        fclose(f);
        updatePageTable(pcb, pcb->PC_page, frameNumber, victimFrame);
    }
    else{
        frameNumber = pcb->pageTable[pcb->PC_page];
    }
    
    pcb->PC = frameNumber;
    pcb->PC_offset = 0;

    return 0;

}
