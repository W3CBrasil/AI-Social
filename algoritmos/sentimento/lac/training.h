#ifndef _TRAINING_
#define _TRAINING_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <set>
#include <map>

#include "itemset.h"
#include "limits.h"
#include "timer.h"

using namespace std;

extern map<int, set<int> > TID_CLASSES;
extern int CLASS_MAP[MAX_CLASSES], META_LEARNING, N_TRANSACTIONS, TARGET_ID[MAX_CLASSES], COUNT_TARGET[MAX_CLASSES];
extern char *DELIM;
extern map<string, int> CLASS_NAME;
extern map<int, string> SYMBOL_TABLE;
extern map<string, int> ITEM_MAP;
extern set<string> PROC_IDS;

int read_training_set(char* training);

#endif
