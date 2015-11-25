#ifndef _CRITERIA_
#define _CRITERIA_

#include <math.h>

#define MAX(a,b) ((a)<(b)?(b):(a))
#define MIN(a,b) ((a)<(b)?(a):(b))

#define CONFIDENCE 0 
#define ADDEDVALUE 1
#define CERTAINTY 2
#define YULESY 3
#define YULESQ 4
#define SS 5
#define SUPPORT 6
#define RELACC 7
#define SATISFACTION 8
#define NOVELTY 9
#define PHI 10
#define STRENGTH 11
#define ODDS 12
#define COSINE 13
#define GAIN 14
#define LAPLACE 15
#define WEIGHTEDCHI2 16

#define INTEREST 17
#define SHAPIRO 18
#define KLOSGEN 19
#define COVERAGE 20
#define JACCARD 21
#define KAPPA 22
#define RELNREG 23
#define JMEASURE 24
#define RELSENSITIVITY 25
#define RELSPECIFICITY 26
#define ONE 27
#define WRACC 28
#define WRNEGREL 29
#define WRSENSITIVITY 30
#define WRSPECIFICITY 31
#define CONVICTION 32

#define F11 (rule.count)
#define F10 (cont.ant_count-rule.count)
#define F01 (COUNT_TARGET[i]-rule.count)
#define FP0 (N_TRANSACTIONS-COUNT_TARGET[i])
#define F00 (FP0-F10)
#define FP1 (N_TRANSACTIONS-FP0)
#define F1P (F11+F10)
#define F0P (F01+F00)

#define P_A (F1P/(float)N_TRANSACTIONS)
#define P_B (FP1/(float)N_TRANSACTIONS)
#define P_NA (F0P/(float)N_TRANSACTIONS)
#define P_NB (FP0/(float)N_TRANSACTIONS)
#define P_AB (F11/(float)N_TRANSACTIONS)
#define P_ANB (F10/(float)N_TRANSACTIONS)
#define P_NAB (F01/(float)N_TRANSACTIONS)
#define P_NANB (F00/(float)N_TRANSACTIONS)
#define P_AgB ((0.0000001+P_AB)/(float)(0.0000001+P_B))
#define P_BgA ((0.0000001+P_AB)/(float)(0.0000001+P_A))
#define P_NAgB ((0.0000001+P_NAB)/(float)(0.0000001+P_B))
#define P_NBgA ((0.0000001+P_ANB)/(float)(0.0000001+P_A))
#define P_BgNA ((0.0000001+P_NAB)/(float)(0.0000001+P_NA))
#define P_AgNB ((0.0000001+P_ANB)/(float)(0.0000001+P_NB))
#define P_NBgNA ((0.0000001+P_NANB)/(float)(0.0000001+P_NA))
#define P_NAgNB ((0.0000001+P_NANB)/(float)(0.0000001+P_NB))


#define COMPUTE_SUPPORT (P_AB)
//#define COMPUTE_CONFIDENCE (P_AB)
#define COMPUTE_COVERAGE (P_A)
#define COMPUTE_CONFIDENCE (P_BgA)
#define COMPUTE_CONVICTION ((1-P_B)/(float)(1-P_BgA))
#define COMPUTE_KLOSGEN (sqrt(MAX(0.0000001, F11*F11-FP1*F1P))/(float)(0.0000001+N_TRANSACTIONS*F1P))
#define COMPUTE_CERTAINTY ((P_BgA-P_B)/(float)(0.0000001+1-P_B))
//#define COMPUTE_CONFIDENCE ((P_BgA-P_B)/(float)(0.0000001+1-P_B))
#define COMPUTE_SPECIFICITY (P_NAgNB)
#define COMPUTE_COSINE (P_AB/(float)sqrt(0.0000001+P_A*P_B))
#define COMPUTE_NOVELTY (P_AB-P_A*P_B)
#define COMPUTE_SS ((P_AgB*P_BgA)/(float)P_AgNB)
#define COMPUTE_SATISFACTION (P_B*(P_NB-P_NBgA)/(float)P_NB);
#define COMPUTE_PHI (((P_AB-P_A*P_B)/(float)sqrt(0.0000001+P_A*P_B*(1-P_A)*(1-P_B))+1)/(float)2)
#define COMPUTE_LAPLACE ((rule.count+1)/(float)(cont.ant_count+MAX_CLASSES))
#define COMPUTE_WRNEGREL (P_NA*(P_NBgNA-P_NB))
#define COMPUTE_WRSENSITIVITY (P_B*(P_AgB-P_A))
#define COMPUTE_WRSPECIFICITY (P_NB*(P_NAgNB-P_NA))
#define COMPUTE_RELSENSITIVITY (P_AgB-P_A)
#define COMPUTE_RELSPECIFICITY (P_NAgNB-P_NA)
#define COMPUTE_WRACC (P_A*(P_BgA-P_B))
#define COMPUTE_RELACC (P_BgA-P_B)
//#define COMPUTE_CONFIDENCE (P_BgA-P_B)
#define COMPUTE_RELNREG (P_NBgNA-P_NB)
#define COMPUTE_ODDS ((0.0000001+P_AB*P_NANB)/(float)(0.0000001+P_ANB*P_NAB))
#define COMPUTE_YULESQ ((P_AB*P_NANB-P_ANB*P_NAB)/(float)(0.000001+P_AB*P_NANB+P_ANB*P_NAB))
//#define COMPUTE_CONFIDENCE ((P_AB*P_NANB-P_ANB*P_NAB)/(float)(0.000001+P_AB*P_NANB+P_ANB*P_NAB))
#define COMPUTE_YULESY ((sqrt(P_AB*P_NANB)-sqrt(P_ANB*P_NAB))/(float)(0.000001+sqrt(P_AB*P_NANB)+sqrt(P_ANB*P_NAB)))
#define COMPUTE_KAPPA ((P_AB+P_NANB-P_A*P_B-P_NA*P_NB)/(float)(1-P_A*P_B-P_NA*P_NB))
#define COMPUTE_JMEASURE (P_AB*log2(P_BgA/(float)P_B)+P_ANB*log2(P_NBgA/(float)P_NB))
#define COMPUTE_GINI (P_A*(P_BgA*P_BgA+P_NBgA*P_NBgA)+P_NA*(P_BgNA*P_BgNA+P_NBgNA*P_NBgNA)-P_B*P_B-P_NB*P_NB)
#define COMPUTE_INTEREST ((0.0000001+P_AB)/(float)(0.0000001+P_A*P_B))
#define COMPUTE_SHAPIRO (P_AB-P_A*P_B)
#define COMPUTE_ADDEDVALUE (P_BgA-P_B)
//#define COMPUTE_CONFIDENCE (P_BgA-P_B)
#define COMPUTE_STRENGTH (((0.0000001+P_AB+P_NANB)/(float)(0.0000001+P_A*P_B+P_NA*P_NB))*((0.0000001+1-P_A*P_B-P_NA*P_NB)/(float)(0.0000001+1-P_AB-P_NANB)))
//#define COMPUTE_CONFIDENCE (((0.0000001+P_AB+P_NANB)/(float)(0.0000001+P_A*P_B+P_NA*P_NB))*((0.0000001+1-P_A*P_B-P_NA*P_NB)/(float)(0.0000001+1-P_AB-P_NANB)))
#define COMPUTE_JACCARD ((0.0000001+P_AB)/(float)(0.0000001+P_A+P_B-P_AB))
#define COMPUTE_GAIN (rule.count*(log2(rule.count/(float)cont.ant_count)-log2(COUNT_TARGET[i]/(float)N_TRANSACTIONS)))
#define COMPUTE_ONE (1)

#define MAX_CHI2 ((MIN(cont.ant_count, COUNT_TARGET[i])-(cont.ant_count*COUNT_TARGET[i])/(float)N_TRANSACTIONS)*(MIN(cont.ant_count, COUNT_TARGET[i])-(cont.ant_count*COUNT_TARGET[i])/(float)N_TRANSACTIONS)*N_TRANSACTIONS*(1/(float)(cont.ant_count*COUNT_TARGET[i])+1/(float)(cont.ant_count*(N_TRANSACTIONS-COUNT_TARGET[i]))+1/(float)((N_TRANSACTIONS-cont.ant_count)*COUNT_TARGET[i])+1/(float)((N_TRANSACTIONS-cont.ant_count)*(N_TRANSACTIONS-COUNT_TARGET[i]))))
#define CHI2 ((F11*F00-F10*F01)*(F11*F00-F10*F01)*(F11+F10+F01+F00)/(float)(F11+F10)*(F01+F00)*(F10+F00)*(F11+F01))
#define COMPUTE_WEIGHTEDCHI2 (CHI2*CHI2/(float)MAX_CHI2)


#endif
