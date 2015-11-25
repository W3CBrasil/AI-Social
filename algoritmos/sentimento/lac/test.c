/**
THIS MODULE IMPLEMENTS OPERATIONS OF READING/PARSING THE TEST SET, AND PROJECTING THE TRAINING DATA
ACCORDING TO A TEST SET.
*/

#include "test.h"

/**
GIVEN SOME ITEMS, PROJECTS THE ORIGINAL TRAINING DATA TO THE INSTANCES CONTAINING AT LEAST ONE SPECIFIED ITEM.
*/
int project_training(int* items, int n_items, int* n_transactions, int* n_classes, set<int>& relevant_classes) {
   __START_TIMER__
   relevant_classes.clear();
   short* temp=(short*)malloc(sizeof(short)*(N_TRANSACTIONS+1));
   for(int i=0;i<N_TRANSACTIONS+1;i++) temp[i]=0;
   for(int i=0;i<n_items;i++) {
      for(int j=0;j<ITEMSETS[items[i]].count;j++) temp[ITEMSETS[items[i]].list[j]]=1;
   }
   *n_transactions=0;
   *n_classes=0;
   int freq[MAX_CLASSES];
   for(int i=0;i<MAX_CLASSES;i++) freq[i]=0;
   for(int i=1;i<N_TRANSACTIONS+1;i++) {
      if(temp[i]==1) {
         (*n_transactions)++;
         for(set<int>::iterator j=TID_CLASSES[i].begin();j!=TID_CLASSES[i].end();j++) {
            freq[*j]++;
            if(freq[*j]>=1) relevant_classes.insert(*j);
         }
      }
   }
   (*n_classes)=relevant_classes.size();
   free(temp);
   __FINISH_TIMER__
   return(0);
}

/**
READS THE TEST SET. STORES ALL TEST INSTANCES IN A GLOBAL MAP.
*/
int read_test_set(char* test) {
	__START_TIMER__
	printf("Reading test set ");
	fflush(stdout);
	int target=0;
	FILE* file=fopen(test, "r");
	if(file==NULL) {
		fprintf(stderr,"Test file %s not found.\n\n",test);
		exit(-1);
	}
	free(test);
	int n_lines=0;
	char line[100*KB];
	while(1) {
		fgets(line, 100*KB, file);
		if(feof(file)) break;
		n_lines++;
	}
	rewind(file);
	set<int> instance;
	TEST=(list_t*)malloc(sizeof(list_t));
	list_t* last_test=NULL;
	while(1) {
		//char line[100*KB];
		fgets(line, 100*KB, file);
		if(feof(file)) break;
		target=-1;
		char *id=strtok(line, " \t\n");
		if(id==NULL) break; //empty line
		N_TESTS++;
		char* item=strtok(NULL, " \t\n");
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
				ITEMSETS[N_ITEMSETS].list=(int*)malloc(sizeof(int));
				ITEMSETS[N_ITEMSETS].layout=(int*)malloc(sizeof(int));
				ITEMSETS[N_ITEMSETS].layout[0]=N_ITEMSETS;
				SYMBOL_TABLE[N_ITEMSETS]=strdup(item);
				++N_ITEMSETS;
			}
			if(CLASS_NAME.find(item)==CLASS_NAME.end()) instance.insert(ITEM_MAP[item]);
			item=strtok(NULL, " \t\n");
		}
		if(N_TESTS==1) {
			TEST->instance=(int*)malloc(sizeof(int)*instance.size());
			TEST->size=0;
			for(set<int>::iterator it=instance.begin();it!=instance.end();it++) TEST->instance[TEST->size++]=*it;
			TEST->id=strdup(id);
			TEST->label=target;
  			TEST->next=NULL;
			last_test=TEST;
		}
		else {
			list_t* q=last_test;
			while(q->next!=NULL) q=q->next;
			list_t* t=(list_t*)malloc(sizeof(list_t));
			t->instance=(int*)malloc(sizeof(int)*instance.size());
			t->size=0;
			for(set<int>::iterator it=instance.begin();it!=instance.end();it++) t->instance[t->size++]=*it;
			t->id=strdup(id);
			t->label=target;
			t->next=NULL;
			q->next=t;
			last_test=t;
		}
		instance.clear();
	}
	printf("[done]\n");
	fclose(file);
	__FINISH_TIMER__
	return(0);
}
