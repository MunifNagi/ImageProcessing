#include "IP.h"
using namespace IP;
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>
#define PI 3.14159265358979323846

typedef struct
{
	int len;
	float* real;
	float* imag;
} complexS, *complexP;

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW_spectrum:
//
// Convolve magnitude and phase spectrum from image I1.
// Output is in Imag and Iphase.
//

extern void HW_fft2MagPhase(ImagePtr Ifft, ImagePtr Imag, ImagePtr Iphase);
void fft4(complexP input, int dir, complexP out);
void HW_histoStretch(ImagePtr I1, int t1, int t2, ImagePtr I2);

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW_spectrum:
//
// Compute magnitude and phase spectrum from input image I1.
// Save results in Imag and Iphase.
//
void
HW_spectrum(ImagePtr I1, ImagePtr Imag, ImagePtr Iphase)
{
	int w = I1->width();
	int h = I1->height();
	// compute FFT of the input image
	
// PUT YOUR CODE HERE...----------------------------------------------------------------------------
	// declarations for image channel pointers and datatype
	ImagePtr I2 = NEWIMAGE;
	I2->allocImage(w, h, FFT_TYPE);
	ChannelPtr<uchar> p1;
	ChannelPtr<float> p2, p3;
	int type;

	// visit all image channels and evaluate output image, usinfg the rowBlurredImage` as the output
	for (int ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {	// get input  pointer for channel ch
		/*ChannelPtr<float>   p2 = I2[0];
		ChannelPtr<float>	p3 = I2[1];*/
		IP_getChannel(I2, 0, p2, type);		// get output pointer for channel ch
		IP_getChannel(I2, 1, p3, type);		// get output pointer for channel ch

		//initalize variables to be used with fft4
		complexP input = new complexS; // for the input image
		input->len		= w * h;
		input->real		= (float*)malloc(sizeof(float) * w*h);
		input->imag		= (float*)malloc(sizeof(float) * w*h);
		complexP output = new complexS; // for the output image
		output->len		= w * h;
		output->real	= (float*)malloc(sizeof(float) * w*h);
		output->imag	= (float*)malloc(sizeof(float) * w*h);

		//fill the struct for 1st pass (0 bec imaginary is empty) ROWS
		for (int i = 0; i < w*h; i++) {
			input->real[i] = p1[i];
			input->imag[i] = 0;
		}
		fft4(input, 0, output); //1st pass of fft (on rows)

		//2nd pass COLUMNS
		int c = 0;
		for (int j = 0; j < w; j++) {
			for (int i = 0; i < w*h; i += w) {
				float ga = input->real[c] = output->real[i + j];
				float gb = input->imag[c] = output->imag[i + j];
				c++;
			}
		}
		fft4(input, 0, output); //2nd pass of fft (on columns)
		//put back into image
		c = 0;
		for (int j = 0; j < w; j++) {
			for (int i = 0; i < w*h; i += w) {
				p2[i + j] = output->real[c];
				p3[i + j] = output->imag[c];
				c++;
			}
		}
		delete(input);//just to help with mem usage
		delete(output);
	}
	//end of my code ----------------------------------------------------------------------------
	
	// compute magnitute and phase spectrums from FFT image
	ImagePtr Im = NEWIMAGE; //mag
	ImagePtr Ip = NEWIMAGE; //phase
// PUT YOUR CODE HERE...--------------------------------------------------------------------------
	HW_fft2MagPhase(I2, Im, Ip);
	I1->~Image();
	I2->~Image();
	//end of my code -----------------------------------------------------------------------------

	// find min and max of magnitude and phase spectrums

// PUT YOUR CODE HERE...---------------------------------------------------------------------------
	//calculating min an dmax of mag and phase
	ChannelPtr<float> mmag, mphase;
	IP_getChannel(Im, 0, mmag, type);
	IP_getChannel(Ip, 0, mphase, type);
	float mmin = 255, mmax = 0, pmin = 255, pmax = 0;
	for (int i = 0; i < w*h; i++) {
		if (mmin > mmag[i]) {
			mmin = mmag[i];
		}
		if (mmax < mmag[i]) {
			mmax = mmag[i];
		}
		if (pmin > mphase[i]) {
			pmin = mphase[i];
		}
		if (pmax < mphase[i]) {
			pmax = mphase[i];
		}
	}
	//end of my code------------------------------------------------------------------------------

	// allocate uchar image for displaying magnitude and phase
	Imag  ->allocImage(w, h, BW_TYPE);
	Iphase->allocImage(w, h, BW_TYPE);

	// set imagetypes for single 8-bit channel
	Imag  ->setImageType(BW_IMAGE);
	Iphase->setImageType(BW_IMAGE);

	// get channel pointers for input magnitude and phase spectrums
	ChannelPtr<float>   magF = Im[0];
	ChannelPtr<float> phaseF = Ip[0];

	// get channel pointers for output magnitude and phase spectrums
	ChannelPtr<uchar> mag    = Imag  [0];
	ChannelPtr<uchar> phase  = Iphase[0];

	// scale magnitude and phase to fit between [0, 255]

// PUT YOUR CODE HERE...----------------------------------------------------------
	//scaling
	for (int i = 0; i < w*h; i++) {
		mag[i] = CLIP(ROUND((255 * (magF[i] - mmin)) / (mmax - mmin)), 0, 255);
		phase[i] = CLIP(ROUND((255 * (phaseF[i] - pmin)) / (pmax - pmin)), 0, 255);
	}
	//end of my code------------------------------------------------
}



void fft4(complexP in, int dir, complexP out) {

	if (!out) out = new complexS;

	out->len = in->len;
	out->real = (float *)malloc(sizeof(float) * in->len);
	out->imag = (float *)malloc(sizeof(float) * in->len);

	int n = in->len;

	out->len = n;
	out->real = new float[out->len];
	out->imag = new float[out->len];
	int d;
	int direction = (dir) ? -1 : 1;  // forward or inverse DFT

	if (n == 1) { // Nothing else to do
		for (int i = 0; i < n; i++) {
			out->real[i] = in->real[i];
			out->imag[i] = in->imag[i];
		}
	}
	else {
		complexP even, odd;
		even = new complexS;
		odd = new complexS;

		even->len = n / 2;
		odd->len = n / 2;

		even->real = new float[even->len];
		odd->real = new float[odd->len];

		even->imag = new float[even->len];
		odd->imag = new float[odd->len];

		// construct the even half
		for (int i = 0; i < even->len; i++) {
			even->real[i] = in->real[i * 2];
			even->imag[i] = in->imag[i * 2];
		}

		// construct the odd half
		for (int i = 0; i < odd->len; i++) {
			odd->real[i] = in->real[i * 2 + 1];
			odd->imag[i] = in->imag[i * 2 + 1];
		}

		complexS f_even, f_odd;
		fft4(even, dir, &f_even);
		fft4(odd, dir, &f_odd);

		for (int i = 0; i < n / 2; i++) {
			float w_imag;
			float w_real;
			w_imag = sin(direction* -2 * PI*i / (n));
			w_real = cos(direction* -2 * PI*i / (n));

			out->real[i] = w_real * f_odd.real[i] - w_imag * f_odd.imag[i] + f_even.real[i];
			out->imag[i] = w_real * f_odd.imag[i] + w_imag * f_odd.real[i] + f_even.imag[i];
			out->real[i + n / 2] = f_even.real[i] - w_real * f_odd.real[i] - w_imag * f_odd.imag[i];
			out->imag[i + n / 2] = f_even.imag[i] - w_real * f_odd.imag[i] + w_imag * f_odd.real[i];
		}

		for (int i = 0; i < n; i++) {
			if (dir) {
				out->real[i] = out->real[i];
				out->imag[i] = out->imag[i];
			}
			else {
				out->real[i] = out->real[i] / 2;
				out->imag[i] = out->imag[i] / 2;
			}
		}
	}
}