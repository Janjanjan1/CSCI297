#include <stdio.h>
#include <stdlib.h>
#include "myarray.h"

typedef struct
{
    int size;
    int *array;
    int count;
} Dynamic_Array;

void initialize_array(Dynamic_Array *arr, int size)
{
    arr->size = size;
    arr->count = 0;
    arr->array = (int *)malloc(size * sizeof(int));
    if (arr->array == NULL)
    {
        fprintf(stderr, "Failed to Allocated Memory for Dynamic Array\n");
        exit(EXIT_FAILURE);
    }
}
void resize_array(Dynamic_Array *arr, int size)
{
    arr->size += size;
    int *temp = (int *)realloc(arr->array, arr->size * sizeof(int));
    // free(arr->array);
    if (temp == NULL)
    {
        fprintf(stderr, "Could not resize array ! \n");
    }
    arr->array = temp;
}
void append_array(Dynamic_Array *arr, int val)
{
    resize_array(arr, arr->size + 1);
    arr->array[arr->size - 1] = val;
}
void add_element(Dynamic_Array *arr, int val)
{
    if (arr->count >= arr->size)
    {
        resize_array(arr, arr->size + 10);
    }
    arr->array[arr->count] = val;
    arr->count++;
}
void free_array(Dynamic_Array *arr)
{
    free(arr->array);
    arr->array = NULL;
    arr->count = 0;
    arr->size = 0;
}

void copy_dynamic_array(Dynamic_Array *from_arr, Dynamic_Array *to_arr)
{
    if (from_arr->size != to_arr->size)
    {
        resize_array(to_arr, from_arr->size);
    }
    copyArray(from_arr->array, to_arr->array, from_arr->count);
    to_arr->count = from_arr->count;
}

int main()
{
    Dynamic_Array array;
    initialize_array(&array, 0);
    int current_number;
    while (1)
    {
        scanf("%d", &current_number);
        if (current_number == -1)
        {
            break;
        }
        add_element(&array, current_number);
    }
    // Copy array into array_2 by first initializing it to the same size and then the values:
    Dynamic_Array array_2;
    initialize_array(&array_2, array.size);
    copy_dynamic_array(&array, &array_2);
    // Sort:
    sortArray(array.array, array.count);
    // array is sorted while array_2 is not sorted.

    // Search:
    int number_to_search;
    printf("Enter a number to search: ");
    scanf("%d", &number_to_search);
    int num_comparisons_binary = 0;
    int num_comparisons_linear = 0;
    int b_search_index = binarySearch(array.array, array.count, number_to_search, &num_comparisons_binary);
    int l_search_index = linearSearch(array_2.array, array_2.count, number_to_search, &num_comparisons_linear);
    printf("\nItem: %d\n", number_to_search);
    printf("\nBinary Search ");
    if (b_search_index == -1)
    {
        printf("did not find your number.");
    }
    else
    {
        printf("found your number!");
        printf("\nAt Index: %d", b_search_index);
    }
    printf("\nNumber of Comparisons: %d\n", num_comparisons_binary);
    printf("\nLinear Search ");
    if (l_search_index == -1)
    {
        printf("did not find your number.");
    }
    else
    {
        printf("found your number!");
        printf("\nAt Index: %d\n", l_search_index);
    }
    printf("Number of Comparisons: %d\n", num_comparisons_linear);
    free_array(&array_2);
    free_array(&array);
}