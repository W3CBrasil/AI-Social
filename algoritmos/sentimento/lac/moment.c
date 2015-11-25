#include <stdio.h>
#include <math.h>

void moment(float data[], int n, float *ave, float *adev, float *sdev, float *var, float *min, float *max) {
        int j;
        float ep=0,s,p;
	*min=10000000;
	*max=0;

        if(n > 1) {
                s=0;
                for(j=0;j<n;j++) s+=data[j];
                *ave=s/(float)n;
                *adev=(*var)=0;
                for(j=0;j<n;j++) {
                        *adev+=fabs(s=data[j]-(*ave));
                        *var+=(p=s*s);
			if(data[j]>=*max) *max=data[j];
			if(data[j]<=*min) *min=data[j];
                }
                *adev/=n;
                *var=(*var-ep*ep/(float)n)/(float)(n-1);
                *sdev=sqrt(*var);
        }
}

int main(int argc, char** argv) {
	float data[1000];
	for(int i=0;i<1000;i++) data[i]=0;
	int n=0;
	FILE* f=stdin;
	fscanf(f, "%f", &(data[n]));
	while(!feof(f)) {
		n++;
		fscanf(f, "%f", &(data[n]));
	}
	float ave, adev, sdev, var, min, max;
	moment(data, n, &ave, &adev, &sdev, &var, &min, &max);
	printf("%f %f %f\n", ave, min, max);
	return(0);
}
