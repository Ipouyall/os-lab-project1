#include <stdio.h>
#include <string.h>

int command_num=0,size=0;
char command[5][30];

int startswith(char *pre, char *str);
void updatehistory(char inp[]);
void printcommand(char key[]);