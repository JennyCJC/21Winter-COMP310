#ifndef INTERPRETER_H
#define INTERPRETER_H

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

int interpreter(char *tokens[]);

void help();

void quit();

int set();

int print();

int run();

#endif