extern int command_num ,size ;
extern char command[15][30];

int startswith(char *pre, char *str);
void updatehistory(char* inp,int size);
char printcommand(char key[]);