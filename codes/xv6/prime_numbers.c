#include "types.h"
#include "fcntl.h"
#include "user.h"

// custom implementation of atoi
int achar_to_int(char c[])
{
    int num = 0, i;
    for(i=0; c[i] != '\0'; i++)
    {
        if ('0' <= c[i] <= '9')
        {
            num = num * 10 + (c[i] - '0');
        }
        else
        {
            return -1;
        }
    }
    return num;
}

// program to get two number as command line arguments and print prime number between them
int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf(2, "Error in calling; please call like:\n prime_numbers <number1> <number2>\n");
        exit();
    }
    int i, j, not_prime, start, end, n1, n2;

    n1 = achar_to_int(argv[1]);
    n2 = achar_to_int(argv[2]);
    if (n1 == -1 || n2 == -1)
    {
        printf(2, "Error in numbers; please enter uint numbers.\n");
        exit();
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
    if (start < 2)
    {
        start = 2;
    }

    unlink("prime_numbers.txt");
    int file = open("prime_numbers.txt", O_CREATE | O_RDWR);

    for (i = start; i <= end; i++)
    {
        not_prime = 0;
        for (j = 2; j <= i / 2; j++)
        {
            if (i % j == 0)
            {
                not_prime = 1;
                break;
            }
        }
        if (not_prime == 0)
        {
            printf(file, "%d ", i);
        }
    }
    printf(file, "\n");
    close(file);
    exit();
}