#include <stdio.h>
#include <stdlib.h>
int main()
{
    int *arr = (int *)malloc(10 * sizeof(int));

    printf("%p\n", arr);
    for (int i = 0; i < 10; i++)
    {
        arr[i] = i + 1;
    }
    int *temp = (int *)realloc(arr, 20 * sizeof(int));
    printf("%p\n", arr);
    printf("%p\n", temp);
    for (int i = 0; i < 10; i++)
    {
        printf("From Arr After: %x\n", arr[i]);
    }
    arr = temp;
    for (int i = 0; i < 10; i++)
    {
        printf("From Reassigned Arr: %d\n", arr[i]);
    }
}