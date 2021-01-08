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
// HW_swapPhase:
//
// Swap the phase channels of I1 and I2.
// Output is in II1 and II2.
//

extern void HW_fft2MagPhase(ImagePtr Ifft, ImagePtr Imag, ImagePtr Iphase);
extern void HW_MagPhase2fft(ImagePtr Imag, ImagePtr Iphase, ImagePtr Ifft);
void Ifft2d(ImagePtr I1, ImagePtr Imag, ImagePtr Ireal);
void invIfft2d(ImagePtr Ifft, ImagePtr I1);
void fft4(complexP input, int dir, complexP out);
void padImage(ImagePtr i1, ImagePtr i2);

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW_swapPhase:
//
// Swap phase of I1 with I2.
// (I1_mag, I2_phase) -> II1
// (I1_phase, I2_mag) -> II2
//
void
HW_swapPhase(ImagePtr I1, ImagePtr I2, ImagePtr II1, ImagePtr II2)
{
	ImagePtr Ifft1, Ifft2, IinvFFT1, IinvFFT2;
	ImagePtr Imag1, Iphase1, Imag2, Iphase2;

	// compute FFT of I1 and I2

// PUT YOUR CODE HERE...
	padImage(I1, I2);
	//get widths and heights of images
	int w = I1->width();
	int h = I1->height();

	// allocated soace for images
	//Ifft1->allocImage(w1, h1, FFT_TYPE);
	//IinvFFT1->allocImage(w1, h1, FFT_TYPE);
	//Imag1->allocImage(w1, h1, FLOATCH_TYPE);
	//Iphase1->allocImage(w1, h1, FLOATCH_TYPE);
	//Ifft2->allocImage(w2, h2, FFT_TYPE);
	//IinvFFT2->allocImage(w2, h2, FFT_TYPE);
	//Imag2->allocImage(w2, h2, FLOATCH_TYPE);
	//Iphase2->allocImage(w2, h2, FLOATCH_TYPE);

	//pass in imzges to be fourier transformed
	Ifft2d(I1, Imag1, Iphase1);
	Ifft2d(I2, Imag2, Iphase2);

	// compute magnitude and phase from real and imaginary FFT channels

// PUT YOUR CODE HERE...
	HW_fft2MagPhase(Ifft1, Imag1, Iphase1);
	HW_fft2MagPhase(Ifft2, Imag2, Iphase2);
	// swap phases and convert back to FFT images

// PUT YOUR CODE HERE...
	HW_MagPhase2fft(Imag1, Iphase2, Ifft1);
	HW_MagPhase2fft(Imag2, Iphase1, Ifft2);
	// compute inverse FFT

// PUT YOUR CODE HERE...
	invIfft2d(Ifft1, IinvFFT1);
	invIfft2d(Ifft2, IinvFFT2);

	// extract magnitude from resulting images

// PUT YOUR CODE HERE...
	HW_fft2MagPhase(IinvFFT1, Imag1, Iphase1);
	HW_fft2MagPhase(IinvFFT2, Imag2, Iphase2);
	// allocate uchar image and cast float channel to uchar for mag1

// PUT YOUR CODE HERE...
	ImagePtr mag1 = NEWIMAGE;
	mag1->allocImage(w, h, UCHARCH_TYPE);
	IP_castChannel(Imag1, 0, mag1, 0, 0);
	// allocate uchar image and cast float channel to uchar for mag2

// PUT YOUR CODE HERE...
	ImagePtr mag2 = NEWIMAGE;
	mag1->allocImage(w, h, UCHARCH_TYPE);
	IP_castChannel(Imag2, 0, mag2, 0, 0);
}

void Ifft2d(ImagePtr I1, ImagePtr Imag, ImagePtr Iphase) {
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
		input->len = w * h;
		input->real = (float*)malloc(sizeof(float) * w*h);
		input->imag = (float*)malloc(sizeof(float) * w*h);
		complexP output = new complexS; // for the output image
		output->len = w * h;
		output->real = (float*)malloc(sizeof(float) * w*h);
		output->imag = (float*)malloc(sizeof(float) * w*h);

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
}

void invIfft2d(ImagePtr Ifft, ImagePtr I1) {
	int w = Ifft->width();
	int h = Ifft->height();
	// compute FFT of the input image

	IP_copyImageHeader(Ifft, I1);

	// PUT YOUR CODE HERE...
		// declarations for image channel pointers and datatype
	ChannelPtr<float> p1, p2;
	ChannelPtr<uchar> p3, p4;

	int type;
	// visit all image channels and evaluate output image, usinfg the rowBlurredImage` as the output
	for (int ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {	// get input  pointer for channel ch
		IP_getChannel(Ifft, 0, p2, type);		// get output pointer for channel ch
		IP_getChannel(Ifft, 1, p3, type);		// get output pointer for channel ch
		IP_getChannel(I1, 1, p4, type);
		//ChannelPtr<float> p2 = I2[0];
		//ChannelPtr<float> p3 = I2[1];
		complexP input = new complexS; // for the input image
		input->len = w * h;
		input->real = (float*)malloc(sizeof(float) * w*h);
		input->imag = (float*)malloc(sizeof(float) * w*h);
		complexP output = new complexS; // for the output image
		output->len = w * h;
		output->real = (float*)malloc(sizeof(float) * w*h);
		output->imag = (float*)malloc(sizeof(float) * w*h);
		for (int i = 0; i < w*h; i++) {
			input->real[i] = p2[i];
			input->imag[i] = p3[i];
		}
		fft4(input, 1, output);
		int c = 0;
		for (int j = 0; j < w; j++) {
			for (int i = 0; i < w*h; i += w) {
				input->real[c] = output->real[i + j];
				input->imag[c] = output->imag[i + j];
				c++;
			}
		}
		fft4(input, 1, output);
		c = 0;
		for (int j = 0; j < w; j++) {
			for (int i = 0; i < w*h; i += w) {
				/*float ga =*/ p1[i + j] = output->real[c];
				/*float gb =*/ p4[i + j] = output->imag[c];
				c++;
			}
		}
		free(input);//just to help with mem usage
		free(output);
	}
}

void padImage(ImagePtr i1, ImagePtr i2) {
	//get widths and heights of images
	int w1 = i1->width();
	int h1 = i1->height();
	int w2 = i2->width();
	int h2 = i2->height();
	//pad image that is smaller
	// declarations for image channel pointers and datatype
	ChannelPtr<float> p1, p2;
	int type;
	// visit all image channels and evaluate output image
		if (w1 < w2) {
			if (h1 < h2) {
				i1->allocImage(w2, h2, FLOATCH_TYPE);
					IP_getChannel(i1, 0, p1, type);	// get input  pointer for channel ch
					IP_getChannel(i2, 0, p2, type);		// get output pointer for channel ch
				for (int j = w1; j < h1*w2; j += w1) {
					for (int i = 0; i < (w2 - w1); i++) {
						p1[i + j] = 0;
					}
				}
				for (int i = h1 * w2; i < h2*w2; i++) {
					p1[i] = 0;
				}
			}
			else { //width is bigger on p2 but h is bigger on p1
				i1->allocImage(w2, h1, FLOATCH_TYPE);
				i2->allocImage(w2, h1, FLOATCH_TYPE);
				IP_getChannel(i1, 0, p1, type);	// get input  pointer for channel ch
				IP_getChannel(i2, 0, p2, type);		// get output pointer for channel ch
				for (int j = w1; j < h1*w2; j += w1) {
					for (int i = 0; i < (w2 - w1); i++) {
						p1[i + j] = 0;
					}
				}
				for (int i = h2 * w2; i < h1*w2; i++) {
					p2[i] = 0;
				}
			}
		}
		else {
			if (w1 > w2) {
				i1->allocImage(w1, h1, FLOATCH_TYPE);
				i2->allocImage(w1, h1, FLOATCH_TYPE);
				IP_getChannel(i1, 0, p1, type);	// get input  pointer for channel ch
				IP_getChannel(i2, 0, p2, type);		// get output pointer for channel ch
				for (int j = w2; j < h2*w1; j += w2) {
					for (int i = 0; i < (w1 - w2); i++) {
						p2[i + j] = 0;
					}
				}
				if (h1 < h2) {
					i1->allocImage(w1, h2, FLOATCH_TYPE);
					i2->allocImage(w1, h2, FLOATCH_TYPE);
					IP_getChannel(i1, 0, p1, type);	// get input  pointer for channel ch
					IP_getChannel(i2, 0, p2, type);		// get output pointer for channel chs
					for (int i = h1 * w1; i < h2*w2; i++) {
						p1[i] = 0;
					}
				}
				else { //width is bigger on p2 but h is bigger on p1
					i1->allocImage(w1, h1, FLOATCH_TYPE);
					i2->allocImage(w1, h1, FLOATCH_TYPE);
					IP_getChannel(i1, 0, p1, type);	// get input  pointer for channel ch
					IP_getChannel(i2, 0, p2, type);		// get output pointer for channel ch
					for (int i = h2 * w1; i < h1*w1; i++) {
						p2[i] = 0;
					}
				}
			}
		}

}

