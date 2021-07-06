#ifndef RAM_H
#define RAM_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void copy_ram(char* IR, int ip);

int addToRam(FILE *file, int **start, int **end);

void unload(int end);

#endif
