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
const int INT_MAX = 2147483647;
long int START_ADDRESS;

void printStatus(mem_pool_t *pool){
    void* curr = pool->start;
    while(curr < pool->end){
        int size = get_block_size(&((struct mem_std_free_block*)curr)->header);
        bool isUsed = is_block_used(&((struct mem_std_free_block*)curr)->header);
        printf("Address : %ld\n", (void*) curr - pool->start + 8);
        printf("Size    : %d\n", size);
        printf("Status  : %s\n", isUsed? "Used" : "Free");
        if(!isUsed){
            printf("Prev    : %ld\n", ((struct mem_std_free_block*)curr)->prev != NULL? ((void *) ((struct mem_std_free_block*)curr)->prev) - pool->start + 8: -1);
            printf("Next    : %ld\n", ((struct mem_std_free_block*)curr)->next != NULL? ((void *) ((struct mem_std_free_block*)curr)->next) - pool->start + 8: -1);
        }
        curr += size + (SIZE_OF_POINTER *2);
        puts("");
    }
    puts("====================");
}

void set_free_size(struct mem_std_free_block *address, size_t size){
    set_block_free(&address->header);
    set_block_size(&address->header, size);
}

void set_allocated_size(struct mem_std_allocated_block *address, size_t size){
    set_block_used(&address->header);
    set_block_size(&address->header, size);
}

bool position_compare(struct mem_std_free_block *curr, struct mem_std_free_block *temp){
    return (void *)curr > (void *)temp;
}

void sort_block(mem_pool_t *pool){
    struct mem_std_free_block *curr = pool->first_free;
    struct mem_std_free_block *temp = curr->next;
    bool compare = position_compare(curr, temp);
     
    bool isFirst = true;
    //printf(">> Curr %p\nTemp %p\nResult %s\n\n", (void *)curr ,(void *)temp, compare? "true": "false");
    while(temp != NULL && compare){
        //printf(">! Curr %p\nTemp %p\nResult %s\n", (void *)curr ,(void *)temp, compare? "true": "false");
        //printf(" > Curr Next %p\nCurr->Prev %p\n", (void *)curr->next ,(void *)curr->prev);
        //printf(" > Temp Next %p\nTemp->Prev %p\n", (void *)temp->next ,(void *)temp->prev);
        curr->next = temp->next;
        temp->prev = curr->prev;
        curr->prev = temp;
        temp->next = curr;
        if(isFirst){
            pool->first_free = (void *) temp;
            isFirst = false;
        }

        //printf(" > Curr Next %p\nCurr->Prev %p\n", (void *)curr->next ,(void *)curr->prev);
        //printf(" > Temp Next %p\nTemp->Prev %p\n", (void *)temp->next ,(void *)temp->prev);
        temp = curr->next;
        if(temp != NULL)
            temp->prev = curr;
        //printf("Curr %p\nTemp %p\n\n\n", (void *)curr ,(void *)temp);

        compare = position_compare(curr, temp);
    };
}

int get_full_size_of_block(void *addr){
    return get_block_size(&((struct mem_std_allocated_block*)addr)->header) + (SIZE_OF_POINTER*2);
}

void merge_block(struct mem_std_free_block *freed){
    if(freed->next == (void *) freed + get_full_size_of_block(freed)){
        struct mem_std_free_block* temp = freed->next;
        freed->next = temp->next;
        if(temp->next != NULL)
            temp->next->prev = freed;

        int total_size = get_block_size(&freed->header)+ get_full_size_of_block(temp);
        
        set_block_used(&temp->header);
        set_free_size(freed, total_size);
    }

    struct mem_std_free_block *curr = freed->prev;
    if(curr != NULL && freed == (void *) curr + get_full_size_of_block(curr)){
        curr->next = freed->next;
        if(freed->next != NULL)
            freed->next->prev = curr;
        int total_size = get_block_size(&curr->header)+ get_full_size_of_block(freed);
        
        set_block_used(&freed->header);
        // set_block_used(&freed->footer);

        set_free_size(curr, total_size);
    }
}

void init_standard_pool(mem_pool_t *p, size_t size, size_t min_request_size, size_t max_request_size)
{
    /* TODO Init Standard Pool IMPLEMENTED */
    //printf("%s:%d: Please, implement me!\n", __FUNCTION__, __LINE__);
    void* temp = my_mmap(size);
    struct mem_std_free_block *free_mem = temp;
    p->start = p->first_free = free_mem;
    p->end = (char*) temp + size-(SIZE_OF_POINTER*4); 
    free_mem->next = NULL;
    free_mem->prev = NULL;
    set_free_size(free_mem, size - (SIZE_OF_POINTER*2));
    START_ADDRESS = (void*) p->start;

    // printStatus(p);
}

bool useableMemory(struct mem_std_free_block *curr, int size){
    //printf(">>%ld (%ld) need %d\n",(void *)curr - START_ADDRESS, get_block_size(&curr->header), size);

	return curr != NULL && get_block_size(&curr->header) >= size && is_block_free(&curr->header);
}

void *mem_alloc_standard_pool(mem_pool_t *pool, size_t size)
{
    /* TODO Alloc Standard Pool IMPLEMENTED */
    //printf("%s:%d: Please, implement me!\n", __FUNCTION__, __LINE__);
    //printf("<%d>\n", STDPOOL_POLICY);

    struct mem_std_free_block* curr = pool->first_free;

    if(STDPOOL_POLICY == 1){
        //puts("========== Using First Fit ===================");
        while(!useableMemory(curr, size)){
            //printf(">>%p (%ld)\n", curr, get_block_size(&curr->header));
            curr = curr->next;
        }
    }
    else if(STDPOOL_POLICY == 2){
        //puts("========== Using Best Fit ===================");
        int t_size = INT_MAX;
        struct mem_std_free_block* temp = curr;
        while(temp != NULL){
            int temp_size = get_block_size(&temp->header);
            if(useableMemory(temp, size) &&  temp_size < t_size){
                //printf("Take the Memory, %d\n",temp_size);
                curr = temp;
                t_size = temp_size;
            }
            temp = temp->next;
        }
    }
    if(curr != NULL){
        int remaining_size = get_block_size(&curr->header) - (size + (SIZE_OF_POINTER*2));
        struct mem_std_allocated_block* allocated;
        
        if(remaining_size < (SIZE_OF_POINTER*4)){
            struct mem_std_free_block *temp = curr;
            if(temp->prev != NULL)
                temp->prev->next = temp->next;
            if(temp->next != NULL)
                temp->next->prev = temp->prev;

            
            if(curr == pool->first_free)
                pool->first_free = curr->next;
            allocated = curr;
            set_allocated_size(allocated, size);
        }
        else{
            allocated = curr;
            set_allocated_size(allocated, size);
            struct mem_std_free_block* new_free = (void *)((char*)allocated + (size+ (SIZE_OF_POINTER*2)));
            new_free->next = curr->next;
            new_free->prev = curr->prev;
            if(curr->next != NULL)
                curr->next->prev = new_free;
            if(curr->prev != NULL)
                curr->prev->next = new_free;

            if(curr == pool->first_free)
                pool->first_free = new_free;
            
            set_free_size(new_free, remaining_size);
        }

        // printStatus(pool);
        return (char*)allocated + SIZE_OF_POINTER;
    }
    // printStatus(pool);
    return NULL;
}

bool mem_free_standard_pool(mem_pool_t *pool, void *addr)
{
    /* TODO Free Standard Pool IMPLEMENTED */
    //printf("%s:%d: Please, implement me!\n", __FUNCTION__, __LINE__);

    struct mem_std_free_block* freed = (void*)((char*)addr - SIZE_OF_POINTER);
    if(is_block_free(&freed->header))
        return false;
    set_free_size(freed, get_block_size(&freed->header));

    struct mem_std_free_block* curr = pool->first_free;
    freed->prev = NULL; 
    freed->next = curr;
    curr->prev = freed;
    pool->first_free = (void*) freed;

    sort_block(pool);
    merge_block(freed);
    // printStatus(pool);
    return true;
}

size_t mem_get_allocated_block_size_standard_pool(mem_pool_t *pool, void *addr)
{
    /* TODO Get Allocated Block Size IMPLEMENTED */
    //printf("%s:%d: Please, implement me!\n", __FUNCTION__, __LINE__);
    struct mem_std_allocated_block* allocated = (void*)((char*)addr -SIZE_OF_POINTER);
    return get_block_size(&allocated->header);
}
