#ifndef _CLASSIFICATION_
#define _CLASSIFICATION_

#define LAZY_SUPERVISED_CLASSIFICATION 0
#define LAZY_SEMISUPERVISED_CLASSIFICATION 1
#define LAZY_TRANSDUCTIVE_CLASSIFICATION 2
#define LAZY_ACTIVE_CLASSIFICATION 3

#include "rule.h"
#include "limits.h"
#include "timer.h"
#include "cache.h"
#include "evaluation.h"
#include "test.h"
#include "prediction.h"
#include "score.h"
#include "training.h"
#include "unlabeled.h"

typedef struct {
	short processed;
	int n_rules, diff, total_occurs;
	list_t test;
} active_t;

extern int CAPACITY, MAX_JUDGEMENTS, MIN_RULES, RELATIVE;

int lazy_supervised_classification();
int lazy_transductive_classification();
int lazy_active_classification();
int lazy_semisupervised_classification();

#endif
