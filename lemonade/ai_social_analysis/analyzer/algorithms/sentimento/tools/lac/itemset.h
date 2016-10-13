#ifndef _ITEMSET_
#define _ITEMSET_

#include <map>
#include <string>
#include <stdlib.h>

#include "limits.h"
#include "timer.h"

typedef struct {
	short size;
	int count, *list, *layout;
}itemset_t;


extern itemset_t ITEMSETS[MAX_ITEMS];
extern int N_ITEMSETS;
extern map<int, string> SYMBOL_TABLE;

int release_itemset(itemset_t a);
itemset_t create_itemset(int* its, int size);


#endif
