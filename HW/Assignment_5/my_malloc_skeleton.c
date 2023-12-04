#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define HEAP_SIZE 1024 // Size of the heap
#define BLOCK_HEADER_SIZE sizeof(header_t)

char heap[HEAP_SIZE];

// Heap header struct
typedef struct header
{
    size_t size;
    bool free;
} header_t;

// This is the initial allocator. It covers the entire heap minus the block header size.
void init_allocator()
{
    header_t *header = (header_t *)heap;
    header->size = HEAP_SIZE - BLOCK_HEADER_SIZE;
    header->free = true;
    printf("\nBeginning of the Heap:%p | End of the Heap:%p\n", heap, ((char *)heap) + HEAP_SIZE);
}

// This is a helper function used by my_malloc. It searches through the heap using block headers to find a free block.
header_t *find_free_block(size_t size)
{
    header_t *ptr = (header_t *)heap;
    while (((char *)ptr) + size <= (char *)heap + HEAP_SIZE)
    {
        printf("\nPTR:%p, Is PTR FREE:%d, PTR->size:%zu | Req Size:%zu", ptr, ptr->free, ptr->size, size);
        if (ptr->free && ptr->size >= size)
        {
            return ptr;
        }
        ptr = (header_t *)((char *)ptr) + ptr->size + 1;
    }
    printf("\n Quitting Ptr at: %p", ptr);
    return NULL;
}

// This is another helper function used by my_malloc to split the blocks if necessary.
void split_block(header_t *block, size_t size)
{
    // printf("\nStaring:%p|Ending:%p", (char *)block, (char *)block + size);
    size_t diff = block->size - size;
    block->size = size;
    header_t *ptr = (header_t *)((char *)block) + size + 1;
    ptr->free = true;
    ptr->size = diff - BLOCK_HEADER_SIZE;
    // printf("\nSize Requested: %zu, Difference: %zu, New Free Space:%p\n", size, diff, ptr);
}

// This is a helper function that coalesces blocks when freeing them.
// You need to implement this function.
void coalesce()
{

    // COMPLETE THIS FUNCTION
}

// The main malloc function that allocated memory, or returns Null if no block is found.
// This function has been implemented for you.
void *my_malloc(size_t size)
{
    header_t *block = find_free_block(size);

    if (!block)
    {
        printf("\nNo FREE BLOCK FOUND\n");
        return NULL; // No free block found
    }
    if (block->size > size + BLOCK_HEADER_SIZE)
    {
        split_block(block, size);
    }
    block->free = false;
    return (char *)block + BLOCK_HEADER_SIZE;
}

// This function frees a pointer, pointing to a block on the heap.
// This function has been implemented for you.
void my_free(void *ptr)
{
    if (!ptr)
    {
        return;
    }
    header_t *block = (header_t *)((char *)ptr - BLOCK_HEADER_SIZE);
    block->free = true;
    // coalesce(); // Coalesce if possible
}

int main()
{

    // Initialize the heap
    init_allocator();
    printf("Size of BlockHeader:%zu", BLOCK_HEADER_SIZE);
    // Test the heap.
    void *ptr1 = my_malloc(500);
    printf("\nFree Space Ptr: %p | Ending:%p \n", ptr1, ((char *)heap) + BLOCK_HEADER_SIZE + 500);
    printf("Allocated %ld bytes\n", ((header_t *)((char *)ptr1 - BLOCK_HEADER_SIZE))->size);
    // my_free(ptr1);
    // printf("Block freed\n");
    void *ptr2 = my_malloc(10);
    printf("\nFree Space Ptr: %p\n", ptr2);

    return 0;
}
