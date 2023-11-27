#include <stdio.h>
#include <stdlib.h>

void copyArray(int fromArray[], int toArray[], int size)
{
    for (int i = 0; i < size; i++)
    {
        toArray[i] = fromArray[i];
    }
};

void sortArray(int arr[], int size)
{
    // Bubble Sort.
    for (int i = 0; i < size - 1; i++)
    {
        for (int j = i; j < size; j++)
        {
            if (arr[i] > arr[j])
            {
                // Swap Function:
                arr[i] = arr[i] + arr[j];
                arr[j] = arr[i] - arr[j];
                arr[i] = arr[i] - arr[j];
            }
        }
    }
}

int linearSearch(int arr[], int size, int target, int *numComparisons)
{
    for (int i = 0; i < size; i++)
    {
        (*numComparisons)++;
        if (arr[i] == target)
        {
            return i;
        }
    }
    return -1;
}

int binarySearch(int arr[], int size, int target, int *numComparisons)
{
    int lo = 0;
    int hi = size - 1;
    int mid;
    while (1)
    {
        mid = (lo + hi) / 2;
        (*numComparisons)++;
        if (arr[mid] < target)
        {
            lo = mid + 1;
        }
        else if (arr[mid] > target)
        {
            hi = mid - 1;
        }
        else
        {
            return mid;
        }
        if (lo > hi)
        {
            return -1;
        }
    }
}
