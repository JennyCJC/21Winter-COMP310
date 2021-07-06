#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "ram.h"

int p1 = -1;
int p2 = -1;
int p2_start = 0;
int p3 = -1;
int p3_start = 0;
int ct = 0;

char *ram[1000] = { NULL };

void copy_ram(char* IR, int ip){
    strcpy(IR, ram[ip]);
}

void unload(int end){
    if(end == 1001 || end == p1){
        for(int i = 0; i < p1; i++){
            free(ram[i]);
            ram[i] = NULL;
        }
        p1 = -1;
        ct = 0;
    }
    else if(end == 1002 || end == p2){
        for(int i = p2_start; i < p2; i++){
            free(ram[i]);
            ram[i] = NULL;
        }
        p2 = -1;
        ct = 0;
    }
    else if(end == 1003 || end == p3){
        for(int i = p3_start; i < p3; i++){
            free(ram[i]);
            ram[i] = NULL;
        }
        p3 = -1;
        ct = 0;
    }
    else if(end == 0){    //case when ram full
        for(int i = 0; i < 1000; i++){
            free(ram[i]);
            ram[9] = NULL;
        }
        p1 = -1;
        p2 = -1;
        p3 = -1;
        ct = 0;

    }
}

int addToRam(FILE *file, int **start, int **end){
    if(p1 == -1){
        **start = 0;
    }
    else if(p2 == -1){
        p2_start = p1;
        **start = p1;
    }
    else if(p3 == -1){
        p3_start = p2;
        **start = p2;
    }
    else{
        printf("more programs than expected");
        return 2;
    }
    //printf("hi");
    
    char line[1000];

    while(!feof(file) && ct < 1000){
        fgets(line, 999, file);
        if(feof(file)) break;
        ram[ct] = strdup(line);
        ct++;
    }

    if(ct == 1000){
        printf("ERROR: Not enough RAM to add program.");
        return 1;
    }

    if(p1 == -1){
        p1 = ct;
        *end = &p1;
    }
    else if(p2 == -1){
        p2 = ct;
        *end = &p2;
    }
    else if(p3 == -1){
        p3 = ct;
        *end = &p3;
    }
    else{
        printf("more programs than expected");
        return 2;
    }
    return 0;
    
}

