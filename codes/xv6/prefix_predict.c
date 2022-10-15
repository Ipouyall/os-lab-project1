#include "prefix_predict.h"

#include "types.h"
#include "user.h"
int command_num = 0, sizeCommand = 0 ;
char command[15][30];

static char*
stringcpy(char *s, const char *t,int size)
{
  char *os;

  os = s;
  while(((*s++ = *t++) != 0))
    ;
  return os;
}        
void updatehistory(char* inp,int sizeCommand) {

    stringcpy(command[command_num%15], inp , sizeCommand);
    command_num++;
    if(sizeCommand<15) sizeCommand++;

}


int startswith(char *pre, char *str)
{
   if(strncmp(str, pre, strlen(pre)) == 0) return 1;
   return 0;

   /*
       int len1=strlen(pre),len2=strlen(str);
    if(len1>len2) return 0;
    else
        for (int i = 0; i < len1; i++)
            if (pre[i]!=str[i])
                return 0;
   return 1;*/
}
char* printcommand(char key[]){
    if(sizeCommand<15){
        for (int i = 0; i < sizeCommand; i++)
            if(startswith(key,command[i])) {
                // printf(1,"%s",command[i]);
                return command[i];
            }
    }
    else {
        int endIndex = ((command_num % 15) + 14) % 15 ;
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
        if(startswith(key,command[i])) return command[i];

    }
}