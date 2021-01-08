#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>
#define PI 3.141592653589793

typedef struct {
	int len;
	float* real;
	float* imag;
} complexS, *complexP;

void read_input_file(char* input_file, complexP inputP);
void FFT4(complexP in, int dir, complexP out);
void write_to_output(char* outFile, complexP outputP);

int main(int argc, char* argv[]){
    
    FILE *input, *output;

    if(argc == 4){
        char *in_file_name = argv[1];
        char *out_file_name = argv[3];
        int dir = atoi(argv[2]);

        complexP inputP = new complexS; 
        complexP outputP = new complexS;

        read_input_file(in_file_name, inputP);
        FFT4(inputP, dir, outputP);
        write_to_output(out_file_name, outputP);
        return 0;
    }
    else {
        printf("%s\n", "Program requires 3 parameters");
        return 0;
    }
}

void read_input_file(char* input_file, complexP inputP){
	FILE *in = fopen(input_file, "r");

    //get width and height
	int width;
	int height;
	fscanf(in, "%d %d", &width, &height);
    if (!inputP) inputP = new complexS;

    inputP->len = height;
    inputP->real = (float *) malloc(sizeof(float) * height);
    inputP->imag = (float *) malloc(sizeof(float) * height);

    //read input.txt values
    for (int i = 0; i < height; i++) {
        fscanf(in, "%f %f", inputP->real + i, inputP->imag + i);
    }

	fclose(in);
}

void FFT4(complexP in,int dir, complexP out){

    if (!out) out = new complexS; 
    
    out->len = in->len;
    out->real = (float *) malloc(sizeof(float) * in->len);
    out->imag = (float *) malloc(sizeof(float) * in->len);

    int n = in->len;

    out->len = n;
    out->real = new float[out->len];
    out->imag = new float[out->len];
    int d;
    int direction = (dir) ? -1: 1;  // forward or inverse DFT

    if(n == 1){ // Nothing else to do
        for(int i = 0; i<n; i++){
            out->real[i] = in->real[i];
            out->imag[i] = in->imag[i];
        }
    }
    else{
        complexP even, odd;
        even = new complexS;
        odd = new complexS;

        even->len = n/2;
        odd->len = n/2;

        even->real = new float[even->len];
        odd->real = new float[odd->len];
        
        even->imag = new float[even->len];
        odd->imag = new float[odd->len];

        // construct the even half
        for(int i = 0; i < even->len; i++){
            even->real[i] = in->real[i*2];
            even->imag[i] = in->imag[i*2];
        }

        // construct the odd half
        for(int i = 0; i < odd->len; i++){
            odd->real[i] = in->real[i*2+1];
            odd->imag[i] = in->imag[i*2+1];
        }

        complexS f_even,f_odd;
        FFT4(even,dir,&f_even);
        FFT4(odd,dir,&f_odd);

        for(int i = 0; i < n/2; i++){
            float w_imag;
            float w_real;
            w_imag = sin(direction* -2*PI*i/(n));
            w_real = cos(direction* -2*PI*i/(n));

            out->real[i] = w_real*f_odd.real[i] - w_imag* f_odd.imag[i] + f_even.real[i];
            out->imag[i] = w_real*f_odd.imag[i] + w_imag*f_odd.real[i] + f_even.imag[i];
            out->real[i+n/2] = f_even.real[i] - w_real*f_odd.real[i] - w_imag* f_odd.imag[i];
            out->imag[i+n/2] = f_even.imag[i] - w_real*f_odd.imag[i] + w_imag*f_odd.real[i];
        }

        for (int i = 0; i < n; i++) {
            if(dir) {
                out->real[i] = out->real[i]; 
                out->imag[i] = out->imag[i];
            } else {
                out->real[i] = out->real[i]/2; 
                out->imag[i] = out->imag[i]/2;
            }
        }
    }
}


void write_to_output(char* outFile, complexP outputP){
    FILE *out = fopen(outFile, "w");

    fprintf(out, "2 %d\n", outputP->len);
    
    // write to the output file
    for (int i = 0 ; i < outputP->len; i++) {
        fprintf(out, "%f %f\n", outputP->real[i], outputP->imag[i]);
    }

    fclose(out);
}
