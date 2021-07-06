#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<time.h>
#include"kernel.h"
#include <stdbool.h>
int pageFault(struct PCB* pcb, bool victmedPage);

int launcher(FILE *p, char *originalFileName);
