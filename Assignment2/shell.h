#ifndef SHELL_H
#define SHELL_H

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include"interpreter.h"
#include"shellmemory.h"

void parse(char *input, char delimiter, char *tokens[]);

int shellUI(int agrc, char* argv[]);

#endif