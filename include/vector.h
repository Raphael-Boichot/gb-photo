#ifndef __VECTOR_H_INCLUDE__
#define __VECTOR_H_INCLUDE__

#include <string.h>

#define VECTOR_DECLARE(NAME, TYP, NUM_ELEMS) TYP NAME[NUM_ELEMS + 1] = {0}
#define VECTOR_DECLARE_EXTERN(NAME, TYP, NUM_ELEMS) extern TYP NAME[NUM_ELEMS + 1]
#define VECTOR_LEN(VECTOR) (VECTOR[0])
#define VECTOR_ITERATE(VECTOR, IDX, ELEM) for(IDX = 0, ELEM = VECTOR[1]; IDX < VECTOR_LEN(VECTOR); IDX++, ELEM = VECTOR[IDX + 1])
#define VECTOR_CLEAR(VECTOR) (VECTOR[0]=0)
#define VECTOR_GET(VECTOR, POS) (VECTOR[(POS) + 1])
#define VECTOR_ADD(VECTOR, ELEM) (VECTOR[++VECTOR[0]] = (ELEM))
#define VECTOR_DEL(VECTOR, POS) (memcpy(&(VECTOR[(POS) + 1]), &(VECTOR[(POS) + 2]), ((VECTOR[0]) - (POS)) * sizeof(VECTOR[0])),VECTOR[0]--)
#define VECTOR_POP(VECTOR) (VECTOR[VECTOR[0]--])
#define VECTOR_INS(VECTOR, POS, ELEM) (memmove(&(VECTOR[(POS) + 2]), &(VECTOR[(POS) + 1]), ((VECTOR[0]) - (POS) + 1) * sizeof(VECTOR[0])), (VECTOR[(POS) + 1] = (ELEM)), VECTOR[0]++)
#define VECTOR_SET_DIRECT(VECTOR, POS, ELEM) ((VECTOR[(POS) + 1]) = (ELEM))

#endif