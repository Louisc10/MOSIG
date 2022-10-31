#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define LOOPS 20

#define BUF_SIZE 4

int index;
int buffer[BUF_SIZE];
sem_t full_count, empty_count, mutex;

void pushing(int val)
{
    /* TO DO */

    sem_wait(&empty_count);
    sem_wait(&mutex);
    /* NOTE: pay attention to where to put the call to printf to be
       sure it can be helpful for debugging */
    printf("pushing value %d\n",val);
    buffer[index] = val;
    index = index + 1;
    sem_post(&mutex);
    sem_post(&full_count);
}

int fetching(void)
{
    int val=0;

    /* TO DO */

    sem_wait(&full_count);
    sem_wait(&mutex);
    /* NOTE: pay attention to where to put the call to printf to be
       sure it can be helpful for debugging */
    index = index - 1;
    val = buffer[index];
    printf("\t feched value %d\n",val);
    sem_post(&mutex);
    sem_post(&empty_count);

    return val;
}

void* thread_generating(void* arg)
{
    int i = 0;
    int new_value = 0;
    
    for(i=0; i<LOOPS; i++){
        new_value = rand()%10;
        pushing(new_value);
    }

    return NULL;
}


void* thread_using(void* arg)
{
    int i = 0;
    int value = 0;
    
    for(i=0; i<LOOPS; i++){
        value = fetching();
    }

    return NULL;
}



int main(void)
{
    pthread_t tids[2];
    int i=0;
    
    struct timespec tt;
    clock_gettime(CLOCK_MONOTONIC, &tt);
    /* seed for the random number generator */
    srand(tt.tv_sec);

    sem_init(&empty_count, 0, BUF_SIZE);
    sem_init(&full_count, 0, 0);
    sem_init(&mutex, 0, 1);
    index = 0;
    
    if(pthread_create (&tids[0], NULL, thread_using, NULL) != 0){
        fprintf(stderr,"Failed to create the using thread\n");
        return EXIT_FAILURE;
    }
    
    if(pthread_create (&tids[1], NULL, thread_generating, NULL) != 0){
        fprintf(stderr,"Failed to create the generating thread\n");
        return EXIT_FAILURE;
    }
    
    /* Wait until every thread ended */ 
    for (i = 0; i < 2; i++){
        pthread_join (tids[i], NULL) ;
    }  
    
    return EXIT_SUCCESS;
}
