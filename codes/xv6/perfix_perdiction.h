#include <stdio.h>

int commandNum=0,size=0;
char color[5][30];

int startsWith(char *pre, char *str);
void updateHistory(char inp[]);
void print(char key[]);