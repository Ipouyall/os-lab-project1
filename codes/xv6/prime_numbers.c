#include <stdio.h>
#include <stdlib.h>

// program to get two number as command line arguments and print prime number between them
int main(int argc, char *argv[])
{
    int i, j, flag, start, end, n1, n2;
    n1 = atoi(argv[1]);
    n2 = atoi(argv[2]);
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
            printf("%d ", i);
        }
    }
    printf("\n");
    return 0;
}