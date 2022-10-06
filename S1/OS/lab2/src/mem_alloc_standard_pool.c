#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <stdbool.h>

#include "mem_alloc_types.h"
#include "mem_alloc_standard_pool.h"
#include "my_mmap.h"
#include "mem_alloc.h"

/////////////////////////////////////////////////////////////////////////////

#ifdef STDPOOL_POLICY
/* Get the value provided by the Makefile */
std_pool_placement_policy_t std_pool_policy = STDPOOL_POLICY;
#else
std_pool_placement_policy_t std_pool_policy = DEFAULT_STDPOOL_POLICY;
#endif

/////////////////////////////////////////////////////////////////////////////

const int SIZE_OF_POINTER = 8;

void set_free_size(struct mem_std_free_block *address, size_t size){
    set_block_free(&address->header);
    set_block_size(&address->header, size);
}

void init_standard_pool(mem_pool_t *p, size_t size, size_t min_request_size, size_t max_request_size)
{
    /* TODO Init Standard Pool IMPLEMENTED */
    printf("%s:%d: Please, implement me!\n", __FUNCTION__, __LINE__);
    void* temp = my_mmap(size);
    struct mem_std_free_block *free_mem = temp;
    p->start = p->first_free = free_mem;
    p->end = (char*) temp + size-(SIZE_OF_POINTER*4); 
    free_mem->next = NULL;
    free_mem->prev = NULL;
    set_free_size(free_mem, size - (SIZE_OF_POINTER*2));
}

bool useableMemory(struct mem_std_free_block *curr, int size){
	return curr != NULL && get_block_size(&curr->header) >= size && is_block_free(&curr->header);
}

void *mem_alloc_standard_pool(mem_pool_t *pool, size_t size)
{
    /* TODO Alloc Standard Pool IMPLEMENTED */
    printf("%s:%d: Please, implement me!\n", __FUNCTION__, __LINE__);

    struct mem_std_free_block* curr = pool->first_free;
    while(!useableMemory(curr, size)){
        printf(">>%p (%d)\n", curr, get_block_size(&curr->header));
        curr = curr->next;
    }
    if(curr != NULL){
        int remaining_size = get_block_size(&curr->header) - (size + (SIZE_OF_POINTER*2));

        struct mem_std_allocated_block* allocated = curr;
        set_block_used(&allocated->header);
        set_block_size(&allocated->header, size);
        if(remaining_size < (SIZE_OF_POINTER*4)){
            struct mem_std_free_block *temp = curr;
            if(temp->prev != NULL)
                temp->prev->next = temp->next;
            if(temp->next != NULL)
                temp->next->prev = temp->prev;
        }
        else{
            struct mem_std_free_block* new_free = (void *)((char*)allocated + (size+ (SIZE_OF_POINTER*2)));
            new_free->next = curr->next;
            new_free->prev = curr->prev;
            if(curr->next != NULL)
                curr->next->prev = new_free;
            if(curr->prev != NULL)
                curr->prev->next = new_free;
            set_block_free(&new_free->header);
            set_block_size(&new_free->header, remaining_size);
            if(curr == pool->first_free)
                pool->first_free = new_free;
        }
        return (char*)allocated + SIZE_OF_POINTER;
    }
    return NULL;
}

void mem_free_standard_pool(mem_pool_t *pool, void *addr)
{
    /* TODO Free Standard Pool IMPLEMENTED */
    printf("%s:%d: Please, implement me!\n", __FUNCTION__, __LINE__);

    struct mem_std_free_block* freed = (void*)((char*)addr - SIZE_OF_POINTER);
    set_free_size(freed, get_block_size(&freed->header));

    struct mem_std_free_block* curr = pool->first_free;
    bool isStart = false;

    if(freed < curr){
        freed->prev = NULL; 
        freed->next = curr;
        curr->prev = freed;
        pool->first_free = freed;
        isStart = true;
    }
    else{
        while(curr != NULL){
            if(curr->next != NULL && curr->next < freed){
                curr = curr->next;
            }
            else{
                freed->next = curr->next;
                freed->prev = curr;
                if(curr->next != NULL)
                    curr->next->prev = freed;
                curr->next = &freed;
                break;
            }
        }
    }
    if(freed->next == (void *) freed + get_block_size(&freed->header) + (SIZE_OF_POINTER*2)){
        struct mem_std_free_block* temp = freed->next;
        freed->next = temp->next;
        if(temp->next != NULL)
            temp->next->prev = freed;

        int total_size = get_block_size(&freed->header)+ get_block_size(&temp->header) + (SIZE_OF_POINTER*2);
        
        set_block_used(&temp->header);
        
        set_free_size(freed, total_size);
    }
    if(!isStart && freed == (void *) curr + get_block_size(&curr->header) + (SIZE_OF_POINTER*2)){
        curr->next = freed->next;
        if(freed->next != NULL)
            freed->next->prev = curr;
        int total_size = get_block_size(&curr->header)+ get_block_size(&freed->header) + (SIZE_OF_POINTER*2);
        
        set_block_used(&freed->header);

        set_free_size(curr, total_size);
    }
    
}

size_t mem_get_allocated_block_size_standard_pool(mem_pool_t *pool, void *addr)
{
    /* TODO Get Allocated Block Size IMPLEMENTED */
    printf("%s:%d: Please, implement me!\n", __FUNCTION__, __LINE__);
    struct mem_std_allocated_block* allocated = (void*)((char*)addr -SIZE_OF_POINTER);
    return get_block_size(&allocated->header);
}
