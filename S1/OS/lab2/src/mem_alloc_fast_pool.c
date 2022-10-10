#include <assert.h>
#include <stdio.h>
#include <stdbool.h>

#include "mem_alloc_fast_pool.h"
#include "my_mmap.h"
#include "mem_alloc.h"

/* Function: init_fast_pool
----------------------------------------------------------------
Initializes a fast pool with the given parameters

    p - address of the pool
    size: size of the pool
    min_request_size: minimum size of the request to the block
    max_request_size: maximum size of the request to the block
*/
void init_fast_pool(mem_pool_t *p, size_t size, size_t min_request_size, size_t max_request_size)
{
    /* Requests a chunk of memory and
       calculates the number of blocks */
    void* temp = my_mmap(size);
    int total_block = size/max_request_size;
    
    /* Initializes the linked list of free blocks */
    struct mem_fast_free_block *prev = (struct mem_fast_free_block*) temp, *curr;
    p->start = p->first_free = prev;
    for(int i = 1; i < total_block; i++){
        curr = (struct mem_fast_free_block*)((char*) prev + max_request_size);
        prev->next = curr;
        prev = curr;
    }
    p->end = curr;
    curr->next = NULL;
}

/* Function: mem_alloc_fast_pool
----------------------------------------------------------------
Allocates a memory in the pool using the linked list of free blocks.
Loops through the list until finds a free block.

    pool: addres to the pool
    size: size of the memory allocation request

    returns:if allocated, the address to the allocated block
            else null
*/
void *mem_alloc_fast_pool(mem_pool_t *pool, size_t size)
{
    struct mem_fast_free_block* temp = ((struct mem_fast_free_block*) pool->first_free);
    if(temp == NULL){
        return NULL;
    }
    pool->first_free = temp->next;
    
    return temp;
}

/* Function: mem_fast_free
----------------------------------------------------------------
Frees a block with the given adress.
And the freed block will be inserted to the linked list of free blocks.

    pool - address to the pool
    b - address to the block

*/
bool mem_free_fast_pool(mem_pool_t *pool, void *b)
{
    struct mem_fast_free_block* curr = pool->first_free;
    while(curr != NULL){
        if((void *)curr == (void *) b){
            return false;
        }
        curr = curr->next;
    }

    ((struct mem_fast_free_block*)b)->next = ((struct mem_fast_free_block*) pool->first_free);
    pool->first_free = b;

    return true;
}

size_t mem_get_allocated_block_size_fast_pool(mem_pool_t *pool, void *addr)
{
    size_t res;
    res = pool->max_request_size;
    return res;
}
