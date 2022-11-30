#include <stdio.h>
#include <stdlib.h>
#include "imageFormationUtils.h"

int main(int argc, char* argv[])
{
  /*
        Rigid    Perspective   Final Mapping
    Pworld -> Pcamera -> x_image -> u,v pixel
    3D     -> 3D      -> 2D      -> 2D 
  */
  
  struct point3d *points, *pinholes, *points_2d;
  int N_v = 0;
  float f = 0.2;
  int resolution_u = 500;
  int resolution_v = 500;

  points = readOff(argv[1], &N_v);
  centerThePCL(points, N_v);
  
  float gama = 0.2;
  float beta = 0.2;
  float alpha = 0.2;
  float T_x = 0;
  float T_y = 0;
  float T_z = 0;
  
  gama = 0;
  beta = 180;
  alpha = 0;

  float TransMatrix[16];
  computeTrans(gama, beta, alpha, T_x, T_y, T_z, TransMatrix);
  transform(points, N_v, TransMatrix);
  /* For using pinhole camera*/
  // pinholes = pinhole(points, N_v, f);

  /* For using orthographic camera*/
  pinholes = orthographic(points, N_v);
  points_2d = normalize_dot(pinholes, N_v, resolution_u, resolution_v);
  create_image(N_v, points_2d, resolution_u, resolution_v);
} 