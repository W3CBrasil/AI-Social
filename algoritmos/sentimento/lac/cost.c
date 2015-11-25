#include "cost.h"

/**
READS COSTS. ALL ITEMS ARE STORED IN A GLOBAL ARRAY, WHICH IS
THEN ACCESSED DURING SCORING.
*/
int read_costs(char* costs) {
	__START_TIMER__
	if(costs==NULL) return(0);
	printf("Reading costs\n\n");
	fflush(stdout);
	FILE* file=fopen(costs,"r");
	if(file==NULL) {
		fprintf(stderr,"Cost file %s not found.\n\n", costs);
		exit(-1);
	}
	free(costs);
	int i=0;
	for(i=0;i<MAX_CLASSES || !feof(file);i++) {
		int x;
		fscanf(file, "%d", &x);
		if(x>=MAX_CLASSES) {
			fprintf(stderr,"%d is greater than MAX_CLASSES.\n\n", x);
			exit(-1);
		}
		fscanf(file, "%f\n", &(COSTS[x]));
	}
	if(i!=MAX_CLASSES) {
		fprintf(stderr,"Cost file %s is incomplete.\n\n", costs);
		exit(-1);
	}
	fclose(file);
	__FINISH_TIMER__
	return(0);
}
