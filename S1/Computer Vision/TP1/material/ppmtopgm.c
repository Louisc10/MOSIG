#include<stdlib.h>
#include<stdio.h>
#include"Util.h"

int main(int argc, char* argv[])
    {
    FILE* ifp;
    gray* redmap, *greenmap, *bluemap;
    int ich1, ich2, rows, cols, maxval=255, pgmraw, i, j;

    /* Arguments */
    if ( argc != 2 ){
      printf("\nUsage: %s file \n\n", argv[0]);
      exit(0);
    }

    /* Opening */
    ifp = fopen(argv[1],"r");
    if (ifp == NULL) {
      printf("error in opening file %s\n", argv[1]);
      exit(1);
    }

    /*  Magic number reading */
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
      else pgmraw = 1;

    /* Reading image dimensions */
    cols = pm_getint( ifp );
    rows = pm_getint( ifp );
    maxval = pm_getint( ifp );

    /* Memory allocation  */
    redmap = (gray *) malloc(cols * rows * sizeof(gray));
    greenmap = (gray *) malloc(cols * rows * sizeof(gray));
    bluemap = (gray *) malloc(cols * rows * sizeof(gray));

    /* Reading */
    for(i=0; i < rows; i++){
      for(j=0; j < cols ; j++){
        if(pgmraw){
          redmap[i * cols + j] = pm_getrawbyte(ifp) ;
          greenmap[i * cols + j] = pm_getrawbyte(ifp) ;
          bluemap[i * cols + j] = pm_getrawbyte(ifp) ;
        }
        else{
          redmap[i * cols + j] = pm_getint(ifp);
          greenmap[i * cols + j] = pm_getint(ifp);
          bluemap[i * cols + j] = pm_getint(ifp);
        }
      }
    }

    /* Writing */
    if(pgmraw)
      printf("P2\n");
    else
      printf("P5\n");

    printf("%d %d \n", cols, rows);
    printf("%d\n",maxval);

    for(i=0; i < rows; i++)
      for(j=0; j < cols ; j++){
        float sepR = 0.339f;
        float sepG = 0.537f;
        float sepB = 0.124f;

        int tempR = redmap[i * cols + j] * sepR;
        int tempG = greenmap[i * cols + j] * sepG;
        int tempB = bluemap[i * cols + j] * sepB;

        int temp = (tempR + tempG + tempB) % 255;
        if(pgmraw)
          printf("%d ", temp);
        else
          printf("%c ", temp);
        /*putc(graymap[i * cols + j],stdout);*/
      }


    /* Closing */
    fclose(ifp);
    return 0;
}
