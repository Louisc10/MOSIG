#include<stdio.h>
#include<stdlib.h>

int main(){
	int *pa = 2;
	*pa = 34;
	int b = 4, *pb = &b;
	*pb = 5;
	int *pc;
	printf("pc is equal to %d\n", pc);
	printf("*pc is equal to %d\n", *pc);
	pc = malloc(sizeof(int));
	*pc = -2;
	pa = pc;
	free(pa);
	pc = -4;
}
