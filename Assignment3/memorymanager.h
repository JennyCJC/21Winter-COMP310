#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<time.h>
#include"kernel.h"

int pageFault(struct PCB* pcb);

int launcher(FILE *p, char *originalFileName);