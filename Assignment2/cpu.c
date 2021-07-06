#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "cpu.h"
#include "pcb.h"
#include "ram.h"
#include "interpreter.h"
#include "shell.h"

struct CPU 
{
    int IP;
    char IR[1000];
    int quanta;
};

static struct CPU cpu = {.quanta =2};

int check_cpu(){
    if(cpu.quanta == 2){
        return 0;
	}        
return 1;
}

void load_pcb(int pc){
    cpu.IP = pc;
}

int get_cpu_ip(){
    return cpu.IP;
}

int run_cpu(int quanta, int end){
	cpu.quanta--;
    

    while (cpu.quanta >= 0 && cpu.IP < end){
        char *tokens[100];
        copy_ram(cpu.IR, cpu.IP);
        //parse(cpu.IR, ' ', tokens);
        interpret(cpu.IR);
        cpu.IP++;
        cpu.quanta--;

        if(end == cpu.IP){
            cpu.quanta = 2;
            return 1;
        }
    }

    cpu.quanta = 2;
    
    return 0;
}
