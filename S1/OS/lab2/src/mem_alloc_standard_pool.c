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

/* Function: printStatus
----------------------------------------------------------------
Print Memory Status (Debugging Purpose only)

    pool - address of the pool
*/
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

/* Function: set_free_size
----------------------------------------------------------------
Set the block into Free and also with the specific size

    address - address of the block
    size - size of the block
*/
void set_free_size(struct mem_std_free_block *address, size_t size){
    set_block_free(&address->header);
    set_block_size(&address->header, size);
}

/* Function: set_allocated_size
----------------------------------------------------------------
Set the block into Allocated and also with the specific size

    address - address of the block
    size - size of the block
*/
void set_allocated_size(struct mem_std_allocated_block *address, size_t size){
    set_block_used(&address->header);
    set_block_size(&address->header, size);
}

/* Function: position_compare
----------------------------------------------------------------
Compare address of 2 passed free block address

    curr - address of the first free block
    temp - address of the second free block
*/
bool position_compare(struct mem_std_free_block *curr, struct mem_std_free_block *temp){
    return (void *)curr > (void *)temp;
}

/* Function: sort_block
----------------------------------------------------------------
Sort the block in the linked list of the free block

    pool - address of the pool
*/
void sort_block(mem_pool_t *pool){
    struct mem_std_free_block *curr = pool->first_free;
    struct mem_std_free_block *temp = curr->next;
    bool compare = position_compare(curr, temp);
     
    bool isFirst = true;
    while(temp != NULL && compare){
        curr->next = temp->next;
        temp->prev = curr->prev;
        curr->prev = temp;
        temp->next = curr;
        if(isFirst){
            pool->first_free = (void *) temp;
            isFirst = false;
        }
        temp = curr->next;
        if(temp != NULL)
            temp->prev = curr;

        compare = position_compare(curr, temp);
    };
}

/* Function: get_full_size_of_block
----------------------------------------------------------------
Get the full size of the block

    addr - address of the block
*/
int get_full_size_of_block(void *addr){
    return get_block_size(&((struct mem_std_allocated_block*)addr)->header) + (SIZE_OF_POINTER*2);
}


/* Function: merge_block
----------------------------------------------------------------
Merge the newest free block with adjacent free blocks

    freed - address of the newest free block
*/
void merge_block(struct mem_std_free_block *freed){
    //Merge freed block with the next free block if it was next to it
    if(freed->next == (void *) freed + get_full_size_of_block(freed)){
        struct mem_std_free_block* temp = freed->next;
        freed->next = temp->next;
        if(temp->next != NULL)
            temp->next->prev = freed;
        int total_size = get_block_size(&freed->header)+ get_full_size_of_block(temp);
        
        set_block_used(&temp->header);
        set_free_size(freed, total_size);
    }

    //Merge freed block with the prev free block if it was next to it
    struct mem_std_free_block *curr = freed->prev;
    if(curr != NULL && freed == (void *) curr + get_full_size_of_block(curr)){
        curr->next = freed->next;
        if(freed->next != NULL)
            freed->next->prev = curr;
        int total_size = get_block_size(&curr->header)+ get_full_size_of_block(freed);
        
        set_block_used(&freed->header);
        set_free_size(curr, total_size);
    }
}

/* Function: init_standard_pool
----------------------------------------------------------------
Initializes a standard pool with the given parameters

    p - address of the pool
    size: size of the pool
    min_request_size: minimum size of the request to the block
    max_request_size: maximum size of the request to the block
*/
void init_standard_pool(mem_pool_t *p, size_t size, size_t min_request_size, size_t max_request_size)
{
    void* temp = my_mmap(size);
    struct mem_std_free_block *free_mem = temp;
    p->start = p->first_free = free_mem;
    p->end = (char*) temp + size-(SIZE_OF_POINTER*4); 
    free_mem->next = NULL;
    free_mem->prev = NULL;
    set_free_size(free_mem, size - (SIZE_OF_POINTER*2));
    START_ADDRESS = (void*) p->start;
}

/* Function: useableMemory
----------------------------------------------------------------
Return values if block is useable with that specific size.

    curr - address to the free block
    size - size that needed

*/
bool useableMemory(struct mem_std_free_block *curr, int size){
	return curr != NULL && get_block_size(&curr->header) >= size && is_block_free(&curr->header);
}

/* Function: mem_alloc_standard_pool
----------------------------------------------------------------
Allocate a block with the given size. And the allocated block will be split 
if the remaining size is greater or equal to 32 and 
the splitted / the free block will added to the linked list of free blocks.

    pool - address to the pool
    size - size to the block

*/
void *mem_alloc_standard_pool(mem_pool_t *pool, size_t size)
{
    struct mem_std_free_block* curr = pool->first_free;
    //For First Fit Policy
    if(STDPOOL_POLICY == 1){
        while(!useableMemory(curr, size)){
            curr = curr->next;
        }
    }
    //For Best Fit Policy
    else if(STDPOOL_POLICY == 2){
        int t_size = INT_MAX;
        struct mem_std_free_block* temp = curr;
        while(temp != NULL){
            int temp_size = get_block_size(&temp->header);
            if(useableMemory(temp, size) &&  temp_size < t_size){
                curr = temp;
                t_size = temp_size;
            }
            temp = temp->next;
        }
    }
    if(curr != NULL){
        int remaining_size = get_block_size(&curr->header) - (size + (SIZE_OF_POINTER*2));
        struct mem_std_allocated_block* allocated;
        //If The block size is smaller than 32 we keep the block
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
        //We Split the block into Allocated and Free block
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
        return (char*)allocated + SIZE_OF_POINTER;
    }
    return NULL;
}

/* Function: mem_free_standard_pool
----------------------------------------------------------------
Frees a block with the given adress.
And the freed block will be inserted to the linked list of free blocks.

    pool - address to the pool
    addr - address to the block

*/
bool mem_free_standard_pool(mem_pool_t *pool, void *addr)
{
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
    return true;
}

size_t mem_get_allocated_block_size_standard_pool(mem_pool_t *pool, void *addr)
{
    struct mem_std_allocated_block* allocated = (void*)((char*)addr -SIZE_OF_POINTER);
    return get_block_size(&allocated->header);
}
