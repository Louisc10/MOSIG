#include "imageFormationUtils.h"

int main(int argc, char* argv[])
{
  /*
        Rigid    Perspective   Final Mapping
    Pworld -> Pcamera -> x_image -> u,v pixel
    3D     -> 3D      -> 2D      -> 2D 
  */
  
  struct point3d *points, *pinholes;
  int N_v = 0;
  int f = 3;
  int resolution_u = 500;
  int resolution_v = 500;

  points = readOff(argv[1], &N_v);
  centerThePCL(points, N_v);
  pinholes = pinhole(points, N_v, f);
  
//   float gama =  0.1;
//   float beta =  1.74;
//   float alpha =  0.2;
//   float T_x =  0;
//   float T_y =  0.52;
//   float T_z =  0.15;

//   float TransMatrix[16];
//   computeTrans(gama, beta, alpha, T_x, T_y, T_z, TransMatrix);

  print_result(N_v, pinholes);
} 