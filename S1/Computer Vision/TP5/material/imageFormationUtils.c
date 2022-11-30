/* Naive off reader for point cloud from .off file */
/* Only works for TP5 */
/* Point cloud is stored in points */
/* The size of points is N */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include "imageFormationUtils.h"


struct point3d *readOff(char *fileName, int *N){
	struct point3d *points;

	FILE *ifp = fopen(fileName, "r");
	if (ifp == NULL) {
  		fprintf(stderr, "Can't open input file in.list!\n");
  		exit(1);
	}

	char fooChar[16];
	int N_v, fooInt;
	fscanf(ifp, "%s", fooChar);
	//printf("%s \n", fooChar);
	fscanf(ifp, "%d %d %d", &N_v, &fooInt, &fooInt);
	*N = N_v;
	points = (struct point3d *) malloc(N_v * sizeof(struct point3d));
	//printf("%d %d %d\n", N_v, fooInt, fooInt);
	int i = 0;
	for(i = 0; i < N_v; i++){
		fscanf(ifp, "%f %f %f %d %d %d %d", &(points[i].x), &(points[i].y), &(points[i].z), &(points[i].r), &(points[i].g), &(points[i].b), &fooInt);
		//printf("%f %f %f %d %d %d\n", points[i].x, points[i].y, points[i].z, points[i].r, points[i].g, points[i].b);
	}

	return points;
}

void centerThePCL(struct point3d *points, int N){
  int i = 0;
  float xMin = 100000, xMax = -1000000, yMin = 1000000, yMax = -10000000, zMin = 100000000, zMax = -10000000;
  for(i = 0; i < N; i++){
    if(points[i].x > xMax)
      xMax = points[i].x;
    else if (points[i].x < xMin)
      xMin = points[i].x;

    if(points[i].y > yMax)
      yMax = points[i].y;
    else if (points[i].y < yMin)
      yMin = points[i].y;

    if(points[i].z > zMax)
      zMax = points[i].z;
    else if (points[i].z < zMin)
      zMin = points[i].z;
  }

  float xMid = (xMin+xMax)/2, yMid = (yMin+yMax)/2, zMid = (zMin+zMax)/2;
  for(i = 0; i < N; i++){
    points[i].x = points[i].x-xMid;
    points[i].y = points[i].y-yMid;
    points[i].z = points[i].z-zMid;
  }
}


void Rx(double alpha, float * R){

  int i = 0, j = 0;
  for(i = 0; i < 4; i++)
    for(j = 0; j < 4; j++)
      R[i*4+j] = 0;
    // 1      0     0    0
    //   0     cosA -sinA  0
    //   0     sinA  cosA  0
    //   0      0     0    1
      
  R[0] = 1;

  R[5] = cos(alpha);
  R[6] = -sin(alpha);

  R[9] = sin(alpha);
  R[10] = cos(alpha);

  R[15] = 1;

  return;
}

void Ry(float alpha, float *R){

  int i = 0, j = 0;
  for(i = 0; i < 4; i++)
    for(j = 0; j < 4; j++)
      R[i*4+j] = 0;
  /* cosA    0    sinA   0
      0      1     0     0
    -sinA    0    cosA   0
      0      0     0     1
      */
  R[0] = cos(alpha);
  R[2] = sin(alpha);

  R[5] = 1;

  R[8] = -sin(alpha);
  R[10] = cos(alpha);

  R[15] = 1;

  return;
}

void Rz(float alpha, float *R){

  int i = 0, j = 0;
  for(i = 0; i < 4; i++)
    for(j = 0; j < 4; j++)
      R[i*4+j] = 0;
  /* cosA  -sinA   0   0
     sinA   cosA   0   0
      0      0     1   0
      0      0     0   1
      */
  R[0] = cos(alpha);
  R[1] = -sin(alpha);

  R[4] = sin(alpha);
  R[5] = cos(alpha);

  R[10] = 1;
  R[15] = 1;

  return;
}

void matMul(float *A, float *B, float *result){
  float tmpA[16], tmpB[16];
  int i = 0, j = 0;
  for(i = 0; i < 4; i++){
    for(j = 0; j < 4; j++){
      tmpA[i*4+j] = A[i*4+j];
      tmpB[i*4+j] = B[i*4+j];
    }
  }
  for(i = 0; i < 4; i++){
    for(j = 0; j < 4; j++){
      result[i*4+j] = tmpA[i*4+0]*tmpB[0*4+j]+tmpA[i*4+1]*tmpB[1*4+j]+tmpA[i*4+2]*tmpB[2*4+j]+tmpA[i*4+3]*tmpB[3*4+j];
    }
  }
  return;
}

void computeTrans(float gama, float beta, float alpha, float T_x, float T_y, float T_z, float *result){
  float R_x[16], R_y[16], R_z[16];
  Rx(gama, R_x);
  Ry(beta, R_y);
  Rz(alpha, R_z);

  matMul(R_y, R_z, result);
  matMul(R_x, result, result);
  result[3] = T_x;
  result[7] = T_y;
  result[11] = T_z;
  return;
}

struct point3d *transform(struct point3d *points, int N_v, float *matrix){
  int i;
  struct point3d *temp = malloc(sizeof(struct point3d));
  for(i = 0; i < N_v; i++){
    temp[0].x = points[i].x;
    temp[0].y = points[i].y;
    temp[0].z = points[i].z;
    temp[0].r = points[i].r;
    temp[0].g = points[i].g;
    temp[0].b = points[i].b;

    points[i].x = matrix[0]*temp[0].x + matrix[1]*temp[0].y + matrix[2]*temp[0].z + matrix[3];
    points[i].y = matrix[4]*temp[0].x + matrix[5]*temp[0].y + matrix[6]*temp[0].z + matrix[7];
    points[i].z = matrix[8]*temp[0].x + matrix[9]*temp[0].y + matrix[10]*temp[0].z + matrix[11];
  }

  return points;
}

struct point3d *pinhole(struct point3d *points, int N_v, float f){
  int i;
  struct point3d *temp = malloc(sizeof(struct point3d));
  for(i = 0; i < N_v; i++){
    temp[0].x = points[i].x;
    temp[0].y = points[i].y;
    temp[0].z = points[i].z;
    // temp[0].r = points[i].r;
    // temp[0].g = points[i].g;
    // temp[0].b = points[i].b;

    points[i].x = temp[0].x/((1 + temp[0].z)/f);
    points[i].y = temp[0].y/((1 + temp[0].z)/f);
    // points[i].z = 0;
  }

  return points;
}

struct point3d *orthographic(struct point3d *points, int N_v){
  int i;
  struct point3d *temp = malloc(sizeof(struct point3d));
  for(i = 0; i < N_v; i++){
    temp[0].x = points[i].x;
    temp[0].y = points[i].y;
    temp[0].z = points[i].z;
    // temp[0].r = points[i].r;
    // temp[0].g = points[i].g;
    // temp[0].b = points[i].b;

    points[i].x = temp[0].x;
    points[i].y = temp[0].y;
    // points[i].z = 0;
  }

  return points;
}

void print_result(int N_v, struct point3d *points){
  int i;
  printf("%s\n", "OFF");
  printf("%d %d %d", N_v, 0, 0);
  for(i = 0; i < N_v; i++){
    printf("\n%f %f %f %d %d %d %d", points[i].x, points[i].y, points[i].z, points[i].r, points[i].g, points[i].b, 255);
  }
}

struct point3d *normalize_dot(struct point3d *points, int N_v, int resolution_u, int resolution_v){
  int i;
  int central_width = resolution_u/2;
  int central_height = resolution_v/2; 
  float alpha_u = 0.005; 
  float alpha_v = 0.005; 
  for(i = 0; i < N_v; i++){
    points[i].x = central_width + (points[i].x/alpha_u);
    points[i].y = central_height + (points[i].y/alpha_v);
  }
  return points;
}

bool is_still_seen(int x, int y, int w, int h){
  if (x <0 || y <0)
    return false;
  if(x >= w || y >= h)
    return false;
  return true;
}

void create_image(int N_v, struct point3d *points, int resolution_u, int resolution_v){
  int i,j;
  int *count=malloc(sizeof(int) * resolution_u * resolution_v);
  int *red=malloc(sizeof(int) * resolution_u * resolution_v);
  int *green=malloc(sizeof(int) * resolution_u * resolution_v);
  int *blue=malloc(sizeof(int) * resolution_u * resolution_v);
  for(i=0;i<resolution_v;i++){
    for(j=0;j<resolution_u;j++){
      count[i*resolution_u + j] = -1;
      red[i*resolution_u + j] = 0;
      green[i*resolution_u + j] = 0;
      blue[i*resolution_u + j] = 0;
    }
  }
  
  for(i=0;i<N_v;i++){
    int index_x = (int) points[i].x;
    int index_y = (int) points[i].y;
    if(!is_still_seen(index_x,index_y,resolution_u, resolution_v))
      continue;
    if(count[index_y * resolution_u + index_x] == -1){
      count[index_y * resolution_u + index_x] = points[i].z;
      red[index_y * resolution_u + index_x] = points[i].r;
      green[index_y * resolution_u + index_x] = points[i].g;
      blue[index_y * resolution_u + index_x] = points[i].b;
    }
    else if(points[i].z >= count[index_y * resolution_u + index_x]){
      red[index_y * resolution_u + index_x] = points[i].r;
      green[index_y * resolution_u + index_x] = points[i].g;
      blue[index_y * resolution_u + index_x] = points[i].b;
    }
  }
  printf("%s\n", "P3");
  printf("%d %d\n%d\n", resolution_u, resolution_v, 255);
  for(i=0;i<resolution_v;i++){
    for(j=0;j<resolution_u;j++){
      if(count[i * resolution_u + j] == -1)
        printf("0 0 0 ");
      else
        printf("%d %d %d ", red[i * resolution_u + j], green[i * resolution_u + j], blue[i * resolution_u + j]);
    }
  }
}