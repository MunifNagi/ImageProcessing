#include "IP.h"
using namespace IP;

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW_errDiffusion:
//
// Apply error diffusion algorithm to image I1.
//
// This procedure produces a black-and-white dithered version of I1.
// Each pixel is visited and if it + any error that has been diffused to it
// is greater than the threshold, the output pixel is white, otherwise it is black.
// The difference between this new value of the pixel from what it used to be
// (somewhere in between black and white) is diffused to the surrounding pixel
// intensities using different weighting systems.
//
// Use Floyd-Steinberg     weights if method=0.
// Use Jarvis-Judice-Ninke weights if method=1.
//
// Use raster scan (left-to-right) if serpentine=0.
// Use serpentine order (alternating left-to-right and right-to-left) if serpentine=1.
// Serpentine scan prevents errors from always being diffused in the same direction.
//
// A circular buffer is used to pad the edges of the image.
// Since a pixel + its error can exceed the 255 limit of uchar, shorts are used.
//
// Apply gamma correction to I1 prior to error diffusion.
// Output is saved in I2.
//

void gammaCorrect(ImagePtr I1, double gamma, ImagePtr I2);

void HW_errDiffusion(ImagePtr I1, int method, bool serpentine, double gamma, ImagePtr I2) {
	// copy image header (width, height) of input image I1 to output image I2
	IP_copyImageHeader(I1, I2);

	// init vars for width, height, and total number of pixels
	int w = I1->width();
	int h = I1->height();

	// look up table to hold the threshold values
	int i, lut[MXGRAY];
	for (i = 0; i < 128 && i < MXGRAY; ++i) lut[i] = 0;
	for (; i < MXGRAY; ++i) lut[i] = 255;


	// Create the space needed for the padding floyd method requires teh current row and the next row, and the jarvis method requires the current row and the next two rows. We can use the same circular buffer for
	// both jarvis and floyd so we would need a maximum of 3 arrays inside of another array.
	// Array that holds pointers to 3 other arrays
	short *cbuffer[3];

	// w+4 refers to the width and the maximum extra 2 rows on each side for the padding for edge cases
	cbuffer[0] = new short[w+4];
	cbuffer[1] = new short[w+4]; 
	cbuffer[2] = new short[w+4];
	

	// allocate the predefined weights for each method
	// Floyd Method
	double floydMethod[4];
	floydMethod[0] = (double)7 / 16;
	floydMethod[1] = (double)5 / 16;
	floydMethod[2] = (double)3 / 16;
	floydMethod[3] = (double)1 / 16;


	//Jarvis method
	double jarvisMethod[4];
	jarvisMethod[0] = (double)7 / 48;
	jarvisMethod[1] = (double)5 / 48;
	jarvisMethod[2] = (double)3 / 48;
	jarvisMethod[3] = (double)1 / 48;
	

	// perform the Gamma correction prior to iterating over all row of the image
	// Store the result in memory referenced to by the ImagePtr gammaCorrectedImage
	ImagePtr gammaCorrectedImage;
	gammaCorrect(I1, gamma, gammaCorrectedImage);

	//populate the circular buffer
	// +2 shifts points the firstRow pointer past the inital padding
	short *firstRow = cbuffer[0] + 2; 
	short *secondRow = cbuffer[1] + 2; 
	short *thirdRow = cbuffer[2] + 2; 


	// declarations for image channel pointers and datatype
	ChannelPtr<uchar> p1, p2;
	int type;
	// visit all image channels and evaluate output image, usinfg the gammaCorrectedImage as the output
	for (int ch = 0; IP_getChannel(gammaCorrectedImage, ch, p1, type); ch++) {	// get input  pointer for channel ch
		IP_getChannel(I2, ch, p2, type);		// get output pointer for channel ch
		
		// Copy teh first row of the input image to the first two rows of the circular buffer
		// this allows us to calculate the corresponding errors of each pixel

		//Floyd Method
		if (method == 0) {

			// Populate the first two rows of the circular buffer with the first two rows of teh input image
			for (int x = 0; x < w; x++) firstRow[x] = *p1++;
			for (int x = 0; x < w; x++) secondRow[x] = *p1++;

			// 'indexing' -1 pushes the pointer the column of padding, the left padding has the same value as the first element in the row 
			firstRow[-1] = firstRow[0];
			firstRow[-2] = firstRow[0];
			secondRow[-1] = secondRow[0];
			secondRow[-2] = secondRow[0];

			// The right padding has the same element as teh last element in that row
			firstRow[w] = firstRow[w-1];
			firstRow[w+1] = firstRow[w-1];
			secondRow[w] = secondRow[w-1];
			secondRow[w+1] = secondRow[w-1];

		}

		else if (method == 1) {

			// Populate all 3 rows of teh circular buffer with the first 3 rows in the image
			for (int x = 0; x < w; x++) firstRow[x] = *p1++;
			for (int x = 0; x < w; x++) secondRow[x] = *p1++;
			for (int x = 0; x < w; x++) thirdRow[x] = *p1++;
			
			
			// Replicate the first pixel of the row to the left two padding pixels
			firstRow[-1] = firstRow[0];
			firstRow[-2] = firstRow[0];
			firstRow[w] = firstRow[w - 1];
			firstRow[w+1] = firstRow[w - 1];

			// Replicate the first pixel of second row to teh left two padding pixels
			secondRow[-1] = secondRow[0];
			secondRow[-2] = secondRow[0];
			secondRow[w] = secondRow[w - 1];
			secondRow[w+1] = secondRow[w - 1];

			// Replicate the first pixel of teh third row to the left two padding pixels
			thirdRow[-1] = thirdRow[0];
			thirdRow[-2] = thirdRow[0];
			thirdRow[w] = thirdRow[w - 1];
			thirdRow[w+1] = thirdRow[w - 1];

		}

		// Now we can iterate through each column and each row and apply the error diffusion using the
		// circular buffer
		// Erro used to hold teh difference between orighinal and thresholded pixel
		int error;

		for (int y = 0; y < h; y++) {

			//Raster scan method
			// Not serpentining or we are at an odd row go left to right
			if (!serpentine || !(y%2)) {

				//iterate across each column
				for (int x = 0; x < w; x++) {

					// trheshold teh original image using the LUT
					// p2[x] = lut[firstRow[x]];
					(firstRow[x] < 128) ? p2[x] = 0 : p2[x] = 255;
		
					// we need the error of the original pixel and the thresholded value
					error = firstRow[x] - p2[x];

					// using the predefined weights add the errors to the correspondinf
					// neighbor according to the algorithm

					if (method == 0) {
						//Floyd method 

						firstRow[x + 1] += floydMethod[0] * error;
						secondRow[x - 1] += floydMethod[2] * error;
						secondRow[x] += floydMethod[1] * error;
						secondRow[x + 1] += floydMethod[3] * error;

					}
					else if (method == 1) {
						//Jarvis method

						firstRow[x + 1] += jarvisMethod[0] * error;
						firstRow[x + 2] += jarvisMethod[1] * error;
						secondRow[x - 2] += jarvisMethod[2] * error;
						secondRow[x - 1] += jarvisMethod[1] * error;
						secondRow[x] += jarvisMethod[0] * error;
						secondRow[x + 1] += jarvisMethod[1] * error;
						secondRow[x + 2] += jarvisMethod[2] * error;
						thirdRow[x - 2] += jarvisMethod[3] * error;
						thirdRow[x - 1] += jarvisMethod[2] * error;
						thirdRow[x] += jarvisMethod[1] * error;
						thirdRow[x + 1] += jarvisMethod[2] * error;
						thirdRow[x + 2] += jarvisMethod[3] * error;

					}
				}

			}
			// serpentine is checked and we are at an odd row so we should go right to left
			else {

				for (int x = w - 1; x >= 0; x--) {

					// trheshold each pixel of the row
					// p2[x] = lut[firstRow[x]];
					(firstRow[x] < 128) ? p2[x] = 0 : p2[x] = 255;

					// calculate the error
					error = firstRow[x] - p2[x];

					if (method == 0) {
						// floyd method

						firstRow[x - 1] += floydMethod[0] * error;
						secondRow[x + 1] += floydMethod[2] * error;
						secondRow[x] += floydMethod[1] * error;
						secondRow[x - 1] += floydMethod[3] * error;
					}
					else if (method == 1) {
						//Jarvis method

						firstRow[x - 1] += jarvisMethod[0] * error;
						firstRow[x - 2] += jarvisMethod[1] * error;
						secondRow[x + 2] += jarvisMethod[2] * error;
						secondRow[x + 1] += jarvisMethod[1] * error;
						secondRow[x] += jarvisMethod[0] * error;
						secondRow[x - 1] += jarvisMethod[1] * error;
						secondRow[x - 2] += jarvisMethod[2] * error;
						thirdRow[x + 2] += jarvisMethod[3] * error;
						thirdRow[x + 1] += jarvisMethod[2] * error;
						thirdRow[x] += jarvisMethod[1] * error;
						thirdRow[x - 1] += jarvisMethod[2] * error;
						thirdRow[x - 2] += jarvisMethod[3] * error;

					}
				}
			}


	           


			// move on to the next row and uipdate teh circular buffer
			if (method == 0) {
				
				//floyd method
				firstRow = secondRow;
				secondRow = cbuffer[y % 2] + 2; // set this row to alternate between 1 and 0

				if (y < h - 2) {
					// y is not yet at the second to last row
					for (int x = 0; x < w; x++) {
						secondRow[x] = *p1++;
					}
				}
			}
			else if (method == 1) {
				firstRow = secondRow;
				secondRow = thirdRow;
				thirdRow = cbuffer[y % 3] + 2;

				if (y < h - 3) {
					// y is not yet at the second to last row
					for (int x = 0; x < w; x++) {
						thirdRow[x] = *p1++;
					}
				}
			}	

			// increments the output image
			p2 += w;
		}
	}
}


// Gamma correction 
void gammaCorrect(ImagePtr I1, double gamma, ImagePtr I2) {
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
		for (i = 0; i < total; i++)* p2++ = lut[*p1++];	// use lut[] to eval output
	}
}
