#include "IP.h"
using namespace IP;

void histoMatchApprox(ImagePtr, ImagePtr, ImagePtr);

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW_histoMatch:
//
// Apply histogram matching to I1. Output is in I2.
//
void
HW_histoMatch(ImagePtr I1, ImagePtr targetHisto, bool approxAlg, ImagePtr I2)
{
	if (approxAlg) {
		histoMatchApprox(I1, targetHisto, I2);
		return;
	}
	// copy image header (width, height) of input image I1 to output image I2
	IP_copyImageHeader(I1, I2);
	int w = I1->width();
	int h = I1->height();
	int total = w * h;

	//create variables to be used
	int i, p, R, Hsum, Havg;
	int left[MXGRAY], right[MXGRAY], h1[MXGRAY], reserve[MXGRAY];
	double scale;

	//point to the input and output histograms as well as the target
	int type;
	ChannelPtr<uchar> in, out;
	ChannelPtr<int> h2;
	IP_getChannel(targetHisto, 0, h2, type);   // target histogram

	//normalize h2 to confrom with dimensions of I1
	for (i = Havg = 0; i < MXGRAY; i++) Havg += h2[i];
	scale = (double)total / Havg;
	if (scale != 1) for (i = 0; i < MXGRAY; i++) h2[i] = ROUND(h2[i] * scale); // normalize h2

	for (int ch = 0; IP_getChannel(I1, ch, in, type); ch++) {
		IP_getChannel(I2, ch, out, type);

		for (i = 0; i < MXGRAY; i++) h1[i] = 0;   // clear histogram
		for (i = 0; i < total; i++) h1[in[i]]++;  // evaluate histogram

		R = 0;
		Hsum = 0;
		// remap gray values with the endpoint of intervals being left[] and right[]
		for (i = 0; i < MXGRAY; i++) {
			left[i] = R;    // set left end of interval
			Hsum += h1[i];    // store cumulative/total value
			while (Hsum > h2[R] && R < MXGRAY - 1) {
				Hsum -= h2[R];//as we fill the new histogram we subtract the pixels used up
				R++; //move to the next grey value
			}
			int lefty = h2[left[i]];
			reserve[i] = h2[left[i]] - Hsum;
			int resrv = reserve[i];
			right[i] = R;    // set right end of interval
		}
		// visit all input pixels
		for (i = 0; i < MXGRAY; i++) h1[i] = 0;
		for (i = 0; i < total; i++) { //move pixel by pixel through the input image
			p = left[in[i]];//assign p the left of the interval for pixel i with gray value in[i]
			//if the number of pixels used is less then the amount allowed by reserve OR the intervals dont overlap
			if ((h1[p] < reserve[in[i] - 1]) || (left[in[i]] != right[in[i] - 1]) && h1[p] < h2[p])
				out[i] = p; //then assign the grey value to the output image for that pixel
			else out[i] = p = left[in[i]] = MIN(p + 1, right[in[i]]);//we move onto the min of next grey value or the right most grey value of the interval and we set the output pixel to that grey value
			h1[p]++;//incremment the number of values have been used for the given p
		}

	}
}

void
histoMatchApprox(ImagePtr I1, ImagePtr targetHisto, ImagePtr I2)
{
	// copy image header (width, height) of input image I1 to output image I2
	IP_copyImageHeader(I1, I2);
	int w = I1->width();
	int h = I1->height();
	int total = w * h;

	int i, p, R, Hsum, Havg;
	int left[MXGRAY], right[MXGRAY], h1[MXGRAY];
	double scale;

	int type;
	ChannelPtr<uchar> in, out;
	ChannelPtr<int> h2;
	IP_getChannel(targetHisto, 0, h2, type);   // target histogram

	for (i = Havg = 0; i < MXGRAY; i++) Havg += h2[i];
	scale = (double)total / Havg;
	if (scale != 1) for (i = 0; i < MXGRAY; i++) h2[i] = ROUND(h2[i] * scale); // normalize h2

	for (int ch = 0; IP_getChannel(I1, ch, in, type); ch++) {
		IP_getChannel(I2, ch, out, type);

		for (i = 0; i < MXGRAY; i++) h1[i] = 0;   // clear histogram
		for (i = 0; i < total; i++) h1[in[i]]++;  // evaluate histogram

		R = 0;
		Hsum = 0;
		// remap gray values with the endpoint of intervals being left[] and right[]
		for (i = 0; i < MXGRAY; i++) {
			left[i] = R;    // set left end of interval
			Hsum += h1[i];    // store cumulative/total value

			while (Hsum > h2[R] && R < MXGRAY - 1) {
				Hsum -= h2[R];
				R++;
			}
			right[i] = R;    // set right end of interval
		}

		for (i = 0; i < MXGRAY; i++) h1[i] = 0; // clear histogram
		for (i = 0; i < total; i++) {   // iterate through all pixels of the image
			p = left[in[i]];
			if (h1[p] < h2[p]) out[i] = p;
			else out[i] = p = left[in[i]] = MIN(p + 1, right[in[i]]);
			h1[p]++;
		}
	}
}