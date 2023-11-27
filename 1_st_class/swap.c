#include <stdio.h>
#include "swap_ints.h"

int main()
{
    int a;
    a = 0;
    int b;
    b = 1;
    swap_ints(&a, &b);
    printf("%d, %d\n", a, b);
}   
