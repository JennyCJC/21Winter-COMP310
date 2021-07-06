#include<string.h>
#include"ram.h"
#include"interpreter.h"
#include"shell.h"
#include"memorymanager.h"

#define DEFAULT_QUANTA 2

struct CPU { 
    int IP; 
    int offset;
    char IR[1000]; 
    int quanta; 
} CPU;

/*
This method passes a quanta.
It will then executes quanta number of lines in RAM as from the address
pointed by the CPU IP.
Returns 2 if EOF.
Returns 1 if pageFault.
Returns 0 if success
*/
int run(int quanta){
    // If a quanta greater than the default quanta of 2 is passed,
    // run will instead execute only default quanta of lines.
    if (quanta > DEFAULT_QUANTA ){
        quanta = DEFAULT_QUANTA;
    }

    for (int i = 0; i < quanta; i++)
    {
        if (CPU.offset < 4 && CPU.IP < 40 && CPU.IP != -4){
            if( ram[CPU.IP+CPU.offset] == NULL){ // If within boundary we have a ram space not used, this is a hole and indicates the end of file.
                return 2;
            }
            //CPU.IR = strdup(ram[CPU.IP+CPU.offset]);
            strcpy(CPU.IR,ram[CPU.IP+CPU.offset]);
            int errorCode = parse(CPU.IR);
            // Do error checking and if error, return error
            if (errorCode != 0){
            // Display error message if fatal
            if (errorCode < 0){
                ram[CPU.IP][strlen(ram[CPU.IP])-2]='\0';
                displayCode(errorCode,ram[CPU.IP]);
            }
            return errorCode;
            }
            CPU.offset++;
        }
        else{
            return 1;
        }
    }
    return 0;
}
