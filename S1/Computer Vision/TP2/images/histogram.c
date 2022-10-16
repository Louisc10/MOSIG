#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "Util.h"

int rows, cols;
gray *graymap;
int *filter;
int divider;
int *histogram;
int *Fi;
int max = __INT_MAX__ * -1;
int min = __INT_MAX__;

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

void clear_histogram(int maxval){
  for(int i = 0; i <= maxval; i++){
    histogram[i] = 0;
  }
}

void create_histogram(int col, int row, int maxval){
  clear_histogram(maxval);
  int i,j;

  for(i=0; i < rows; i++){
    for(j=0; j < cols ; j++){
      histogram[graymap[i * cols + j]] ++;
    }
  }
}

void getMaxMin(int col, int row){
  max = __INT_MAX__ * -1;
  min = __INT_MAX__;
  int i,j;

  for(i=0; i < rows; i++){
    for(j=0; j < cols ; j++){
      int x = graymap[i * cols + j];
      if(max < x)
        max = x;
      if(min > x)
        min = x;
    }
  }
}

void histogram_stretching(int col, int row, int maxval){
  create_histogram(col, row, maxval);
  getMaxMin(col, row);
  int i, j;

  for(i=0; i < rows; i++){
    for(j=0; j < cols ; j++){
      graymap[i * cols + j] = maxval * (graymap[i * cols + j] - min) / (max-min);
    }
  }
}

void make_Fi(int maxval){
  Fi = (int *) malloc(sizeof(int) * (maxval + 2));
  int temp = 0;
  for(int i = 0; i <= maxval; i++){
    temp += histogram[i];
    Fi[i] = temp;
  }
}

void histogram_equalization(int col, int row, int maxval){
  create_histogram(col, row, maxval);
  make_Fi(maxval);

  int total_pixel = row * col;
  int i, j;

  for(i=0; i < rows; i++){
    for(j=0; j < cols ; j++){
      graymap[i * cols + j] = maxval * Fi[graymap[i * cols + j]]/total_pixel;
    }
  }
}

/*
Create Pascal triangle
Courtesy: https://www.faceprep.in/c/program-to-print-pascals-triangle/
*/
void createFilter(int filter_size){
  int coef, i, j, temp_pascal[filter_size+2];
  divider = 0;
  for(i = 0; i<= filter_size; i++){
		if(i==0)
			coef = 1;
		else
			coef = coef * (filter_size-i)/i;
		
		temp_pascal[i] = coef;
	}

  filter = malloc(sizeof(int) * filter_size * filter_size);
  for(i = 0; i < filter_size; i++){
    for(j = 0; j < filter_size; j++){
      int x = temp_pascal[i] * temp_pascal[j];
      filter[i*filter_size+j] = x;
      divider += x;
    }
  }
}


int get_new_byte(int col, int row, int filter_size){
  int temp = 0;
  int start_col = col - (filter_size/2);
  int start_row = row - (filter_size/2);
  for(int i = 0; i < filter_size; i++){
    for(int j = 0; j < filter_size; j++){
      temp += get_byte_by_pos(start_col + i, start_row + j) * filter[i *filter_size + j];
    }
  }

  return temp / divider;
}

void repeat_run(int total_runs, int filter_size){
  gray temp_graymap[cols * rows];
  int k;
  int i, j;
  for(k = 0; k < total_runs; k++){
    for(i=0; i < rows; i++){
      for(j=0; j < cols ; j++){
        temp_graymap[i * cols + j] = get_new_byte(j,i, filter_size);
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
  int ich1, ich2, maxval=255, pgmraw, filter_size, total_runs;
  int i, j;
  char *mode;

  /*
    Argument Handler
  */
  if ( argc != 5 ){
    printf("\nUsage: %s file filter_size repetition mode\n\n", argv[0]);
    exit(0);
  }

  ifp = fopen(argv[1],"r");
  if (ifp == NULL) {
    printf("error in opening file %s\n", argv[1]);
    exit(1);
  }

  filter_size = atoi(argv[2]);
  if(filter_size < 0 || filter_size %2 != 1){
    printf("error in size of filter %s\n", argv[2]);
    printf("expecting %s is greater than 0 and an odd number\n", argv[3]);
    exit(1);
  }

  total_runs = atoi(argv[3]);
  if(total_runs < 0){
    printf("Filter need to run at least once\n");
    exit(1);
  }

  mode = argv[4];
  if(strcmp(mode, "e") != 0 && strcmp(mode, "s") != 0){
    printf("Filter mode must be s[Stretching] or e[Equalization]\n");
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
  histogram = (int *) malloc((maxval+1) * sizeof(int));

  for(i=0; i < rows; i++)
    for(j=0; j < cols ; j++)
      if(pgmraw)
        graymap[i * cols + j] = pm_getrawbyte(ifp) ;
      else
        graymap[i * cols + j] = pm_getint(ifp);

  /*
    Processing
  */
  createFilter(filter_size);
  repeat_run(total_runs, filter_size);

  if(strcmp(mode, "s") == 0){
    histogram_stretching(cols, rows, maxval);
  }
  else if(strcmp(mode, "e") == 0){
    histogram_equalization(cols, rows, maxval);
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

  fclose(ifp);
  return 0;
}
