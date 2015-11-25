/**
THIS MODULE IMPLEMENTS ALL OPERATIONS REGARDING ITEMSET CREATION AND MAINTENANCE.

*/

#include "itemset.h"

//static int layout_cmp(const void* a, const void* b) {
//	int* i=(int*)a;
//	int* j=(int*)b;
//	if(*i<*j) {
//		if(ITEMSETS[*j].evidence==CLASS) return(1);
//		return(-1);
//	}
//	if(*i>*j) {
//		if(ITEMSETS[*i].evidence==CLASS) return(-1);
//		return(1);
//	}
//	return(0);
//}

/**
RELEASE THE MEMORY THAT WAS ALLOCATED TO AN ITEMSET. ID THE ITEMSET IS A CLASS, THEN
IT CANNOT BE RELEASED.
*/
int release_itemset(itemset_t a) {
	__START_TIMER__
	if(a.size>1) {
		free(a.layout);
		free(a.list);
	}
	__FINISH_TIMER__
	return(1);
}

/**
CONSTRUCTS THE LAYOUT OF AN ITEMSET.
*/
itemset_t get_THE_union(itemset_t a, itemset_t b) {
	__START_TIMER__
	itemset_t result;
	result.layout=(int*) malloc(sizeof(int)*(a.size+b.size));
	result.size=0;
	for(int i=0;i<a.size;i++) result.layout[result.size++]=a.layout[i];
	for(int i=0;i<b.size;i++) {
		short contains=0;
		for(int j=0;j<result.size;j++) {
			if(result.layout[j]==b.layout[i]) {
				contains=1;
				break;
			} 
		}
		if(!contains) result.layout[result.size++]=b.layout[i];
	}
	result.layout=(int*)realloc(result.layout, sizeof(int)*result.size);
	//qsort((int*)result.layout, result.size, sizeof(int), layout_cmp);
	__FINISH_TIMER__
	return(result);
}

/**
COMPUTES THE INTERSECTION OF LISTS OF TIDS.
*/
itemset_t get_THE_intersection(itemset_t a, itemset_t b) {
	__START_TIMER__
	itemset_t result=get_THE_union(a, b);
	result.count=0;
//	if(a.evidence==CLASS) result.evidence=b.evidence;
//	else if(b.evidence==CLASS) result.evidence=a.evidence;
//	else if(a.evidence==b.evidence) result.evidence=a.evidence;
//	else result.evidence=a.evidence|b.evidence;
	if(a.count!=0 && b.count!=0) {
		if(a.count>b.count) result.list=(int*) malloc(sizeof(int)*b.count);
		else result.list=(int*) malloc(sizeof(int)*a.count);
		for(int i=0, j=0;i<a.count && j<b.count;) {
			if(a.list[i]>b.list[j]) j++;
			else if(a.list[i]==b.list[j]) {
				result.list[result.count++]=a.list[i];
				j++;
				i++;
			}
			else i++;
		}
	}
	else result.list=(int*) malloc(sizeof(int));
	__FINISH_TIMER__
	return(result);
}

/**
CREATES AN ITEMSET USING THE k-WAY INTERSECTION APPROACH.
*/
itemset_t create_itemset(int* its, int size) {
	__START_TIMER__
	if(size==1) {
		__FINISH_TIMER__
		return(ITEMSETS[its[0]]);
	}
	itemset_t result[2];
	result[0]=get_THE_intersection(ITEMSETS[its[0]], ITEMSETS[its[1]]);
	result[1].size=0;
	for(int i=2;i<size;i++) {
		result[(i+1)%2]=get_THE_intersection(result[i%2], ITEMSETS[its[i]]);
		release_itemset(result[i%2]);
	}
	__FINISH_TIMER__
	return(result[size%2]);
}
