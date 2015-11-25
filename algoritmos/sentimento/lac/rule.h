#ifndef _RULE_
#define _RULE_

#include <math.h>
#include <map>
#include <string>

#include "itemset.h"
#include "cache.h"
#include "training.h"
#include "limits.h"
#include "criteria.h"
#include "timer.h"

using namespace std;

typedef struct {
	int ant[MAX_RULE_SIZE], label, size, evidence, id, count;
	float supp, conf;
} rule_t;

extern rule_t RULES[MAX_RULES];
extern int N_RULES, TARGET_ID[MAX_CLASSES], COUNT_TARGET[MAX_CLASSES];
extern map<int, string> SYMBOL_TABLE;

void print_rule(rule_t rule);
void reset_rule(int id);
void reset_rules();
void induce_rules(int* items, int n_items, int* count_target, int level, int projection_size, int min_count, const set<int>& relevant_classes);
int mark_rules(int id, int h);

#endif
