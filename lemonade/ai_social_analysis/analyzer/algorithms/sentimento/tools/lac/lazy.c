//trata-se a colecao como uma linguagem que pode ser comprimida por regras de associacao
//trata-se cada classe como uma linguagem, que eh um subconjunto da linguagem que comprime a colecao
//a melhor compressao, embora nao possa ser calculada (kolmogorov complexity), pode ser aproximada (por exemplo, com a dimensao fractal, ou pela ziv-lempel complexity)
//a melhor compressao para cada classe, eh ultizada para realizar a classificacao
//uma linguagem eh similar a outra se as duas sao sobrepostas de alguma forma. o grau de superposicao eh o grau de similaridade.

//modelo baseado em regras eh descritivo e nao generativo. modelos generativos tentam construir o que gerou os dados, e entao com esse modelo pode-se fazer simulacoes.

//regras contraditorias. por exemplo, uma regra a->0(100%) e outra b->1(80%). enfim, como remover a regra b->1. acho que da um teorema assim, "para um dado item a, b, c etc. sempre existe uma regra com 100% de confianca". o ponto eh como ser especifico o suficiente para encontrar tais regras.

#include <getopt.h>
#include <stdlib.h>
#include <string.h>

#include "rule.h"
#include "itemset.h"
#include "timer.h"
#include "cache.h"
#include "core.h"
#include "training.h"
#include "limits.h"
#include "score.h"
#include "test.h"
#include "list.h"
#include "cost.h"

using namespace std;

cache_t CACHE;
itemset_t ITEMSETS[MAX_ITEMS];
rule_t RULES[MAX_RULES];
int META_LEARNING=0, RELATIVE=0, N_RULES=0, N_TESTS=0, N_POINTS=0, N_TRANSACTIONS=0, N_ITEMSETS=0, COUNT_TARGET[MAX_CLASSES], TARGET_ID[MAX_CLASSES], MIN_SIZE=1, MAX_SIZE=MAX_RULE_SIZE, MIN_RULES=1, MAX_JUDGEMENTS=1;
int MIN_COUNT=1;
float MIN_CONF=0.001, MIN_LEVEL=0.5, MIN_SUPP=0.01, FACTOR=1.00, COSTS[MAX_CLASSES];
char *DELIM=" ";

map<int, set<int> > TID_CLASSES;
map<string, int> ITEM_MAP;
map<int, string> SYMBOL_TABLE;
map<string, int> CLASS_NAME;
map<string, long long> PROOF;
set<string> PROC_IDS;
list_t *TEST, *UNLABELED;


int classify(char* training, char* test, char* unlabeled, char* costs, int mode) {
	__START_TIMER__
	read_costs(costs);
	read_training_set(training);
	read_test_set(test);
	if(mode==LAZY_SUPERVISED_CLASSIFICATION) lazy_supervised_classification();
	else if(mode==LAZY_SEMISUPERVISED_CLASSIFICATION) {
		read_unlabeled_set(unlabeled);
		lazy_semisupervised_classification();
		lazy_supervised_classification();
	}
	else if(mode==LAZY_TRANSDUCTIVE_CLASSIFICATION) lazy_transductive_classification();
	else if(mode==LAZY_ACTIVE_CLASSIFICATION) lazy_active_classification();
	for(int i=0;i<N_ITEMSETS;i++) {
		free(ITEMSETS[i].layout);
		free(ITEMSETS[i].list);
	}
	__FINISH_TIMER__
	return(0);
}


int main(int argc, char** argv) {
	int c, mode=LAZY_SUPERVISED_CLASSIFICATION;
	char *file_in=NULL, *file_test=NULL, *file_unlabeled=NULL, *file_costs=NULL;
	CACHE.max_size=CACHE.hits=CACHE.misses=0;
	CACHE.factor=0.1;
	CACHE.locked=0;
	for(int i=0;i<MAX_CLASSES;i++) COSTS[i]=1;
	while((c=getopt(argc,argv,"k:q:f:i:l:g:j:p:a:c:d:s:x:u:t:n:m:e:h"))!=-1) {
		switch(c) {
			case 'i': file_in=strdup(optarg);
				  break;
			case 't': file_test=strdup(optarg);
				  break;
			case 'u': file_unlabeled=strdup(optarg);
				  break;
			case 'q': file_costs=strdup(optarg);
				  break;
			case 'c': MIN_CONF=atof(optarg);
				  break;
			case 'l': MIN_LEVEL=atof(optarg);
				  break;
			case 'e': CACHE.max_size=atoi(optarg);
				  break;
			case 'j': MAX_JUDGEMENTS=atoi(optarg);
				  break;
			case 'g': RELATIVE=1;
				  MIN_SUPP=atof(optarg);
				  break;
			case 's': MIN_COUNT=atoi(optarg);
				  break;
			case 'd': mode=atoi(optarg);
				  break;
			case 'n': MIN_RULES=atoi(optarg);
				  break;
			case 'm': MAX_SIZE=atoi(optarg);
				  if(MAX_SIZE>MAX_RULE_SIZE) MAX_SIZE=MAX_RULE_SIZE;
				  break;
			case 'p': MIN_SIZE=atoi(optarg);
				  if(MAX_SIZE<1) MIN_SIZE=1;
				  break;
			case 'f': FACTOR=atof(optarg);
				  break;
			case 'k': DELIM=strdup(optarg);
				  break;
			default:  exit(1);
		}
	}
	classify(file_in, file_test, file_unlabeled, file_costs, mode);
	for(map<string, long long>::iterator it=PROOF.begin();it!=PROOF.end();it++) printf("time: %s %lf\n", (*it).first.c_str(), (*it).second/(double)1000000);
	printf("\n");
	return(0);
}
