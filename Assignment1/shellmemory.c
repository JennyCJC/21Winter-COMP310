#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include"shellmemory.h"

struct SHELLMEMORY {
    char *variable;
    char *value;
};

struct SHELLMEMORY str[1000] = { {0} };

int findAndSet(char *var, char* val){
    struct SHELLMEMORY p = {var, val};
    int ct;
    for(ct = 0; ct < 1000; ct++){
	int i = 0;
        if(str[ct].variable != NULL){
        while(str[ct].variable[i] != '\0') {i++;}
	    if(strncmp(str[ct].variable, var, i) == 0){
                str[ct].value = val;
                return 0;   //variable found
            }
        }
        else break;
    }

    if(ct < 1000){
        str[ct] = p;
        return 1;   //variable saved
    }
    return -1;  //variable not found and not saved
}


int findAndPrint(char *var){
	
    for(int ct = 0; ct < 1000; ct++){
	int i = 0;
	 if(str[ct].variable != NULL){
	 while(str[ct].variable[i] != '\0') {i++;}
         if(strncmp(str[ct].variable, var, i) == 0){
                int j = 0;
                int k = 0;
                while(str[ct].value[j] != '\n'){ j++; }
                char temp[j+1];
                while(j >= k){
                    temp[k] = str[ct].value[k];
                    k++;
                    }
                temp[j] = '\0';
                printf("%s", temp);
                return(0);   //variable found
            }
        }
        else break;
    }
    printf("Variable does not exist");
    return -1;  //variable not found
}

