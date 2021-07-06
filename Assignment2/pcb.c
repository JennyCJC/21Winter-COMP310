#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "pcb.h"

struct PCB* makePCB(int start, int end){
    struct PCB * new = malloc(sizeof(struct PCB));
    new->PC = start;
    new->start  = start;
    new->end = end;
    return new;
}