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

int get_new_byte(int col, int row){
  int temp = 0;
  temp += get_byte_by_pos(col-2,row-2) * 1;
  temp += get_byte_by_pos(col-2,row-1) * 4;
  temp += get_byte_by_pos(col-2,row) * 6;
  temp += get_byte_by_pos(col-2,row+1) * 4;
  temp += get_byte_by_pos(col-2,row+2) * 1;

  temp += get_byte_by_pos(col-1,row-2) * 4;
  temp += get_byte_by_pos(col-1,row-1) * 16;
  temp += get_byte_by_pos(col-1,row) * 24;
  temp += get_byte_by_pos(col-1,row+1) * 16;
  temp += get_byte_by_pos(col-1,row+2) * 4;

  temp += get_byte_by_pos(col,row-2) * 6;
  temp += get_byte_by_pos(col,row-1) * 24;
  temp += get_byte_by_pos(col,row) * 36;
  temp += get_byte_by_pos(col,row+1) * 24;
  temp += get_byte_by_pos(col,row+2) * 6;

  temp += get_byte_by_pos(col+1,row-2) * 4;
  temp += get_byte_by_pos(col+1,row-1) * 16;
  temp += get_byte_by_pos(col+1,row) * 24;
  temp += get_byte_by_pos(col+1,row+1) * 16;
  temp += get_byte_by_pos(col+1,row+2) * 4;

  temp += get_byte_by_pos(col+2,row-2) * 1;
  temp += get_byte_by_pos(col+2,row-1) * 4;
  temp += get_byte_by_pos(col+2,row) * 6;
  temp += get_byte_by_pos(col+2,row+1) * 4;
  temp += get_byte_by_pos(col+2,row+2) * 1;

  return temp / 256;
}

void repeat_run(int total_runs){
  gray temp_graymap[cols * rows];
  int k;
  int i, j;
  for(k = 0; k < total_runs; k++){
    for(i=0; i < rows; i++){
      for(j=0; j < cols ; j++){
        temp_graymap[i * cols + j] = get_new_byte(j,i);
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
  int ich1, ich2, maxval=255, pgmraw, total_runs;
  int i, j;

  /*
    Argument Handler
  */
  if ( argc != 3 ){
    printf("\nUsage: %s file repetition\n\n", argv[0]);
    exit(0);
  }

  ifp = fopen(argv[1],"r");
  if (ifp == NULL) {
    printf("error in opening file %s\n", argv[1]);
    exit(1);
  }

  total_runs = atoi(argv[2]);
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
  repeat_run(total_runs);

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
