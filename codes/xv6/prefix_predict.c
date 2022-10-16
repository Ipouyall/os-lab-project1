#include "prefix_predict.h"

#include "types.h"
#include "user.h"
int command_num = 0, sizeCommand = 0 ;
char command[15][30];

static char* stringcpy(char *s, const char *t,int size)
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
}