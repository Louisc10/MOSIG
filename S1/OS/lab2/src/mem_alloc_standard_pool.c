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
    set_block_size(&free_mem->header, size-(8*2));
}

bool useableMemory(struct mem_std_free_block *curr, int size){
	return curr != NULL && get_block_size(&curr->header) >= size && is_block_free(&curr->header);
    // return true;
}

void *mem_alloc_standard_pool(mem_pool_t *pool, size_t size)
{
    /* TODO Alloc Standard Pool IMPLEMENTED */
    printf("%s:%d: Please, implement me!\n", __FUNCTION__, __LINE__);

/*
    struct data *curr = pool;
	while(!useableMemory(curr, size)){
		curr = curr->next;
	}
	if(curr != NULL){
		struct data *temp = (struct data *)malloc(sizeof(struct data));
		temp->next = curr->next;
		temp->prev = curr;
		temp->isUsed = false;
		temp->size = (curr->size) - (size + (8*2));
		
		if(curr->next != NULL)
			curr->next->prev = temp;
		curr->next = temp;
		curr->isUsed = true;
		curr->size = size;
	}
	return pool;
*/  

    struct mem_std_free_block* curr = pool->first_free;
    while(!useableMemory(curr, size)){
        curr = curr->next;
    }
    if(curr != NULL){
        /*
        struct mem_std_free_block* temp = (void*) curr + (size + (8*2));
        temp->next = curr->next;
        temp->prev = curr->prev;
        set_block_free(&temp->header);
        int remaining_size = get_block_size(&curr->header) - (size + (8*2));
        set_block_size(&temp->header, remaining_size);

        struct mem_std_allocated_block *c2 = curr;
        set_block_used(&c2->header);
        set_block_size(&c2->header, size);
        return (struct mem_std_allocated_block *) c2 + (8);*/
    
        int remaining_size = get_block_size(&curr->header) - (size + (8*2));
        struct mem_std_allocated_block* allocated = curr;
        struct mem_std_allocated_block* temp = (char*)allocated + 8;
        set_block_used(&allocated->header);
        set_block_size(&allocated->header, size);

        struct mem_std_free_block* new_free = (void *)((char*)allocated + (size+ (8*2)));
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
        
        return temp;

    }
    return curr;
}

void mem_free_standard_pool(mem_pool_t *pool, void *addr)
{
    /* TODO Free Standard Pool IMPLEMENTED */
    printf("%s:%d: Please, implement me!\n", __FUNCTION__, __LINE__);
    /*
    struct data *curr = pool;
	while(curr != NULL && curr != addr){
		curr = curr->next;
	}
	if(curr != NULL){S
		bool flag = false;
		if(isFree(curr->next)){
			flag = true;
			puts("Case 1");
			struct data *temp = curr->next;
			curr->next = temp->next;
			if(temp->next != NULL)
				temp->next->prev = curr;
			curr->size+= temp->size + (8*2);
			curr->isUsed = false;
			free(temp);
		}
		
		if(isFree(curr->prev)){
			flag = true;
			puts("Case 2");
			struct data *temp = curr;
			curr->prev->next = temp->next;
			curr->next->prev = temp->prev;
			temp->prev->size+= temp->size + (8*2);
			free(temp);
		}
		
		if(!flag){
			puts("Case 3");
			curr->isUsed = false;
		}
	}
	return pool;
    */
    struct mem_std_free_block* freed = (void*)((char*)addr - 8);
    set_block_free(&freed->header);
    struct mem_std_free_block* curr = pool->first_free;
    if(freed < curr){
        freed->prev = NULL; 
        freed->next = pool->first_free;
        curr->prev = freed;
        pool->first_free = freed;
    }
    // else{
    //     while(curr != NULL){
    //         if(curr->next != NULL && curr->next < freed){
    //             curr = curr->next;
    //         }
    //         else{
    //             freed->next = curr->next;
    //             freed->prev = curr;
    //             if(curr->next != NULL)
    //                 curr->next->prev = freed;
    //             curr->next = freed;
    //             break;
    //         }
    //     }
    // }
    
}

size_t mem_get_allocated_block_size_standard_pool(mem_pool_t *pool, void *addr)
{
    /* TODO Get Allocated Block Size IMPLEMENTED */
    printf("%s:%d: Please, implement me!\n", __FUNCTION__, __LINE__);
    struct mem_std_allocated_block* allocated = (void*)((char*)addr -8);
    return get_block_size(&allocated->header);
}
