#include <stdio.h>

int 
main()
{
    long int num;

    printf("The Size of int is %d bytes\n", sizeof(num));

    printf("Enter an Integer: ");
    scanf("%d", &num);

    printf("Enterd: %d", num);
}