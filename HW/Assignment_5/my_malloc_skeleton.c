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
}

// This is a helper function used by my_malloc. It searches through the heap using block headers to find a free block.
header_t *find_free_block(size_t size)
{

    header_t *ptr = (header_t *)heap;
    while (((char *)ptr) + size + BLOCK_HEADER_SIZE <= (char *)heap + HEAP_SIZE)
    {
        if (ptr->free && ptr->size >= size)
        {
            return ptr;
        }
        // Paranthesis here is crucial !
        ptr = (header_t *)(((char *)ptr) + ptr->size + BLOCK_HEADER_SIZE);
    }
    return NULL;
}

// This is another helper function used by my_malloc to split the blocks if necessary.
void split_block(header_t *block, size_t size)
{
    // Find the excess space. Will be size of the new block we create - BLOCK_HEADER_SIZE.
    size_t diff = block->size - size;
    // Change Size of the current block to just size.
    block->size = size;
    // Cast the address @ block+size as a header and mark as free with size size.
    header_t *ptr = (header_t *)(((char *)block) + size + BLOCK_HEADER_SIZE);
    ptr->free = true;
    ptr->size = diff - BLOCK_HEADER_SIZE;
}

// This is a helper function that coalesces blocks when freeing them.
// You need to implement this function.
void coalesce()
{
    header_t *prev = (header_t *)heap;
    header_t *next;
    while ((char *)prev + BLOCK_HEADER_SIZE + prev->size <= (char *)heap + HEAP_SIZE)
    {
        next = (header_t *)((char *)prev + prev->size + BLOCK_HEADER_SIZE);
        if (next->free && prev->free)
        {
            // If both prev and next are free then merge and donot update prev so
            // we can calculate next using the new merged size.
            prev->size = prev->size + next->size + BLOCK_HEADER_SIZE;
        }
        else
        {
            // Next or Prev was an allocated block so keep traversing.
            prev = next;
        }
    }
    return;
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
    coalesce(); // Coalesce if possible
}

int main()
{
    // Initialize the heap
    init_allocator();
    // Test the heap.
    void *ptr1 = my_malloc(500);
    printf("Allocated %ld bytes\n", ((header_t *)((char *)ptr1 - BLOCK_HEADER_SIZE))->size);
    my_free(ptr1);
    printf("Block freed\n");
    return 0;
}
