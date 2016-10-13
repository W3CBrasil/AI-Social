//expresso: ver o circuito minimo que faz o numero de 1's, assim eu consigo contar o suporte bitwise
//implementar o principio da inclusao-exclusao


/**
THIS MODULE IMPLEMENTS OPERATIONS REGARDING RULE CREATION AND MAINTENANCE.
*/

#include "rule.h"

/**
COMPARES TWO RULES.
*/
static int rule_cmp(const void* a, const void* b) {
	rule_t* i=(rule_t*)a;
	rule_t* j=(rule_t*)b;
	if((*i).conf<(*j).conf) return(1);
	if((*i).conf>(*j).conf) return(-1);
	return(0);
}

/**
GENERATES COMBINATIONS OF ITEMS.
*/
template <class elem_t> bool next_combination(elem_t n_begin, elem_t n_end, elem_t r_begin, elem_t r_end) {
	bool boolmarked=false;
	elem_t r_marked=0;
	elem_t n_it1=n_end;
	--n_it1;
	elem_t tmp_r_end=r_end;
	--tmp_r_end;
	elem_t tmp_r_begin=r_begin;
	--tmp_r_begin;

	for(elem_t r_it1=tmp_r_end; r_it1!=tmp_r_begin ; --r_it1,--n_it1) {
		if(*r_it1==*n_it1 ) {
			if(r_it1!=r_begin) {
				boolmarked=true;
				r_marked=(--r_it1);
				++r_it1;
				continue;
			}
			else return false;
		}
		else {
			if(boolmarked==true) {
				elem_t n_marked=0;
				for(elem_t n_it2=n_begin;n_it2!=n_end;++n_it2) {
					if(*r_marked==*n_it2) {
						n_marked=n_it2;
						break;
					}
				}
				elem_t n_it3=++n_marked;    
				for(elem_t r_it2=r_marked;r_it2!=r_end;++r_it2,++n_it3) {
					*r_it2=*n_it3;
				}
				return true;
			}
			for(elem_t n_it4=n_begin; n_it4!=n_end; ++n_it4) {
				if(*r_it1==*n_it4) {
					*r_it1=*(++n_it4);
					return true;    
				}
			}
		}
	}  
	return true;
}

/**
PRINTS A RULE.
*/
void print_rule(rule_t rule) {
	__START_TIMER__
	for(int i=0;i<rule.size-1;i++) printf("%s-", SYMBOL_TABLE[rule.ant[i]].c_str());
	printf("->%s ", SYMBOL_TABLE[TARGET_ID[rule.label]].c_str());
	printf("size= %d count= %d supp= %f conf= %f", rule.size-1, rule.count, rule.supp, rule.conf);
	__FINISH_TIMER__
}

/**
RESETS A RULE FROM THE GLOBAL ARRAY OF RULES. THE SIZE OF THE ARRAY IS AUTOMATICALY REDUCED, THUS
HAVE CAUTION WHEN CALLING THIS FUNCTION INSIDE A LOOP.
*/
void reset_rule(int id) {
	__START_TIMER__
	if(id<N_RULES) {
		N_RULES--;
		RULES[id]=RULES[N_RULES];
	}
	__FINISH_TIMER__
}

/**
RESETS ALL RULES IN THE GLOBAL ARRAY OF RULES.
*/
void reset_rules() {
	__START_TIMER__
	//for(int i=0;i<N_RULES;i++) {
	//	RULES[i].label=-1;
	//	RULES[i].size=0;
	//	RULES[i].count=0;
	//	for(int j=0;j<MAX_RULE_SIZE;j++) RULES[i].ant[j]=0;
	//	RULES[i].supp=0;
	//	RULES[i].conf=0;
	//}
	N_RULES=0;
	__FINISH_TIMER__
}

/**
INDUCES RULES SATISFYING SOME CONSTRAINTS, SUCH AS ITEMS, SIZE AND SUPPORT/CONFIDENCE.
CRITERIA ARE COMPUTED BASED ON THE PROJECTED TRAINING DATA, WHICH IS SPECIFIED IN tids/projection_size.
INDUCING A RULE INVOLVES DATA SCANS AND ITEMSET CREATION, THUS, BEFORE GENERATING A RULE,
A CACHE IS ACCESSED IN ORDER TO FIND THE RULE, POSSIBLY AVOIDING REWORK.
AT THE END, ALL INDUCED RULES ARE SORTED AND STORED IN A GLOBAL ARRAY OF RULES.
*/
void induce_rules(int* items, int n_items, int* count_target, int level, int projection_size, int min_count, const set<int>& relevant_classes) {
	if(level>n_items) return;
	__START_TIMER__
	struct timeval t1;
	gettimeofday(&t1, NULL);
	double total_time=0;
	int* cb1=(int*)malloc(sizeof(int)*level);
	int* cb2=(int*)malloc(sizeof(int)*(level+1));
	for(int i=0;i<level;i++) cb1[i]=items[i];
	do {
		for(int i=0;i<level;i++) cb2[i]=cb1[i];
		itemset_t ant;
		content_t cont;
		cont=get_from_cache(CACHE, cb1, level);
		if(cont.status==0) {
			ant=create_itemset(cb1, level);
			cont.ant_count=ant.count;
			//cont.evidence=ant.evidence;
		}
		for(int i=0;i<MAX_CLASSES;i++) {
			itemset_t cons;
			cons.size=1;
			if(COUNT_TARGET[i]==0) {
				cont.cons_count[i]=0;
			}
			else {
				cb2[level]=TARGET_ID[i];
				if(cont.status==0) {
					cons=create_itemset(cb2, level+1);
					cont.cons_count[i]=cons.count;
				}
			}
			rule_t rule;
			rule.count=cont.cons_count[i];
			rule.conf=COMPUTE_CONFIDENCE;
			rule.supp=COMPUTE_SUPPORT;
			if(rule.count>=min_count && rule.conf>=MIN_CONF) {
				RULES[N_RULES].count=rule.count;
				RULES[N_RULES].supp=rule.supp;
				RULES[N_RULES].conf=rule.conf;
				RULES[N_RULES].id=N_RULES;
				//RULES[N_RULES].evidence=cont.evidence;
				RULES[N_RULES].size=level+1;
				RULES[N_RULES].label=i;
				for(int j=0;j<level;j++) RULES[N_RULES].ant[j]=cb1[j];
				if(N_RULES>=MAX_RULES-1) {
					release_itemset(cons);
					break;
				}
				N_RULES++;
			}
			if(cont.status==0) release_itemset(cons);
		}
		if(cont.status==0) {
			cont.status=1;
			insert_into_cache(CACHE, cb1, level, cont);
			release_itemset(ant);
		}
		struct timeval t2;
		gettimeofday(&t2, NULL);
		total_time=((t2.tv_sec-t1.tv_sec)*1000000+t2.tv_usec-t1.tv_usec)/(double)1000000;
	}
	while(N_RULES<MAX_RULES-1 && next_combination(items,items+n_items,cb1,cb1+(level)));
	//while(N_RULES<MAX_RULES-1 && next_combination(items,items+n_items,cb1,cb1+(level)) && total_time<MAX_TIME_PER_TEST);
	free(cb1);
	free(cb2);
	qsort((rule_t*) RULES, N_RULES, sizeof(rule_t), rule_cmp);
	__FINISH_TIMER__
}
