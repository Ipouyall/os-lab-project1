#include "perfix_perdiction.h"
#include <cstring>

int startsWith(char *pre, char *str)
{
    size_t lenpre = strlen(pre),
           lenstr = strlen(str);
    return lenstr < lenpre ? 0 : memcmp(pre, str, lenpre) == 0;
}
void updateHistory(char inp[]) {

    strcpy(color[commandNum%5], inp);
    commandNum++;
    if(size<5) size++;

}
void print(char key[]){

    if(size<5){
        for (int i = 0; i < size; i++)
            if(startsWith(key,color[i])) {
                printf("%s",color[i]);
                return;
            }
    }
    else {
        int endIndex = ((commandNum % 5) + 4) % 5 ;
        int i = ( commandNum % 5);
        while (i != endIndex)
        {
            if(startsWith(key,color[i])) {
                printf("%s",color[i]);
                return;
            }
            i++;
            if(i==5) i = 0;
        }
        if(startsWith(key,color[i])) printf("%s",color[i]);

    }
}