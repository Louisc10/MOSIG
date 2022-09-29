#include <stdlib.h>
#include <complex.h>
#include <assert.h>
#include <stdio.h>

/**
   Ajouter la fonction de comparaison du tri ici
*/

double comp(const void * a, const void * b){
	return ( *(double*)a - *(double*)b );
}

const int TAILLE = 100;

int main(void)
{
    /* initialisation du tableau */
    double complex tableau[TAILLE];
    srand48(123456);
    for(int i = 0; i < TAILLE; i++) {
        tableau[i] = drand48() + (drand48() * 1.0i);
    }

    /**
       Mettre le code d'appel de qsort ici
    */

	qsort(tableau, (size_t) TAILLE, sizeof(double), comp);
    /**
       Vérification du tri
    */
    for(int i = 0; i < TAILLE - 1; i++) {
    	printf("%lf\n", tableau[i]);
        //assert(carg(tableau[i]) <= carg(tableau[i+1]));
    }

    return EXIT_SUCCESS;
}
