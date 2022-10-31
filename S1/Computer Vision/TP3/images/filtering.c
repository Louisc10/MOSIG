#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include "Util.h"

int rows, cols;
gray *graymap;
int *filter;
int divider;

/* 
  Get_byte_pos
  it will return the value of the pixel based on the position
*/ 
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

/* 
  Create Filter
  it will create the pascal tree filter and define the value of divider

  Create Pascal triangle
  Courtesy: https://www.faceprep.in/c/program-to-print-pascals-triangle/
*/
void create_filter(int filter_size){
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

/* 
  Get_new_byte
  it will return the value of new pixel based on the filter automatically.
*/
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

/* 
  Repeat_gaussian
  It will apply the Gaussian blur with “total_run” repetition and size from “filter_size”
*/
void repeat_gaussian(int total_runs, int filter_size){
  create_filter(filter_size);
  gray temp_graymap[cols * rows];
  int k;
  int i, j;
  for(k = 0; k < total_runs; k++){
    for(i=0; i < rows; i++){
      for(j=0; j < cols ; j++){
        temp_graymap[i * cols + j] = get_new_byte(j,i, filter_size);
      }
    }

    for(i=0; i < rows * cols; i++){
      graymap[i] = temp_graymap[i];
    }
  }
}

int gradient;
double *theta;

/* 
  get_scharr_filter
  It produces the theta and Gradient for the coordinate that passed in the arguments
*/
void get_scharr_value(int col, int row){
  int filter_size = 3;
  int Gx[] = {3, 0, -3, 10, 0, -10, 3, 0, -3};
  int Gy[] = {3, 10, 3, 0, 0, 0, -3, -10, -3};

  int total_x = 0;
  int total_y = 0;
  int start_col = col - (filter_size/2);
  int start_row = row - (filter_size/2);
  
  for(int i = 0; i < filter_size; i++){
    for(int j = 0; j < filter_size; j++){
      total_x += get_byte_by_pos(start_col + i, start_row + j) * Gx[i *filter_size + j];
      total_y += get_byte_by_pos(start_col + i, start_row + j) * Gy[i *filter_size + j];
    }
  }

  gradient = sqrt(total_x * total_x + total_y * total_y);
  theta[row * cols + cols] = atan((double)(total_y* 1.0f / total_x )) * 180/ M_PI;
}

/* 
  Scharr_filter
  Apply the scharr filter to the image
*/
void scharr_filter(int threshold){
  int i, j;
  int max = __INT_MAX__ *-1;
  int min = __INT_MAX__;
  int temp[cols * rows];
  theta = (double *) malloc(sizeof(double) * cols * rows);

  for(i=0; i < rows; i++){
    for(j=0; j < cols ; j++){
      get_scharr_value(j,i);
      temp[i*cols + j] = gradient; 
      if(gradient > max)
        max = gradient;
      if(gradient < min)
        min = gradient;
    }
  }

  for(i=0; i < rows * cols ; i++){
    int x = (temp[i]-min) * 255 /max;
    graymap[i] = x;
    /*
      For getting the image by using threshold
    */
    // graymap[i] = (x <= threshold) ? 0 : x;
  }
}

/*
  Non Maxima Suppression 
  is for reducing the size of the edge
*/
void non_maxima_suppression(){
  int temp[cols * rows];
  int i, j;

  for(i=0; i < rows * cols; i++){
    temp[i] = graymap[i];
  }

  for(i=1; i < rows-1; i++){
    for(j=1; j < cols-1; j++){
      int my_pos = temp[i * cols + j];
      double t = theta[i * cols + j];
      if(t < 0)
        t += 180;
      
      int a = 0;
      int b = 0;

      if((157.5 <= t && t <= 180) || (0 <= t && t < 22.5)){
        a = temp[i * cols + (j+1)];
        b = temp[i * cols + (j-1)];
      }
      if(22.5 <= t && t < 67.5){
        a = temp[(i+1) * cols + (j-1)];
        b = temp[(i-1) * cols + (j+1)];
      }
      if(67.5 <= t && t < 112.5){
        a = temp[(i+1) * cols + j];
        b = temp[(i-1) * cols + j];
      }
      if(112.5 <= t && t < 157.5){
        a = temp[(i-1) * cols + (j-1)];
        b = temp[(i+1) * cols + (j+1)];
      }

      if (my_pos < a || my_pos < b)
        graymap[i * cols + j] = 0;
    }
  }
}

/* 
  Define the value of edge, undecided, and not_edge
*/
int edge = 255; //white
int undecided = 100; //white gray
int not_edge = 0;//black: not relate to the edge

/* 
  Double Thresholding
  Classify into 3 categories: edge, undecided, and not_edge
*/
void double_thresholding(){
  int i;
  int size = cols*rows;

  // Find max value of the image
  int max = -1 * __INT_MAX__;
  for(i=0; i < size; i++){
    if(max < graymap[i])
      max = graymap[i];
  }

  int high_threshold = max * 0.3;
  int low_threshold = max * 0.1;

  for(i=0; i<size;i++){
    if(graymap[i] >= high_threshold)
      graymap[i] = edge;
    else if(graymap[i] >= low_threshold)
      graymap[i] = undecided;
    else
      graymap[i] = not_edge;
  };
};

/* 
  is_neighbor_an_edge
  it return true if one of the neighbors is an edge
*/
bool is_neighbor_an_edge(int col, int row){
  return 
    get_byte_by_pos(col - 1, row -1) == edge || get_byte_by_pos(col - 1, row) == edge || get_byte_by_pos(col - 1, row + 1) == edge || 
    get_byte_by_pos(col, row -1) == edge     ||                                          get_byte_by_pos(col, row + 1) == edge || 
    get_byte_by_pos(col + 1, row -1) == edge || get_byte_by_pos(col + 1, row) == edge || get_byte_by_pos(col + 1, row + 1) == edge;
}

/* 
  hysteresis
  Change undecided value into an edge if one of the neighbors is an edge
*/
void hysteresis(){
  int i,j;

  for(i=0; i < rows; i++){
    for(j=0; j < cols; j++){
      if (graymap[i * cols + j] == undecided){
        if (is_neighbor_an_edge(j, i))
          graymap[i * cols + j] = edge;
        else
          graymap[i * cols + j] = not_edge;
      }
    }
  }
}


int main(int argc, char* argv[]){
  FILE* ifp;
  int ich1, ich2, maxval=255, pgmraw, filter_size = 5, total_runs=1, threshold = 200;
  int i;

  /*
    Argument Handler
  */
  if ( argc != 2 ){
    printf("\nUsage: %s file\n\n", argv[0]);
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

  for(i=0; i < rows * cols; i++)
    if(pgmraw)
      graymap[i] = pm_getrawbyte(ifp) ;
    else
      graymap[i] = pm_getint(ifp);

  /*
    Processing
    Courtesy: https://towardsdatascience.com/canny-edge-detection-step-by-step-in-python-computer-vision-b49c3a2d8123
  */
  // Noise Reduction;
  repeat_gaussian(total_runs, filter_size);

  // Gradient calculation;
  scharr_filter(threshold);
  
  // Non-maximum suppression;
  non_maxima_suppression();

  // Double threshold;
  // double_thresholding();

  // Edge Tracking by Hysteresis.
  // hysteresis();
  
  if(pgmraw)
    printf("P2\n");
  else
    printf("P5\n");

  printf("%d %d \n", cols, rows);
  printf("%d\n",maxval);

  for(i=0; i < rows * cols ; i++){
    if(pgmraw)
      printf("%d ", graymap[i]);
    else
      printf("%c", graymap[i]);
  }

  fclose(ifp);
  return 0;
}
