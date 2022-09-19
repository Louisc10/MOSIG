#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include"Util.h"

gray* redmap, *greenmap, *bluemap;
int ich1, ich2, rows, cols, maxval=255, pgmraw, i, j;

void fileCreater(char* argv[], char *filename, int flag){
    char *temp = malloc(strlen(strtok(argv[1],".ppm"))); 
    strcpy(temp, strtok(argv[1],".ppm"));
    FILE *fp = fopen(strcat(temp, filename), "w");
    if(pgmraw)
      fprintf(fp,"P3\n");
    else
      fprintf(fp,"P6\n");

    fprintf(fp,"%d %d \n", cols, rows);
    fprintf(fp,"%d\n",maxval);

    for(i=0; i < rows; i++)
      for(j=0; j < cols ; j++){
        int tempR = redmap[i * cols + j];
        int tempG = greenmap[i * cols + j];
        int tempB = bluemap[i * cols + j];
        if(pgmraw){
          fprintf(fp,"%d ", flag == 0 ? tempR : 0);
          fprintf(fp,"%d ", flag == 1 ? tempG : 0);
          fprintf(fp,"%d ", flag == 2 ? tempB : 0);
        }
        else{
          fprintf(fp,"%c ", flag == 0 ? tempR : 0);
          fprintf(fp,"%c ", flag == 1 ? tempG : 0);
          fprintf(fp,"%c ", flag == 2 ? tempB : 0);
        }
        /*putc(graymap[i * cols + j],stdout);*/
      }
    
    fclose(fp);
}

int main(int argc, char* argv[])
    {
    FILE* ifp;
    
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
    fileCreater(argv,"_red.ppm",0);
    fileCreater(argv,"_green.ppm",1);
    fileCreater(argv,"_blue.ppm",2);

    /* Closing */
    fclose(ifp);
    return 0;
}
