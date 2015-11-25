#ifndef _OPTIMIZE_
#define _OPTIMIZE_

#include "score.h"
#include "limits.h"

typedef struct {
	float score[COMB_EVIDENCES];
	float weigth[COMB_EVIDENCES];
	int c;
} opt_t;

int optimize();

#endif
