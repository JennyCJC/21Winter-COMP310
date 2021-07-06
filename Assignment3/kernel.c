#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
#include"shell.h"
#include"pcb.h"
#include"ram.h"
#include"cpu.h"
#include"interpreter.h"
#include"memorymanager.h"


/*
This is a node in the Ready Queue implemented as 
a linked list.
A node holds a PCB pointer and a pointer to the next node.
PCB: PCB
next: next node
*/
typedef struct ReadyQueueNode {
    PCB*  PCB;
    struct ReadyQueueNode* next;
} ReadyQueueNode;

ReadyQueueNode* head = NULL;
ReadyQueueNode* tail = NULL;
int sizeOfQueue = 0;

void boot(){
    struct stat st = {0};
    //check if BackingStore already exists; if it does, delete the existing directory
    if (stat("BackingStore", &st) == 0) {
        system("rm -rf BackingStore");
    }
    system("mkdir BackingStore"); // create a directory

}

int kernel(int argc, char const *argv[]){   
    int error = 0;
    error = shellUI();
    return error;
}

int main(int argc, char const *argv[]) {
    int error=0;
    boot(); // First : actions performed by boot 
    error = kernel(argc, argv); // Second: actions performed by kernel 
    return error;
}

/*
Adds a pcb to the tail of the linked list
*/
void addToReady(struct PCB* pcb) {
    ReadyQueueNode* newNode = (ReadyQueueNode *)malloc(sizeof(ReadyQueueNode));
    newNode->PCB = pcb;
    newNode->next = NULL;
    if (head == NULL){
        head = newNode;
        tail = newNode;
    } else {
        tail->next = newNode;
        tail = newNode;
    }
    sizeOfQueue++;
}

/*
Returns the size of the queue
*/
int size(){
    return sizeOfQueue;
}

/*
Pops the pcb at the head of the linked list.
pop will cause an error if linkedlist is empty.
Always check size of queue using size()
*/
struct PCB* pop(){
    PCB* topNode = head->PCB;
    ReadyQueueNode * temp = head;
    if (head == tail){
        head = NULL;
        tail = NULL;
    } else {
        head = head->next;
    }
    free(temp);
    sizeOfQueue--;
    return topNode;
}

int scheduler(){
    // set CPU quanta to default, IP to -1, IR = NULL
    CPU.quanta = DEFAULT_QUANTA;
    CPU.IP = -1;
    while (size() != 0){
        //pop head of queue
        PCB* pcb = pop();
        pcb->PC = pcb->pageTable[pcb->PC_page];
        
        //copy PC of PCB to IP of CPU
        CPU.IP = pcb->PC*4;
        CPU.offset = pcb->PC_offset;

        int quanta = DEFAULT_QUANTA;
    
        int errorCode = run(quanta);

        if (errorCode == 1){    // indicating page fault
            errorCode = pageFault(pcb);
            if(errorCode == 1){
                free(pcb);
            }else{
                addToReady(pcb);
            }
        }
        else if ( errorCode!=0 ){   // indicating unexpected behavior or end of program
            free(pcb);
        } else {
            pcb->PC_offset = CPU.offset;
            addToReady(pcb);
        }
    }
    // reset RAM
    resetRAM();
    return 0;
}

/*
Flushes every pcb off the ready queue in the case of a load error
*/
void emptyReadyQueue(){
    while (head!=NULL){
        ReadyQueueNode * temp = head;
        head = head->next;
        free(temp->PCB);
        free(temp);
    }
    sizeOfQueue =0;
}

