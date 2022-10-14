#include <stdlib.h>
#include <stdio.h>
#include "Util.h"


int stretch_intensity(int intensity, int maxval, int max_intensity, int min_intensity)
{
    return maxval * (intensity - min_intensity) / (max_intensity - min_intensity);
}


int main(int argc, char* argv[])
    {
    FILE* ifp;
    gray* graymap;
    int ich1, ich2, rows, cols, pgmraw;
    int maxval=255;
    int minval=0;
    int i, j;
    int max_intensity, min_intensity;
    
    /*unsigned int* histogram;*/
    unsigned int histogram[256] = {0};

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
    if(ich2 != '2' && ich2 != '5')
      pm_erreur(" wrong file type ");
    else
      if(ich2 == '2')
	    pgmraw = 0;
      else pgmraw = 1;

    /* Reading image dimensions */
    cols = pm_getint( ifp );
    rows = pm_getint( ifp );
    maxval = pm_getint( ifp );

    /* Memory allocation  */
    graymap = (gray *) malloc(cols * rows * sizeof(gray));
    /*histogram = (unsigned int *) malloc(cols * rows * sizeof(unsigned int));*/


    /* Reading */
    for(i=0; i < rows; i++)
      for(j=0; j < cols ; j++)
        if(pgmraw)
        {
            graymap[i * cols + j] = pm_getrawbyte(ifp) ;
            histogram[(unsigned int)graymap[i * cols + j]] += 1;
        }
        else
        {
            graymap[i * cols + j] = pm_getint(ifp);
            histogram[(unsigned int)graymap[i * cols + j]] += 1;
        }


    /* Histogram */
    max_intensity = minval;
    min_intensity = maxval;

    while(histogram[max_intensity] == 0)
    {
        max_intensity--;
    }

    while(histogram[min_intensity] == 0)
    {
        min_intensity++;
    }


    /* Writing */
    
    if(pgmraw)
      printf("P2\n");
    else
      printf("P5\n");

    printf("%d %d \n", cols, rows);
    printf("%d\n",maxval);

    for(i=0; i < rows; i++)
    {
      for(j=0; j < cols ; j++)
        {
        if(pgmraw)
          printf("%d ", stretch_intensity((int)graymap[i * cols + j], maxval, max_intensity, min_intensity));
        else
          printf("%c", stretch_intensity((int)graymap[i * cols + j], maxval, max_intensity, min_intensity));
        }

      /* Closing */
      /*fclose(ifp);*/
      /*return 0;*/
    }

    fclose(ifp);
    return 0;
}
