#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include"interpreter.h"
#include"shellmemory.h"
#include"shell.h"

void parse(char *input, char delimiter, char *tokens[]) {
	char buffer[1000];
	int s=0, b=0, t=0; // counters for input, buffer and tokens

	while(*(input+s) != '\0') {
		// skip leading spaces
		for(;*(input+s)==' ';s++);  

		// exit if end of input
		if (*(input+s) == '\0') break;

		// otherwise, add word to buffer
		b=0;
		while(*(input+s)!=' ' && *(input+s)!='\0') {
			buffer[b] = *(input+s);
			s++;
			b++;
		}
		buffer[b]='\0'; // make it a string

		// create a token
		tokens[t] = strdup(buffer);
		t++;
	}
	tokens[t] = NULL;
}

int main(int agrc, char* argv[]){

    char input[1000];

    printf("Welcome to Jenny's Shell!\nVersion 1.0 Created January 2021\n");

    while(1){
		
        printf("$");

		fgets(input, 999, stdin);

		input[999] = '\0';
		
        char *tokens[100];

        parse(input, ' ', tokens);

        interpreter(tokens);
    }

    return 0;
}




