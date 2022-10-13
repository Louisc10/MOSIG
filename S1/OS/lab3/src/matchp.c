#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

struct song{
    char *str;
    int nb_song;
};

void *supporter (void *arg){
  char      *str = ((struct song *) arg)->str ;
  int       i ;
  int       pid ;
  pthread_t tid ;  
  pid = getpid () ;
  tid = pthread_self () ;

  for (i = 0; i < ((struct song *) arg)->nb_song; i++){
      printf ("Process %d Thread %x : %s \n", pid, (unsigned int) tid, str) ;
  }
  return (void *) tid ;
}

int main (int argc, char **argv){

  int team1, nb_song_1 ;
  int team2, nb_song_2 ;
  int i ;
  int nb_threads = 0 ;
  pthread_t *tids ;

  if (argc != 5){
    fprintf(stderr, "usage : %s team1 nb_song_team1 team2 nb_song_team2\n", argv[0]) ;
    exit (-1) ;
  }

  team1 = atoi (argv[1]) ;
  nb_song_1 = atoi (argv[2]) ;
  team2  = atoi (argv[3]) ;
  nb_song_2  = atoi (argv[4]) ;
  nb_threads = team1 + team2; 
  tids = malloc (nb_threads*sizeof(pthread_t)) ;

  struct song* song_t1 = (struct song*) malloc(sizeof(struct song));
  song_t1->str = "Allons enfants de la patrie";
  song_t1->nb_song = nb_song_1;

  struct song* song_t2 = (struct song*) malloc(sizeof(struct song));
  song_t2->str = "Swing low, sweet chariot";
  song_t2->nb_song = nb_song_2;

  /* Create the threads for team1 */
  for (i = 0 ; i < team1; i++){
    pthread_create (&tids[i], NULL, supporter, song_t1) ;
  }
  /* Create the other threads (ie. team2) */
  for ( ; i < nb_threads; i++){
    pthread_create (&tids[i], NULL, supporter, song_t2) ;
  }  

  /* Wait until every thread ended */ 
  for (i = 0; i < nb_threads; i++){
    pthread_join (tids[i], NULL) ;
  }
  
  free (tids) ;
  return EXIT_SUCCESS;
}
