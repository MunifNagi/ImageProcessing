#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#define PI 3.1415927



typedef struct
{
	int len;
	float* real;
	float* imag;
} complexS, *complexP;

bool write_output(FILE *out, complexS in);

void dft1D(FILE *in, int dir, FILE *out) {

    int width, height;
    fscanf(in, "%d %d", &width, &height);

    int u, x, N = height;
    float c, s, real, imag;
    complexS f, F;
    f.len = height;
    f.real = malloc(sizeof(float)*N);
    f.imag = malloc(sizeof(float)*N);

    F.len = height;
    F.real = malloc(sizeof(float)*N);
    F.imag = malloc(sizeof(float)*N);
if (dir == 0){
	for (int i = 0; i < N; i++) {
                fscanf(in, "%f %f", &f.real[i], &f.imag[i]);
    }
    for(u = 0; u < N; u++) {        /* compute spectrum over all freq u */
                real = imag = 0;    /* reset real, imag component of F(u) */
                for(x = 0; x < N; x++) {    /* visit each input pixel */
                    c =  cos(2.*PI*u*x/N);
                    s = -sin(2.*PI*u*x/N);
                    real += (f.real[x]*c - f.imag[x]*s);
                    imag += (f.real[x]*s + f.imag[x]*c);
                }
                F.real[u] = real / N;
                F.imag[u] = imag / N;
            }
    if (write_output(out, F))  printf("success!\n");
    else printf("failed to write to output file!\n");	
}
else if (dir == 1){
	for (int i = 0; i < N; i++) {
                fscanf(in, "%f %f", &F.real[i], &F.imag[i]);
            }
    for(x = 0; x < N; x++) {        /* compute each output pixel */
                real = imag = 0;
                for(u=0; u<N; u++) {
                    c = cos(2.*PI*u*x/N);
                    s = sin(2.*PI*u*x/N);
                    real += (F.real[u]*c - F.imag[u]*s);
                    imag += (F.real[u]*s + F.imag[u]*c);
                }
                f.real[x] = real;
                f.imag[x] = imag;
            }
    if (write_output(out, f))  printf("succeed!\n");
    else printf("failed to write to output file!\n");
}

else { 
		printf("Invalid: Please Just choose 0 or 1 \n");
	}
	free(f.real);
    free(f.imag);
    free(F.real);
    free(F.imag);
    return;
}



bool write_output(FILE *out_file ,complexS com) {
	//write header
    if (!(fprintf(out_file, "%d %d\n", 2, com.len))) return false;

    for(int i = 0; i < com.len; i++) {
        if(!(fprintf(out_file, "%f %f\n", com.real[i], com.imag[i]))) return false;
    }
    return true;
}

int main(int argc, char *argv[]) {
    if(argc == 4){
        FILE *input_file = fopen(argv[1], "r");
        FILE *output_file = fopen(argv[3], "w");
        int dir = atoi(argv[2]);
        dft1D(input_file,dir,output_file);
        fclose(input_file);
        fclose(output_file);
        }
    else printf("Program requires 3 parameters\n" );
    return 0;
    }

	


