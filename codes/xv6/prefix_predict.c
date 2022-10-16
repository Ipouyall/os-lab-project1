#include "prefix_predict.h"

#include "types.h"
#include "user.h"
int command_num = 0, size_command = 0 ;
char command[MAX_COMMAND_NUM][128];

static char* stringcpy(char *s, const char *t,int size)
{
  char *os;

  os = s;
  while(((*s++ = *t++) != 0))
    ;
  return os;
}        
void update_history(char* inp,int size_command) {

    stringcpy(command[command_num % MAX_COMMAND_NUM ], inp , size_command);
    command_num++;
    if(size_command < MAX_COMMAND_NUM) size_command++;

}
int starts_with(char *pre, char *str)
{
   if(strncmp(str, pre, strlen(pre)) == 0) return 1;
   return 0;
}