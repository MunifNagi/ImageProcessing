#include "IP.h"
using namespace IP;

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW_gammaCorrect:
//
// Gamma correct image I1. Output is in I2.
//
void HW_gammaCorrect(ImagePtr I1, double gamma, ImagePtr I2){
	// copy image header (width, height) of input image I1 to output image I2
	IP_copyImageHeader(I1, I2);

	// init vars for width, height, and total number of pixels
	int w = I1->width();
	int h = I1->height();
	int total = w * h;

	gamma = 1.0 / gamma;

	// init lookup table
	int i, lut[MXGRAY];
	for (i = 0; i < MXGRAY; i++) {
		lut[i] = (int)CLIP((MaxGray * (pow((double)i / MaxGray, gamma))), 0, MaxGray);
	}

	// declarations for image channel pointers and datatype
	ChannelPtr<uchar> p1, p2;
	int type;

	// visit all image channels and evaluate output image
	for (int ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {	// get input  pointer for channel ch
		IP_getChannel(I2, ch, p2, type);		// get output pointer for channel ch
		for (i = 0; i < total; i++) *p2++ = lut[*p1++];	// use lut[] to eval output
	}
}
