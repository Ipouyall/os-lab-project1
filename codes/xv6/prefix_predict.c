#include "prefix_predict.h"

#include "types.h"
#include "user.h"
int command_num = 0, size = 0 ;
char command[15][30];

static char*
stringcpy(char *s, const char *t,int size)
{
  char *os;

  os = s;
  while(((*s++ = *t++) != 0) && size--)
    ;
  return os;
}
void updatehistory(char* inp,int size) {

    stringcpy(command[command_num%15], inp , size);
    command_num++;
    if(size<15) size++;

}


int startswith(char *pre, char *str)
{
   if(strncmp(str, pre, strlen(pre)) == 0) return 1;
   return 0;
}
char printcommand(char key[]){

    if(size<15){
        for (int i = 0; i < size; i++)
            if(startswith(key,command[i])) {
                // printf(1,"%s",command[i]);
                return command[i];
            }
    }
    else {
        int endIndex = ((command_num % 15) + 4) % 15 ;
        int i = ( command_num % 15);
        while (i != endIndex)
        {
            if(startswith(key,command[i])) {
                // printf(1,"%s",command[i]);
                return command[i];
            }
            i++;
            if(i==15) i = 0;
        }
        if(startswith(key,command[i])) command[i];

    }
}