#ifndef _SCORE_
#define _SCORE_

#include "rule.h"
#include "timer.h"

typedef struct {
	int* ordered_labels;
	float *points;
} score_t;

float* get_THE_score(int evidence);
score_t get_TOTAL_score();

#endif
