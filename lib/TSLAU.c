#ifndef TSLAU_C
#define TSLAU_C


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "TVector.c"
#include "TMatrix.c"


typedef struct TSLAU {
	TMatrix * matrix;

	static struct TSLAU * create (TMatrix * A, TVector *B) {
		if (A!=NULL && B!=NULL && A->rows==B->size) {
			struct TSLAU * result = (struct TSLAU *)malloc(sizeof(struct TSLAU));

			TIndex rows = A->rows, columns = A->columns + 1;
			TValue * values = (TValue *)malloc(sizeof(TValue) * rows * columns);
			for (TIndex i=0; i<rows; i++) {
				TVector * a = A->values[i];
				TValue * _ = a->valueOf();
				memcpy(values + i*columns, _, sizeof(TValue) * A->columns);
				free(_);
				TValue b = B->values[i];
				*(values + i*columns + A->columns) = b;
			}
			result->matrix = TMatrix::create(A->rows, A->columns + 1, values);
			free(values);
			return result;
		}
		return NULL;
	}

	static struct TSLAU * clean (struct TSLAU * slau) {
		if (slau!=NULL) {
			slau->clean();
			free(slau);
		}
		return NULL;
	}

	void clean () {
		if (this->matrix!=NULL) {
			this->matrix->clean();
			this->matrix = NULL;
		}
	}

	void print (const char title[] = NULL) {
		title==NULL ? printf("SLAU {") : printf("SLAU [%s] {\n", title);
		for (TIndex i=0; i<this->matrix->rows; i++) {
			for (TIndex j=0; j<this->matrix->columns; j++) {
				printf("%s%.8lf", j==0 ? "" : " ", this->matrix->values[i]->values[j]);
			}
			printf("\n");
		}
		printf("}\n");
	}

	TMatrix * A () {
		TIndex rows = this->matrix->rows, columns = this->matrix->columns-1;
		TValue * values = (TValue *)malloc(sizeof(TValue) * rows * columns);
		for (TIndex i=0; i<rows; i++) {
			TVector * v = this->matrix->values[i];
			TValue * _ = v->valueOf();
			memcpy(values + i*columns, _, sizeof(TValue) * columns);
			free(_);
		}
		TMatrix * result = TMatrix::create(rows, columns, values);
		free(values);
		return result;
	}

	TVector * B () {
		return this->column(this->matrix->columns-1);
	}

	TVector * column (TIndex j) {
		return this->matrix->column(j);
	}

	TMatrix * column (TIndex j, TVector * vector) {
		return this->matrix->column(j, vector);
	}

	TVector * row (TIndex i) {
		return this->matrix->row(i);
	}

	TMatrix * row (TIndex i, TVector * vector) {
		return this->matrix->row(i, vector);
	}

	TMatrix * swapColumns (TIndex i1, TIndex i2) {
		return this->matrix->swapColumns(i1, i2);
	}

	TMatrix * swapRows (TIndex i1, TIndex i2) {
		return this->matrix->swapRows(i1, i2);
	}

	TValue * valueOf() {
		return this->matrix->valueOf();
	}

	TVector * resolve(TLU f) {
		TMatrix * A = this->A();
		TVector * B = this->B();
		TMatrix * L = NULL, * U = NULL;
		TIndex it = A->LU(&L, &U, f);
		// A->print("A");
		// L->print("L");
		// U->print("U");
		TSLAU * LB = TSLAU::create(L, B);
		TVector * Y = LB->gaussDown();
		// Y->print("Y");
		LB = TSLAU::clean(LB);

		TSLAU * UY = TSLAU::create(U, Y);
		TVector * X = UY->gaussUp();
		// X->print("X");
		UY = TSLAU::clean(UY);

		B = TVector::clean(B);
		A = TMatrix::clean(A);
		return X;
	}

	/*
L:
		1.000000    0.000000    0.000000    0.000000    0.000000
		0.198083    1.000000    0.000000    0.000000    0.000000
		0.132588   -0.651735    1.000000    0.000000    0.000000
		0.198083   -0.007597    0.314891    1.000000    0.000000
		0.038339    0.172096    0.159178   -1.217739    1.000000
*/
	TVector * gaussDown() {
		TMatrix * A = this->A();
		TVector * B = this->B();
		TVector * Y = TVector::create(A->columns);
		for (TIndex i=0; i<A->rows; i++) {
			TValue value = B->values[i];
			for (TIndex j=0; j<i; j++) {
				value -= Y->values[j] * A->values[i]->values[j];
			}
			value /= A->values[i]->values[i];
			Y->values[i] = value;
		}
		A = TMatrix::clean(A);
		B = TVector::clean(B);
		return Y;
	}

/*
U:
		6.260000    0.960000    1.110000    1.240000    0.240000
		0.000000    3.969840    1.080128   -1.875623   -1.427540
		0.000000    0.000000    5.996785    0.713182   -1.879199
		0.000000    0.000000    0.000000    5.615553   14.533358
		0.000000    0.000000   -0.000000    0.000000   25.233430
*/
	TVector * gaussUp() {
		TMatrix * A = this->A();
		TVector * B = this->B();
		TVector * X = TVector::create(A->columns);
		for (TIndex i=0; i<A->rows; i++) {
			TIndex i_idx = A->rows-1 - i;
			TValue value = B->values[i_idx];
			for (TIndex j=0; j<i; j++) {
				TIndex j_idx = A->rows-1 - j;
				value -= X->values[j_idx] * A->values[i_idx]->values[j_idx];
			}
			value /= A->values[i_idx]->values[i_idx];
			X->values[i_idx] = value;
		}
		return X;
	}

} TSLAU;

#endif // TSLAU_C

/** /
int main(int argc, char const *argv[]) {
	double a[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
	double b[] = {11, 12, 13};
	TMatrix * A = TMatrix::create(3, 3, a);
	TVector * B = TVector::create(3, b);
	TSLAU * s = TSLAU::create(A, B);

	A->print("A");
	B->print("B");
	s->print("s");

	A = TMatrix::clean(A);
	B = TVector::clean(B);
	s = TSLAU::clean(s);
	return 0;
}
/**/