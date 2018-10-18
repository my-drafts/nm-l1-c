#ifndef TMAXRIX_C
#define TMAXRIX_C


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "TVector.c"


typedef void (* TLU)(void * L, void * U, TIndex it);


typedef struct TMatrix {
	TIndex rows;
	TIndex columns;
	TVector ** values; // rows*columns

	static struct TMatrix * create (TIndex rows, TIndex columns, TValue value = 0) {
		struct TMatrix * result = (struct TMatrix *)malloc(sizeof(struct TMatrix));
		result->init(rows, columns, value);
		return result;
	}

	static struct TMatrix * create (TIndex rows, TIndex columns, TValue values[]) {
		struct TMatrix * result = TMatrix::create(0, 0);
		result->init(rows, columns, values);
		return result;
	}

	static struct TMatrix * clone (struct TMatrix * matrix) {
		TValue * values = matrix->valueOf();
		TMatrix * result = matrix==NULL ? NULL : TMatrix::create(matrix->rows, matrix->columns, values);
		free(values);
		return result;
	}

	static struct TMatrix * clean (struct TMatrix * matrix) {
		if (matrix!=NULL) {
			matrix->clean();
			free(matrix);
		}
		return NULL;
	}

	void clean () {
		if (this->values!=NULL) {
			for (TIndex i=0; i<this->rows; i++) {
				this->values[i] = TVector::clean(this->values[i]);
			}
			free(this->values);
			this->values = NULL;
			this->rows = 0;
			this->columns = 0;
		}
	}

	void init (TIndex rows, TIndex columns, TValue value = 0) {
		this->rows = 0;
		this->columns = 0;
		this->values = NULL;
		if (rows>0 && columns>0) {
			this->rows = rows;
			this->columns = columns;
			this->values = (TVector **)malloc(sizeof(TVector*) * rows);
			for (TIndex i=0; i<rows; i++) {
				this->values[i] = TVector::create(columns, value);
			}
		}
	}

	void init (TIndex rows, TIndex columns, TValue value[]) {
		this->init(rows, columns);
		for (TIndex i=0; rows>0 && i<rows; i++) {
			for (TIndex j=0; columns>0 && j<columns; j++) {
				this->values[i]->values[j] = value[i*columns + j];
			}
		}
	}

	void print (const char title[] = NULL) {
		title==NULL ? printf("Matrix {") : printf("Matrix [%s] {\n", title);
		for (TIndex i=0; i<this->rows; i++) {
			for (TIndex j=0; j<this->columns; j++) {
				printf("%s%12.8lf", j==0 ? "" : " ", this->values[i]->values[j]);
			}
			printf("\n");
		}
		printf("}\n");
	}

	TVector * column (TIndex j) {
		TVector * result = TVector::create(this->rows);
		for (TIndex i=0; i<this->rows; i++) {
			result->values[i] = this->values[i]->values[j];
		}
		return result;
	}

	TMatrix * column (TIndex j, TVector * vector) {
		if (j>=0 && j<this->columns && vector!=NULL && this->rows==vector->size) {
			for (int i=0; i<this->rows; i++) {
				this->values[i]->values[j] = vector->values[j];
			}
		}
		return this;
	}

	TVector * row (TIndex i) {
		return (i>=0 && i<this->rows) ? TVector::clone(this->values[i]) : NULL;
	}

	TMatrix * row (TIndex i, TVector * vector) {
		if (i>=0 && i<this->rows) {
			this->values[i]->copy(vector);
		}
		return this;
	}

	TMatrix * swapColumns (TIndex i1, TIndex i2) {
		for (TIndex i=0; i<this->rows; i++) {
			this->values[i]->swap(i1, i2);
		}
		return this;
	}

	TMatrix * swapRows (TIndex i1, TIndex i2) {
		if (i1>=0 && i2>=0 && i1<this->rows && i2<this->rows && i1!=i2) {
			TVector * tmp = this->values[i1];
			this->values[i1] = this->values[i2];
			this->values[i2] = tmp;
		}
		return this;
	}

	TValue * valueOf() {
		TValue * result = (TValue *)malloc(sizeof(TValue) * this->rows * this->columns);
		for (TIndex i=0; i<this->rows; i++) {
			TVector * v = this->values[i];
			TValue * _ = v->valueOf();
			memcpy(result + i*this->columns, _, sizeof(TValue) * this->columns);
			free(_);
		}
		return result;
	}

	TIndex LU (TMatrix ** L, TMatrix ** U, TLU f = NULL) {
		if (L==NULL || U==NULL) return -1;
		*U = TMatrix::clean(*U);
		*L = TMatrix::clean(*L);
		// init
		*U = TMatrix::clone(this);
		*L = TMatrix::create(this->rows, this->columns);
		for (TIndex j=0; j<this->columns; j++) {
			for (TIndex i=j; i<this->rows; i++) {
				(*L)->values[i]->values[j] = (*U)->values[i]->values[j] / (*U)->values[j]->values[j];
			}
		}
		if (f!=NULL) f(*L, *U, 0);
		// iteration
		for(TIndex k=1; k<this->rows; k++) {
			for (TIndex j=k-1; j<this->columns; j++)
				for (TIndex i=j; i<this->rows; i++)
					(*L)->values[i]->values[j] = (*U)->values[i]->values[j] / (*U)->values[j]->values[j];
			for (TIndex i=k; i<this->rows; i++)
				for (TIndex j=k-1; j<this->columns; j++)
					(*U)->values[i]->values[j] = (*U)->values[i]->values[j] - (*L)->values[i]->values[k-1] * (*U)->values[k-1]->values[j];
			if (f!=NULL) f(*L, *U, k);
		}
		return this->rows;
	}

} TMatrix;

#endif // TMAXRIX_C

/** /
void lu(void * L, void * U, TIndex it) {
	TMatrix * l = (TMatrix *)L;
	TMatrix * u = (TMatrix *)U;
	printf("iteration #%u:\n", it);
	l->print("l");
	u->print("u");
}

int main(int argc, char const *argv[]) {
	double x1[] = {
			6.37,   0.94,  1.13,   1.225,  0.09,
			1.26,   4.05,  1.3,   -1.63,  -1.53,
			0.81,  -2.46,  5.55,   2.1,   -1.067,
			1.255,  0.16,  2.1,    5.99,   16,
			0.39,   0.87,  1.333, -8,      9
		};
	TMatrix * m1 = TMatrix::create(5, 5, x1);

	m1->print("m1");
	TMatrix * l1 = NULL, * u1 = NULL;
	m1->LU(&l1, &u1, lu);
	l1->print("l1");
	u1->print("u1");

	m1 = TMatrix::clean(m1);
	return 0;
}
/**/