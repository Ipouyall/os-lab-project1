#include "prefix_predict.h"
#include <string.h>

int startswith(char *pre, char *str)
{
   if(strncmp(str, pre, strlen(pre)) == 0) return 1;
   return 0;
}
void updatehistory(char inp[]) {

    strcpy(command[command_num%5], inp);
    command_num++;
    if(size<5) size++;

}
void printcommand(char key[]){

    if(size<5){
        for (int i = 0; i < size; i++)
            if(startswith(key,command[i])) {
                printf("%s",command[i]);
                return;
            }
    }
    else {
        int endIndex = ((command_num % 5) + 4) % 5 ;
        int i = ( command_num % 5);
        while (i != endIndex)
        {
            if(startswith(key,command[i])) {
                printf("%s",command[i]);
                return;
            }
            i++;
            if(i==5) i = 0;
        }
        if(startswith(key,command[i])) printf("%s",command[i]);

    }
}