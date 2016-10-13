#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(int argc, char** argv) {
	float sum_sq_x=0;
	float  sum_sq_y=0;
	float sum_coproduct=0;
	float x[1000], y[1000];

	FILE* f=fopen(argv[1], "r");
	fscanf(f, "%f %f", &x[0], &y[0]);
	int k=0;
	while(!feof(f)) {
		k++;
		fscanf(f, "%f %f", &x[k], &y[k]);
	}
	fclose(f);

	float mean_x=x[0];
	float mean_y=y[0];
	int i=0;
	for(i=1;i<k;i++) {
		float sweep=(i-1.0)/i;
		float delta_x=x[i]-mean_x;
		float delta_y=y[i]-mean_y;
		sum_sq_x+=delta_x*delta_x*sweep;
		sum_sq_y+=delta_y*delta_y*sweep;
		sum_coproduct+=delta_x*delta_y*sweep;
		mean_x+=delta_x/i;
		mean_y+=delta_y/i;
	}
	float pop_sd_x=sqrt(sum_sq_x/k);
	float pop_sd_y=sqrt(sum_sq_y/k);
	float cov_x_y=sum_coproduct/k;
	float correlation=cov_x_y/(pop_sd_x*pop_sd_y);
	printf("%f\n\n", correlation);
	return(0);
}
