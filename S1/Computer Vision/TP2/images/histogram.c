#include <stdlib.h>
#include <stdio.h>
#include "Util.h"

int rows, cols;
gray *graymap;

int main(int argc, char* argv[]){
  FILE* ifp;
  int ich1, ich2, maxval=255, pgmraw;
  int i, j;

  if ( argc != 2 ){
    printf("\nUsage: %s file \n\n", argv[0]);
    exit(0);
  }

  ifp = fopen(argv[1],"r");
  if (ifp == NULL) {
    printf("error in opening file %s\n", argv[1]);
    exit(1);
  }

  ich1 = getc( ifp );
  if ( ich1 == EOF )
    pm_erreur( "EOF / read error / magic number" );
  ich2 = getc( ifp );
  if ( ich2 == EOF )
    pm_erreur( "EOF /read error / magic number" );
  if(ich2 != '2' && ich2 != '5')
    pm_erreur(" wrong file type ");
  else
    if(ich2 == '2')
      pgmraw = 0;
    else 
      pgmraw = 1;

  cols = pm_getint( ifp );
  rows = pm_getint( ifp );
  maxval = pm_getint( ifp );

  graymap = (gray *) malloc(cols * rows * sizeof(gray));
  int histogram[maxval]; 

  for(i = 0; i < maxval; i++){
    histogram[i] = 0;
  }

  for(i=0; i < rows; i++)
    for(j=0; j < cols ; j++)
      if(pgmraw){
        graymap[i * cols + j] = pm_getrawbyte(ifp) ;
        int temp = graymap[i * cols + j];
        histogram[temp] ++;
      }
      else{
        graymap[i * cols + j] = pm_getint(ifp);
        histogram[graymap[i*cols*j]] ++;
      }

  if(pgmraw)
    printf("P2\n");
  else
    printf("P5\n");

  printf("%d %d \n", cols, rows);
  printf("%d\n",maxval);

  for(i=0; i < rows; i++){
    for(j=0; j < cols ; j++){
      if(pgmraw){;
        printf("%d ", graymap[i * cols + j]);
      }
      else{
        printf("%c", graymap[i * cols + j]);
      }
    }
  }

  printf("\n============\n");

  for(i=0; i < maxval; i++){
    printf("%d ", histogram[i]);
  }

  fclose(ifp);
  return 0;
}
