#ifndef _LIMITS_
#define _LIMITS_

#define KB 1024
#define MB 1024*1024
#define GB 1024*1024*1024

#define MAX_CLASSES 3
#define MAX_RULE_SIZE 50
#define MAX_RULES 1*KB
#define MAX_ITEMS 1*MB
#define MAX_TIME_PER_TEST 1.00

extern int MIN_COUNT, MIN_SIZE, MAX_SIZE;
extern float MIN_CONF, MIN_LEVEL, MIN_SUPP, FACTOR;

#endif
