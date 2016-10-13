#ifndef _UNLABELED_
#define _UNLABELED_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <set>

#include "limits.h"
#include "itemset.h"
#include "timer.h"
#include "list.h"

using namespace std;

extern int N_POINTS;
extern map<string, int> CLASS_NAME;
extern map<string, int> ITEM_MAP;
extern map<int, string> SYMBOL_TABLE;
extern list_t* UNLABELED;


int read_unlabeled_set(char* unlabeled);

#endif
