#ifndef _TIMER_
#define _TIMER_

#include <map>
#include <string>
#include <sys/timeb.h>
#include <time.h>
#include <sys/time.h>

using namespace std;

#define __START_TIMER__   struct timeval start; gettimeofday(&start, NULL);
#define __FINISH_TIMER__  struct timeval end; gettimeofday(&end, NULL); PROOF[__FUNCTION__]+=(end.tv_sec-start.tv_sec)*1000000+end.tv_usec-start.tv_usec;

extern map<string, long long> PROOF;

#endif
