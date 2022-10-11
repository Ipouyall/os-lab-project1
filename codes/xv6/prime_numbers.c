#include "types.h"
#include "fcntl.h"
#include "user.h"

int achar_to_int(char c[]){
    int num = 0, i;
    if (c[0] == '-'){
        return -1;
    }
    for(i=0; c[i] != '\0'; i++){
        num = num*10 + (c[i] - '0');
    }
    return num;
}

int int_len(int num){
    int len = 1;
    num /= 10;
    while (num != 0){
        len++;
        num /= 10;
    }
    return len;
}

char* int_to_achar(int num, int len){
    char c[len+1];
    int i;
    for(i=len-1; i>=0; i--){
        c[i] = (num%10) + '0';
        num /= 10;
    }
    c[len] = '\0';
    return c;
}

// program to get two number as command line arguments and print prime number between them
int main(int argc, char *argv[])
{
    if (argc != 3) {

    }
    int i, j, flag, start, end, n1, n2;

    n1 = achar_to_int(argv[1]);
    n2 = achar_to_int(argv[2]);
    if (n1 == -1 || n2 == -1) {

    }
    if (n1 > n2)
    {
        start = n2;
        end = n1;
    }
    else
    {
        start = n1;
        end = n2;
    }

    unlink("prime_numbers.txt");
    int file = open("prime_numbers.txt", O_CREATE | O_RDWR);

    for (i = start; i <= end; i++)
    {
        flag = 0;
        for (j = 2; j <= i / 2; j++)
        {
            if (i % j == 0)
            {
                flag = 1;
                break;
            }
        }
        if (flag == 0)
        {
            int len = int_len(i);
            char* n = int_to_achar(i, len);
            write(file, n, strlen(n));
        }
    }
    close(file);
}