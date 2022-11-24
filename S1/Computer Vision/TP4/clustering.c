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

/**
 * @brief For generating the random centroid
 * 
 * @param k_cluster -> total cluster that we have
 * @param maxval -> the highest value of the pixel that we can get
 * @return int* -> the inital value of cluster
 */
int* generate_random_centroid(int k_cluster,  int maxval){
  int i;
  int *cluster = malloc(sizeof(int) * k_cluster * pixel_val);
  for(i = 0; i < k_cluster * pixel_val; i++){
    cluster[i] = rand() % (maxval + 1);
  }

  return cluster;
}

/**
 * @brief For calculate distance of pixel to the cluster and return nearest cluster
 * 
 * @param pixel -> Coordinate of the pixel that we want to calculate
 * @param k_cluster -> total cluster that we have
 * @param cluster -> value of mark/label that we will use
 * @return int -> the nearest cluster 
 */
int euclidean_distance(gray *pixel, int k_cluster, int *cluster){
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

/**
 * @brief For create the mark from cluster
 * 
 * @param k_cluster -> total cluster that we have
 * @param cluster -> value of mark/label that we will use
 * @return int* -> the new label/mark
 */
int* map(int k_cluster, int *cluster){
  int i, j;
  int *temp = malloc(sizeof(int) * cols * rows);
  for(i = 0; i < cols; i++){
    for(j = 0; j < rows; j++){
      int index = i * (rows * pixel_val) + (pixel_val * j);
      temp[i * rows + j] = euclidean_distance(&image[index], k_cluster, cluster);
    }
  }

  return temp;
}

/**
 * @brief For Update cluster based on the value of the mark
 * 
 * @param k_cluster -> total cluster that we have
 * @param mark -> value of mark/label that we will use
 * @return int* -> the new cluster
 */
int *update_cluster(int k_cluster, int *mark){
  int *cluster = malloc(sizeof(int) * k_cluster * pixel_val);
  unsigned long long int *temp = malloc(sizeof(unsigned long long int) * k_cluster * pixel_val);
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
      if(count[i] != 0)
        cluster[i*pixel_val + j] = (int) (temp[i * pixel_val + j] / count[i]);
    }
  }

  return cluster;
}

/**
 * @brief For Checking the value of cluster result
 * 
 * @param k_cluster -> total cluster that we have
 * @param cluster -> value of cluster that we want to print
 * @param mark -> value of mark/label that we want to print
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

/**
 * @brief For Copy the value from cluster to the new address
 * 
 * @param source -> The cluster that we want to copy
 * @param k_cluster -> total cluster that we have
 * @return int* -> the address of the new value from the cluster
 */
int *copy(int *source, int k_cluster){
  int *dest = malloc(sizeof(int) * k_cluster * pixel_val);
  for(int i = 0; i < k_cluster * pixel_val; i++){
    dest[i] = source[i];
  }

  return dest;
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
    if(ich2 == '3')
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
  // cluster = malloc(sizeof(int) * k_cluster * pixel_val);
  // cluster[0] = 156;
  // cluster[1] = 135;
  // cluster[2] = 24;

  // cluster[3] = 57;
  // cluster[4] = 3;
  // cluster[5] = 1;

  // cluster[6] = 254;
  // cluster[7] = 151;
  // cluster[8] = 237;
  // int *start = copy(cluster, k_cluster);

  for(i=0; i < total_runs; i++){
    mark = map(k_cluster, cluster);
    cluster = update_cluster(k_cluster, mark);
    // print(k_cluster, cluster, mark);
  }

  int *color = generate_random_centroid(k_cluster, maxval);

  printf("P3\n");

  printf("%d %d \n", cols, rows);
  printf("%d\n",maxval);

  for(i=0; i < rows * cols ; i++){
    int k_val = mark[i];
    for(j=0; j < pixel_val; j++){
      // printf("%d ", image[i * pixel_val + j]);
      printf("%d ", color[k_val * pixel_val + j]);
      // printf("%d ", cluster[k_val * pixel_val + j]);
    }
  }

  // printf("\n");
  // print(k_cluster, start, mark);
  // printf("\n");
  // print(k_cluster, cluster, mark);

  fclose(ifp);
  return 0;
}
