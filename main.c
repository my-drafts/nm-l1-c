#include <stdio.h>
#include <stdlib.h>

#include "lib/TVector.c"
#include "lib/TMatrix.c"
#include "lib/TSLAU.c"


typedef struct {
	TIndex size;
	TMatrix *A; // size*size
	TVector *B; // size
} TInput;


void lu(void * L, void * U, TIndex it) {
	return;
	TMatrix * l = (TMatrix *)L;
	TMatrix * u = (TMatrix *)U;
	printf("iteration #%u:\n", it);
	l->print("l");
	u->print("u");
}

int main(int argc, char const *argv[]) {
	TIndex size = 5;
	TValue a[] = {
			6.37,   0.94,  1.13,   1.225,  0.09,
			1.26,   4.05,  1.3,   -1.63,  -1.53,
			0.81,  -2.46,  5.55,   2.1,   -1.067,
			1.255,  0.16,  2.1,    5.99,   16,
			0.39,   0.87,  1.333, -8,      9
		};
	TValue b[] = {
			2.1,    0.12, -3.44,  -6.44,  -0.03
		};

	TMatrix * A = TMatrix::create(size, size, a);
	TVector * B = TVector::create(size, b);
	TSLAU * S = TSLAU::create(A, B);
	TVector * X =	S->resolve(lu);
	X->print("X");

	X = TVector::clean(X);
	S = TSLAU::clean(S);
	B = TVector::clean(B);
	A = TMatrix::clean(A);
	return 0;
}
