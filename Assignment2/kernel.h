#ifndef KERNEL_H
#define KERNEL_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "pcb.h"
#include "cpu.h"
#include "ram.h"
#include "shell.h"

void addToReady(struct PCB * pPCB);

void rotate();

void remove_node();

int myinit(char *filename);

int scheduler();

int try(int agrc, char* argv[]);

#endif
