/**
THIS MODULE IMPLEMENTS OPERATIONS OF READING/PARSING THE UNLABELED SET, AND PROJECTING THE TRAINING DATA
ACCORDING TO A UNLABELED SET.
*/

#include "unlabeled.h"

/**
READS THE UNLABELED SET. STORES ALL POINTS IN A GLOBAL MAP.
*/
int read_unlabeled_set(char* unlabeled) {
	__START_TIMER__
	int target=0;
	FILE* file=fopen(unlabeled, "r");
	if(file==NULL) {
		fprintf(stderr,"Unlabeled file %s not found.\n\n", unlabeled);
		exit(-1);
	}
	set<int> instance;
	UNLABELED=(list_t*)malloc(sizeof(list_t));
	while(1) {
		char line[200*KB];
		fgets(line, 200*KB, file);
		if(feof(file)) break;
		N_POINTS++;
		target=-1;
		char* item=strtok(line, " \n");
		item=strtok(NULL, " \n");
		while(item!=NULL) {
			if(target==-1 && CLASS_NAME.find(item)!=CLASS_NAME.end()) target=(int)CLASS_NAME[item];
			if(ITEM_MAP.find(item)==ITEM_MAP.end()) {
				ITEM_MAP[item]=N_ITEMSETS;
				//int attr_type=NONE;
				//if(item[0]==A1) attr_type=1;
				//else if(item[0]==A2) attr_type=2;
				//else if(item[0]==A3) attr_type=4;
				//else if(item[0]==A4) attr_type=8;
				ITEMSETS[N_ITEMSETS].count=0;
				ITEMSETS[N_ITEMSETS].size=1;
				//ITEMSETS[N_ITEMSETS].evidence=attr_type;
				ITEMSETS[N_ITEMSETS].layout=(int*)malloc(sizeof(int));
				ITEMSETS[N_ITEMSETS].layout[0]=N_ITEMSETS;
				SYMBOL_TABLE[N_ITEMSETS]=strdup(item);
				++N_ITEMSETS;
			}
			if(CLASS_NAME.find(item)==CLASS_NAME.end()) instance.insert(ITEM_MAP[item]);
			item=strtok(NULL, " \n");
			if(item==NULL) break;
		}
		if(N_POINTS==1) {
			UNLABELED->instance=(int*)malloc(sizeof(int)*instance.size());
			UNLABELED->size=0;
			for(set<int>::iterator it=instance.begin();it!=instance.end();it++) UNLABELED->instance[UNLABELED->size++]=*it;
			UNLABELED->label=target;
			UNLABELED->next=NULL;
		}
		else {
	      		list_t* q=UNLABELED;
			while(q->next!=NULL) q=q->next;
			list_t* t=(list_t*)malloc(sizeof(list_t));
			t->instance=(int*)malloc(sizeof(int)*instance.size());
			t->size=0;
			for(set<int>::iterator it=instance.begin();it!=instance.end();it++) t->instance[t->size++]=*it;
			t->label=target;
			t->next=NULL;
			q->next=t;
		}
		instance.clear();
	}
	fclose(file);
	__FINISH_TIMER__
	return(0);
}
