#ifndef _PREDICTION_
#define _PREDICTION_

#include "rule.h"
#include "score.h"

typedef struct {
	short correct;
	int label;
	score_t score;
} prediction_t;

#endif
