#include <stdlib.h>
#include <stdio.h>
#include "Util.h"

int rows, cols;
gray *graymap;

int get_byte_by_pos(int col, int row){
  if(row < 0)
    row = 0;
  if(col < 0)
    col = 0;
  if(col >= cols)
    col = cols-1;
  if(row >= rows)
    row = rows-1;
  
  return graymap[row * cols + col];
}

int get_new_byte(int col, int row, int size_filter){
  int temp[size_filter*size_filter];
  int i, j;
  for(i = -(size_filter/2); i <= (size_filter/2); i++){
    for(j= -(size_filter/2); j <= (size_filter/2); j++){
      temp[i * size_filter + j] = get_byte_by_pos(col-j, row-i);
    }
  }

  for(i = 1; i < size_filter*size_filter; i++){
    for(j=0; j < i ; j++){
      if(temp[j] > temp[j+1]){
        int x = temp[j];
        temp[j] = temp[j+1];
        temp[j+1] = x;
      }
    }
  }

  return temp[size_filter*size_filter/2];
}

void repeat_run(int total_runs, int size_filter){
  gray temp_graymap[cols * rows];
  int k;
  int i, j;
  for(k = 0; k < total_runs; k++){
    for(i=0; i < rows; i++){
      for(j=0; j < cols ; j++){
        temp_graymap[i * cols + j] = get_new_byte(j,i, size_filter);
      }
    }
    for(i=0; i < rows; i++){
      for(j=0; j < cols ; j++){
        graymap[i * cols + j] = temp_graymap[i * cols + j];
      }
    }
  }
}

int main(int argc, char* argv[]){
  FILE* ifp;
  int ich1, ich2, maxval=255, pgmraw, total_runs, size_filter;
  int i, j;

  /*
    Argument Handler
  */
  if ( argc != 4 ){
    printf("\nUsage: %s file \n\n", argv[0]);
    exit(0);
  }

  ifp = fopen(argv[1],"r");
  if (ifp == NULL) {
    printf("error in opening file %s\n", argv[1]);
    exit(1);
  }

  size_filter = atoi(argv[2]);
  if(size_filter % 2 == 0 || size_filter <= 0){
    printf("error in size of filter %s\n", argv[2]);
    printf("expecting %s is greater than 0 and an odd number\n", argv[3]);
    exit(1);
  }

  total_runs = atoi(argv[3]);
  if(total_runs < 0){
    printf("Filter need to run at least once\n");
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

  for(i=0; i < rows; i++)
    for(j=0; j < cols ; j++)
      if(pgmraw)
        graymap[i * cols + j] = pm_getrawbyte(ifp) ;
      else
        graymap[i * cols + j] = pm_getint(ifp);

  /*
    Processing
  */
  repeat_run(total_runs, size_filter);
  
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

  fclose(ifp);
  return 0;
}
