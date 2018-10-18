#ifndef TVECTOR_C
#define TVECTOR_C


#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef int TIndex;
typedef double TValue;


typedef struct TVector {
	TIndex size;
	TValue *values;

	static struct TVector * create (TIndex size, TValue value = 0) {
		struct TVector * result = (struct TVector *)malloc(sizeof(struct TVector));
		result->init(size, value);
		return result;
	}

	static struct TVector * create (TIndex size, TValue values[]) {
		struct TVector * result = TVector::create(0);
		result->init(size, values);
		return result;
	}

	static struct TVector * clone (struct TVector * vector) {
		return vector==NULL ? NULL : TVector::create(vector->size, vector->values);
	}

	static struct TVector * clean (struct TVector * vector) {
		if (vector!=NULL) {
			vector->clean();
			free(vector);
		}
		return NULL;
	}

	void clean () {
		if (this->values!=NULL) {
			free(this->values);
			this->values = NULL;
			this->size = 0;
		}
	}

	void init (TIndex size, TValue value = 0) {
		this->size = 0;
		this->values = NULL;
		if (size>0) {
			this->size = size;
			this->values = (TValue *)malloc(sizeof(TValue) * size);
			for (TIndex i=0; i<size; i++) {
				this->values[i] = value;
			}
		}
	}

	void init (TIndex size, TValue value[]) {
		this->init(size);
		for (TIndex i=0; i<size; i++) {
			this->values[i] = value[i];
		}
	}

	TVector * copy (TVector * vector) {
		if (vector!=NULL && this->size==vector->size) {
			for (TIndex i=0; i<this->size; i++) {
				this->values[i] = vector->values[i];
			}
		}
		return this;
	}

	TVector * multi (TValue value) {
		for (TIndex i=0; i<this->size; this->values[i++] *= value);
		return this;
	}

	TVector * plus (struct TVector * vector) {
		for (TIndex i=0; i<this->size; this->values[i] += i<vector->size ? vector->values[i] : 0, i++);
		// TODO: size 3*4 and 4*3
		return this;
	}

	TVector * swap (TIndex i1, TIndex i2) {
		if (i1>=0 && i2>=0 && i1<this->size && i2<this->size && i1!=i2) {
			TValue tmp = this->values[i1];
			this->values[i1] = this->values[i2];
			this->values[i2] = tmp;
		}
		return this;
	}

	void print (const char title[] = NULL) {
		title==NULL ? printf("Vector (") : printf("Vector [%s] (", title);
		for (TIndex i=0; i<this->size; i++) {
			printf("%s%.8lf", i==0 ? "" : ", ", this->values[i]);
		}
		printf(")\n");
	}

	TValue * valueOf() {
		TValue * result = (TValue *)malloc(sizeof(TValue) * this->size);
		memcpy(result, this->values, sizeof(TValue) * this->size);
		return result;
	}

} TVector;

#endif // TVECTOR_C

/** /
int main(int argc, char const *argv[]) {
	double x1[] = {4, 6, 2, 66.6};
	TVector * v1 = TVector::create(4, x1);
	TVector * v2 = TVector::create(4, 1);
	TVector * v12 = TVector::clone(v1)->plus(v2)->multi(1.1);

	double * x12 = v12->valueOf();
	printf("x12: ");
	for (int i=0; i<v12->size; i++){
		printf("%lf ", *(x12+i));
	}
	printf("\n\n");
	free(x12);

	v1->print("v1");
	v2->print("v2");
	v12->print("v1 + v2");


	v1 = TVector::clean(v1);
	v2 = TVector::clean(v2);
	v12 = TVector::clean(v12);
	return 0;
}
/**/