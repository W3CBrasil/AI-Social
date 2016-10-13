#ifndef _EVALUATION_
#define _EVALUATION_

#include <math.h>
#include <stdlib.h>

#include "timer.h"
#include "rule.h"
#include "limits.h"

typedef struct {
	int *total_predictions, *correct_predictions, *true_labels;
	float acc, mf1, *precision, *recall, *f1;
} evaluation_t;

int initialize_evaluation(evaluation_t* evaluator);
int update_evaluation(evaluation_t *evaluator, int predicted_label, int true_label);
void finalize_evaluation(evaluation_t *evaluator);

#endif
