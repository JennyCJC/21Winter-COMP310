#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "pcb.h"
#include "kernel.h"
#include "cpu.h"
#include "ram.h"
#include "shell.h"

struct node {
	struct PCB *pcb;
	struct node *next;
	};

struct node tail = {NULL, NULL};
struct node head = { NULL, &tail };

void addToReady(struct PCB * p){
    struct node* add = (struct node*)malloc(sizeof(struct node));
    add->pcb = p;
    add->next = &tail;
    struct node* current = &head;
    while(current->next != &tail){
        current = current->next;
    }
    current->next = add;
    //add.next = &tail;
}

void rotate(){
    struct node *last = head.next;
    struct node *fin = head.next;
    while(last->next != &tail){
        last = last->next;
    }
    last->next = fin;
    head.next = head.next->next;
    fin->next = &tail;
   
    
}

void remove_node(){
    struct node* hold = head.next;
    head.next = head.next->next;
    free(hold->pcb);
    //free(&hold);
    

}

int myinit(char *filename){
    int error = 0;
    FILE* f;
    int* start = (int*)malloc(sizeof(int));
    int* end = (int*)malloc(sizeof(int));
    int i = 0;
    

    f = fopen(filename, "rt");
    if(f == NULL){
        printf("File can't be opened");
        error = 1;
        return error;
    }

    error += addToRam(f, &start, &end);
    
    if(error > 0){
        unload(0);  //RAM full
        return error;
    }
    
    struct PCB* new = makePCB(*start,*end);
    
    addToReady(new);

    return error;
}

int scheduler(){
    int error = 0;

    while(head.next != &tail){
        //printf("hi");
        if(check_cpu() == 0){
            load_pcb(head.next->pcb->PC);
            if(run_cpu(2, head.next->pcb->end) == 1){
                unload(head.next->pcb->end);
                remove_node();
            }
            else{
                head.next->pcb->PC = get_cpu_ip();
                rotate();
            }
        }
        else{
            printf("CPU unavailable.");
            return 1;
        }
    }
    return error;
}

int try(int agrc, char* argv[]){
    printf("Kernel 1.0 loaded!");
    shellUI(agrc, argv);
    return 0;
}

