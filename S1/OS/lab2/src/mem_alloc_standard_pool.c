#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

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

void init_standard_pool(mem_pool_t *p, size_t size, size_t min_request_size, size_t max_request_size)
{
    /* TODO Init Standard Pool IMPLEMENTED */
    printf("%s:%d: Please, implement me!\n", __FUNCTION__, __LINE__);
    void* temp = my_mmap(size);
    p->start = p->first_free = temp;
    p->end = (char*) temp + size; 
    struct mem_std_free_block *free_mem = temp;
    free_mem->next = NULL;
    free_mem->prev = NULL;

    set_block_free(&free_mem->header);
    set_block_size(&free_mem->header, size);
}

void *mem_alloc_standard_pool(mem_pool_t *pool, size_t size)
{
    /* TODO Alloc Standard Pool IMPLEMENTED */
    printf("%s:%d: Please, implement me!\n", __FUNCTION__, __LINE__);
    struct mem_std_allocated_block* allocated = pool->first_free;
    struct mem_std_allocated_block* temp = (char*)allocated + 8;
    set_block_used(&allocated->header);
    set_block_size(&allocated->header, size);
    pool->first_free = (char*)allocated + size+ (8*2);
    
    return temp;
}

void mem_free_standard_pool(mem_pool_t *pool, void *addr)
{
    /* TODO Free Standard Pool IMPLEMENTED */
    printf("%s:%d: Please, implement me!\n", __FUNCTION__, __LINE__);
    struct mem_std_free_block* freed = (char*)addr - 8;
    set_block_free(&freed->header);
    struct mem_std_free_block* curr = pool->first_free;
    if(freed < curr){
        freed->prev = NULL; 
        freed->next = pool->first_free;
        curr->prev = freed;
        pool->first_free = freed;
    }
    else{
        while(curr->next != NULL && curr->next < freed){
            if(curr->next != NULL){
                curr = curr->next;
            }
            else{
                break;
            }
        }
        freed->next = curr->next;
        freed->prev = curr;
        curr->next->prev = freed;
        curr->next = freed;
    }
    
}

size_t mem_get_allocated_block_size_standard_pool(mem_pool_t *pool, void *addr)
{
    /* TODO Get Allocated Block Size IMPLEMENTED */
    printf("%s:%d: Please, implement me!\n", __FUNCTION__, __LINE__);
    struct mem_std_allocated_block* allocated = (char*)addr -8;
    return get_block_size(&allocated->header);
}
