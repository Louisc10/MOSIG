#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void *temp (void *arg){
    char* id = (char*) arg ;
    printf("ID: %s\n", id);
    sleep (1);
    return (void*)0;
}

int main(){
    int i ;
    int nb_threads = 10 ;
    pthread_t *tids;
    char text[nb_threads][5];
    tids = malloc (nb_threads*sizeof(pthread_t)) ;

    for(i = 0; i < nb_threads; i++){
        sprintf(text[i],"%d",i);
    }

    pthread_create(&tids[0], NULL, temp, text[0]);

    pthread_join (tids[0], NULL) ;
    pthread_create(&tids[1], NULL, temp, text[1]);
    pthread_create(&tids[2], NULL, temp, text[2]);

    pthread_join (tids[2], NULL) ;
    pthread_create(&tids[3], NULL, temp, text[3]);
    pthread_create(&tids[4], NULL, temp, text[4]);
    pthread_create(&tids[5], NULL, temp, text[5]);

    pthread_join (tids[5], NULL) ;
    pthread_create(&tids[8], NULL, temp, text[8]);

    pthread_join (tids[4], NULL) ;
    pthread_join (tids[8], NULL) ;
    pthread_create(&tids[7], NULL, temp, text[7]);

    pthread_join (tids[1], NULL) ;
    pthread_join (tids[3], NULL) ;
    pthread_join (tids[7], NULL) ;
    pthread_create(&tids[6], NULL, temp, text[6]);

    pthread_join (tids[6], NULL) ;
    pthread_create(&tids[9], NULL, temp, text[9]);

    pthread_join (tids[9], NULL) ;

    return 0;
}