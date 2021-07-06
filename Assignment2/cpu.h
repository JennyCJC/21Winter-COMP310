#ifndef CPU_H
#define CPU_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "pcb.h"
#include "ram.h"
#include "interpreter.h"
#include "shell.h"

int check_cpu();

int get_cpu_ip();

void load_pcb(int pc);

int run_cpu(int quanta, int end);

#endif
