extern int command_num ,sizeCommand ;
extern char command[15][30];

int startswith(char *pre, char *str);
void updatehistory(char* inp,int sizeCommand);
char* printcommand(char key[]);