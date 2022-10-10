#include <assert.h>
#include <stdio.h>
#include <stdbool.h>

#include "mem_alloc_fast_pool.h"
#include "my_mmap.h"
#include "mem_alloc.h"

void init_fast_pool(mem_pool_t *p, size_t size, size_t min_request_size, size_t max_request_size)
{
    /* TODO Init Fast Pool IMPLEMENTED */
    //my_mmap is gonna used to initialize the pool
    //printf("%s:%d: Please, implement me!\n", __FUNCTION__, __LINE__);
    void* temp = my_mmap(size);
    int total_block = size/max_request_size;
    
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

void *mem_alloc_fast_pool(mem_pool_t *pool, size_t size)
{
    /* TODO Alloc Fast Pool IMPLEMENTED */
    //printf("%s:%d: Please, implement me!\n", __FUNCTION__, __LINE__);
    struct mem_fast_free_block* temp = ((struct mem_fast_free_block*) pool->first_free);
    if(temp == NULL){
        return NULL;
    }
    pool->first_free = temp->next;
    
    return temp;
}

bool mem_free_fast_pool(mem_pool_t *pool, void *b)
{
    /* TODO Free Fast Pool IMPLEMENTED */
    //printf("%s:%d: Please, implement me!\n", __FUNCTION__, __LINE__);
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
