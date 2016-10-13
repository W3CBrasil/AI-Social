/**
THIS MODULE IMPLEMENTS A BASIC CACHING MECHANISM FOR STORING RULES.
*/

#include "cache.h"

/**
A RULE IS INSERTED INTO THE cache ACCORDING TO ITS LAYOUT (i.e., THE SET OF ITEMS IN THE RULE).
IF THE cache IS FULL, LESS FREQUENT RULES ARE REMOVED FIRST.
*/
int insert_into_cache(cache_t& cache, int* layout, int size, content_t cont) {
	__START_TIMER__
	if(cache.max_size>0 && cache.locked==0) {
		if(cache.content.size()>=cache.max_size) {
			int n_discards=(int)(cache.factor*cache.max_size);
			map<set<int>, content_t>::iterator begin=cache.content.end();
			for(int i=0;i<n_discards;i++) begin--;
			cache.content.erase(begin, cache.content.end());
			__FINISH_TIMER__
			return(0);
		}
		set<int> key;
		for(int i=0;i<size;i++) key.insert(layout[i]);
		//memmove((content_t*)cache.content[key], (content_t*)cont, sizeof(cont));
		cache.content[key]=cont;
		//cache.content[key].status=cont.status;
		//cache.content[key].evidence=cont.evidence;
		//cache.content[key].count=cont.count;
		//cache.content[key].ant_count=cont.ant_count;
		//for(int i=0;i<MAX_CLASSES;i++) {
		//	cache.content[key].cons_count[i]=cont.cons_count[i];
		//}
		key.clear();
	}
	__FINISH_TIMER__
	return cache.content.size();
}

/**
IF THE RULE IS STORED IN THE cache, IT IS RETURNED (i.e., status=1). OTHERWISE, THE
RETURNED CONTENT IS EMPTY (i.e., status=0).
*/
content_t get_from_cache(cache_t& cache, int* layout, int size) {
	__START_TIMER__
	content_t cont;
	cont.status=0;
	if(cache.max_size>0 && cache.locked==0) {
		set<int> key;
		for(int i=0;i<size;i++) key.insert(layout[i]);
		if(cache.content.find(key)!=cache.content.end()) {
			cache.hits++;
			//memmove((content_t*)cont, (content_t*)cache.content[key], sizeof(cache.content[key]));
			cont=cache.content[key];
			//cont.status=cache.content[key].status;
			//cont.evidence=cache.content[key].evidence;
			//cont.count=cache.content[key].count;
			//cont.ant_count=cache.content[key].ant_count;
			//for(int i=0;i<MAX_CLASSES;i++) {
			//	cont.cons_count[i]=cache.content[key].cons_count[i];
			//}
		}
		else {
			cache.misses++;
			cont.status=0;
			cont.evidence=-1;
			cont.ant_count=0;
			cont.count=0;
			for(int i=0;i<MAX_CLASSES;i++) cont.cons_count[i]=0;
		}
		key.clear();
	}
	__FINISH_TIMER__
	return cont;
}
