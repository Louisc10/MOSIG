#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <time.h>
#include "Util.h"

int rows, cols;
gray *image;
int pixel_val = 3;

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
  
  return image[row * cols + col];
}

int* generate_random_centroid(int k_cluster,  int maxval){
  int i;
  int *cluster = malloc(sizeof(int) * k_cluster * pixel_val);
  for(i = 0; i < k_cluster * pixel_val; i++){
    cluster[i] = rand() % (maxval + 1);
  }

  return cluster;
}

double euclidean_distance(gray *pixel, int k_cluster, int *cluster){
  double min = __INT_MAX__ * 1.0;
  int k_val = -1;
  int i, j;
  for(i = 0; i < k_cluster; i++){
    double temp = 0;
    for(j = 0; j < pixel_val; j++){
      temp += (pixel[j] - cluster[i* pixel_val + j]) * (pixel[j] - cluster[i* pixel_val + j]);
    }
    temp = sqrt(temp);
    if(temp < min){
      min = temp;
      k_val = i;
    }
  }
  return k_val;
}

int* map(int k_cluster, int *cluster){
  int i, j;
  int *temp = malloc(sizeof(int) * cols * rows);
  for(i = 0; i < cols; i++){
    for(j = 0; j < rows; j++){
      temp[i * rows + j] = euclidean_distance(&image[i * rows + (pixel_val * j)], k_cluster, cluster);
    }
  }

  return temp;
}

int *update_cluster(int k_cluster, int *mark){
  int *cluster = malloc(sizeof(int) * k_cluster * pixel_val);
  double *temp = malloc(sizeof(double) * k_cluster * pixel_val);
  int *count = malloc(sizeof(int) * k_cluster);

  int i,j,k;
  for(i=0;i<k_cluster;i++)
    count[i]= 0;

  for(i=0; i<cols; i++){
    for(j=0; j<rows; j++){
      int k_val = mark[i*rows + j];
      count[k_val]++;
      for(k=0; k<pixel_val; k++){
        temp[k_val*pixel_val + k] += image[i * rows + (pixel_val * j) + k];
      }
    }
  }

  for(i=0; i < k_cluster; i++){
    for(j=0; j<pixel_val; j++){
      cluster[i*pixel_val + j] = temp[i * pixel_val + j] / count[i];
    }
  }

  return cluster;
}

/*
  For Checking the value of cluster result
*/
void print(int k_cluster, int *cluster, int *mark){
  int i,j;
  for(i=0; i < k_cluster ; i++){
    for(j = 0; j < pixel_val; j++){
      printf("%d ", cluster[i * pixel_val + j]);
    }
    printf("\n");
  }
  printf("\nValue\n");
  for(i=0; i < rows * cols ; i++){
    printf("%d ", mark[i]);
  }
  printf("\n========================\n");
}

int main(int argc, char* argv[]){
  FILE* ifp;
  int *cluster, *mark;
  int ich1, ich2, maxval=255, pgmraw, k_cluster, total_runs=1;
  int i, j;
  srand(time(NULL));

  /*
    Argument Handler
  */
  if ( argc != 4 ){
    printf("\nUsage: %s file cluster run\n\n", argv[0]);
    exit(0);
  }

  ifp = fopen(argv[1],"r");
  if (ifp == NULL) {
    printf("error in opening file %s\n", argv[1]);
    exit(1);
  }
  
  k_cluster = atoi(argv[2]);
  if(k_cluster < 0){
    printf("You need at least a cluster\n");
    exit(1);
  }
  
  total_runs = atoi(argv[3]);
  if(total_runs < 0){
    printf("k-means need to run at least once\n");
    exit(1);
  }

  ich1 = getc( ifp );
  if ( ich1 == EOF )
    pm_erreur( "EOF / read error / magic number" );
  ich2 = getc( ifp );
  if ( ich2 == EOF )
    pm_erreur( "EOF /read error / magic number" );
  if(ich2 != '3' && ich2 != '6')
    pm_erreur(" wrong file type ");
  else
    if(ich2 == '2')
      pgmraw = 0;
    else 
      pgmraw = 1;

  cols = pm_getint( ifp );
  rows = pm_getint( ifp );
  maxval = pm_getint( ifp );

  image = (gray *) malloc(pixel_val * cols * rows * sizeof(gray));

  for(i=0; i < rows * cols; i++){
    for(j=0; j < pixel_val; j++){
      if(pgmraw)
        image[i*pixel_val + j] = pm_getrawbyte(ifp) ;
      else
        image[i*pixel_val + j] = pm_getint(ifp);
    }
  }

  cluster = generate_random_centroid(k_cluster, maxval);

  for(i=0; i < total_runs; i++){
    mark = map(k_cluster, cluster);
    cluster = update_cluster(k_cluster, mark);
    print(k_cluster, cluster, mark);
  }
  
  printf("P3\n");

  printf("%d %d \n", cols, rows);
  printf("%d\n",maxval);

  for(i=0; i < rows * cols ; i++){
    int k_val = mark[i];
    for(j=0; j < pixel_val; j++){
      printf("%d ", cluster[k_val * pixel_val + j]);
    }
  }

  fclose(ifp);
  return 0;
}
