#include <stdlib.h>
#include <stdio.h>

int main(void)
{
	float a = 0.1f + 0.2f + 0.3f;
	double b = 0.1 + 0.2 + 0.3;
	long double c = 0.1L + 0.2L + 0.3L;
	printf("%f\n", a);
	printf("%lf\n", b);
	printf("%Lf\n", c);
	
	
	printf("%f\n", 0.1f + 0.2f + 0.3f);
	printf("%lf\n", 0.1 + 0.2 + 0.3);
	printf("%Lf\n", 0.1L + 0.2L + 0.3L);
    return EXIT_SUCCESS;
}
