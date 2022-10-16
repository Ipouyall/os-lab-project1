#define MAX_COMMAND_NUM 15
extern int command_num ,size_command ;
extern char command[MAX_COMMAND_NUM][128];

int starts_with(char *pre, char *str);
void update_history(char* inp,int size_command);