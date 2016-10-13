#ifndef _COST_
#define _COST_

#include <stdio.h>
#include <stdlib.h>

#include "timer.h"
#include "limits.h"

using namespace std;

extern float COSTS[MAX_CLASSES];

int read_costs(char* costs);

#endif
