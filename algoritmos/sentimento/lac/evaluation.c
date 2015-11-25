/**
THIS MODULE IMPLEMENTS ALL THE METRICS USED TO EVALUATE A CLASSIFIER, SUCH AS
PRECISION, RECALL AND ACCURACY.
*/

#include "evaluation.h"

/**
INITIALIZE THE EVALUATOR. ALL METRICS STARTS WITH VALUE ZERO.
*/
int initialize_evaluation(evaluation_t* evaluator) {
	__START_TIMER__
	(*evaluator).total_predictions=(int*) malloc(sizeof(int)*MAX_CLASSES);
	(*evaluator).correct_predictions=(int*) malloc(sizeof(int)*MAX_CLASSES);
	(*evaluator).true_labels=(int*) malloc(sizeof(int)*MAX_CLASSES);
	(*evaluator).precision=(float*) malloc(sizeof(float)*MAX_CLASSES);
	(*evaluator).recall=(float*) malloc(sizeof(float)*MAX_CLASSES);
	(*evaluator).f1=(float*) malloc(sizeof(float)*MAX_CLASSES);
	for(int i=0;i<MAX_CLASSES;i++) {
		(*evaluator).total_predictions[i]=(*evaluator).correct_predictions[i]=(*evaluator).true_labels[i]=0;
		(*evaluator).precision[i]=(*evaluator).recall[i]=(*evaluator).f1[i]=0;
	}
	__FINISH_TIMER__
	return(0);
}

/**
UPDATES THE EVALUATOR AFTER THE CLASSIFIER PERFORMS A PREDICTION.
*/
int update_evaluation(evaluation_t *evaluator, int predicted_label, int true_label) {
	__START_TIMER__
	(*evaluator).total_predictions[predicted_label]++;
	(*evaluator).true_labels[true_label]++;
	if(predicted_label==true_label) (*evaluator).correct_predictions[predicted_label]++;
	int total1=0, total2=0;
	for(int i=0;i<MAX_CLASSES;i++) {
		(*evaluator).precision[i]=(*evaluator).correct_predictions[i]/(float)(*evaluator).total_predictions[i];
		(*evaluator).recall[i]=(*evaluator).correct_predictions[i]/(float)(*evaluator).true_labels[i];
		(*evaluator).f1[i]=(2*(*evaluator).precision[i]*(*evaluator).recall[i])/(float)((*evaluator).precision[i]+(*evaluator).recall[i]);
		total1+=(*evaluator).correct_predictions[i];
		total2+=(*evaluator).true_labels[i];
	}
	(*evaluator).acc=total1/(float)total2;
	(*evaluator).mf1=0;
	int k=0;
	for(int i=0;i<MAX_CLASSES;i++) {
		if(!isnan((*evaluator).f1[i])) {
			(*evaluator).mf1+=(*evaluator).f1[i];
			k++;
		}
	}
	(*evaluator).mf1/=(float)k;
	__FINISH_TIMER__
	return(0);
}

/**
FINALIZE THE EVALUATOR AND RELEASE THE MEMORY.
*/
void finalize_evaluation(evaluation_t *evaluator) {
	__START_TIMER__
	free((*evaluator).total_predictions);
	free((*evaluator).correct_predictions);
	free((*evaluator).true_labels);
	free((*evaluator).precision);
	free((*evaluator).recall);
	free((*evaluator).f1);
	__FINISH_TIMER__
}
