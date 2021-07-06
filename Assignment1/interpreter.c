#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <unistd.h>

#include"interpreter.h"
#include"shellmemory.h"
#include"shell.h"


int interpreter(char *tokens[]){
    if(strncmp(tokens[0], "\n", 2) == 0) { return 0; }
    else if(strncmp(tokens[0], "help", 4) == 0 && tokens[1] == NULL){   //use strncmp to avoid when string not terminated with a null character
            help();
            printf("\n");
        }
        else if(strncmp(tokens[0], "quit", 4) == 0 && tokens[1] == NULL){
            quit();
        }
        else if(strncmp(tokens[0], "set", 3) == 0 && tokens[1] != NULL && tokens[2] != NULL && tokens[3] == NULL){
            set(tokens[1], tokens[2]);
 
        }
        else if(strncmp(tokens[0], "print", 5) == 0 && tokens[1] != NULL && tokens[2] == NULL){
            print(tokens[1]);
            printf("\n");
        }
        else if(strncmp(tokens[0], "run", 3) == 0 && tokens[1] != NULL && tokens[2] == NULL){
            run(tokens[1]);
            printf("\n");
        }
        else{
            printf("Unknown command.");
            printf("\n");
            return 1;
        }

        return 0;
}

void help(){
    printf("Valid commands are:\n");
    printf("quit\n");
    printf("set\n");
    printf("print\n");
    printf("run");
}

void quit(){
    exit(0);
}

int set(char *var, char* val){
    
    findAndSet(var, val);
    return 0;    
}

int print(char *var){
    return findAndPrint(var);
}

int run(char *script){
    FILE *file;
    char line[1000];
    int i = 0;

    
	while(script[i] != '\n'){
		i++;
	}
	script[i] = '\0';

    file = fopen(script, "rt");
    if(file == NULL) //Where we able to open the csv file?
	{
		fprintf(stderr, "Script not found.");
		return 1;
	}

    while(!feof(file)){
        fgets(line, 999, file);
        if(feof(file)) break;   // end of data

        //printf("\n");

        char *tokens[100];
        parse(line, ' ', tokens);

        interpreter(tokens);

    }

    fclose(file);
    return 0;
}
