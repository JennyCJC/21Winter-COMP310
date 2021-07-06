#ifndef PCB_H
#define PCB_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

struct PCB
{
    int PC;
    int start;
    int end;
};

struct PCB * makePCB(int start, int end);
#endif
