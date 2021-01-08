#include "IP.h"
using namespace IP;

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW_blur:
//
// Blur image I1 with a box filter (unweighted averaging).
// The filter has width filterW and height filterH.
// We force the kernel dimensions to be odd.
// Output is in I2.
//
void
HW_blur(ImagePtr I1, int filterW, int filterH, ImagePtr I2)
{
	// copy image header (width, height) of input image I1 to output image I2
	IP_copyImageHeader(I1, I2);

	// init vars for width, height, and sum which will be used as rolling sum
	int w = I1->width();
	int h = I1->height();
	short sum = 0;
	int padWidth = ceil((double)filterW / 2);
	int padHeight = ceil((double)filterH / 2);

	//initialize buffer
	int* bufferH = new int[h + padHeight + filterH]; //cols
	int* bufferW = new int[w + padWidth + filterW]; //rows

  //Create new image to store the row blurred Image
	ImagePtr rowBlurredImage;
	IP_copyImageHeader(I1, rowBlurredImage);

	// declarations for image channel pointers and datatype
	ChannelPtr<uchar> p1, p2;
	int type;
	// visit all image channels and evaluate output image, usinfg the rowBlurredImage` as the output
	for (int ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {	// get input  pointer for channel ch
		IP_getChannel(rowBlurredImage, ch, p2, type);		// get output pointer for channel ch
		for (int j = 0; j < h; j++) { //visit every single row
			for (int i = padWidth; i < (w + padWidth); i++) { //fill buffer
				bufferW[i] = *p1++;
			}

			//set padding using pixel replication
			for (int i = 0; i < padWidth; i++) {
				bufferW[i] = bufferW[padWidth];
			}
			for (int i = w; i < (w + filterW); i++) {
				bufferW[i] = bufferW[w + padWidth - 1];
			}

			//initalize first value of sum
			sum = 0;
			for (int i = 0; i < filterW; i++) {
				sum += bufferW[i];
			}

			//Calculate blur by taking in new value and getting rid of previous
			for (int i = 0; i < w; i++) {
				*p2++ = CLIP((sum / filterW), 0, MaxGray);
				sum += bufferW[i + filterW];
				sum -= bufferW[i];
			}
		}
	}
	// declarations for image channel pointers and datatype
	ChannelPtr<uchar> p3, p4;

	// visit all image channels and evaluate output image, usinfg the rowBlurredImage` as the input
	for (int ch = 0; IP_getChannel(rowBlurredImage, ch, p3, type); ch++) {	// get input  pointer for channel ch
		IP_getChannel(I2, ch, p4, type);		// get output pointer for channel ch
		for (int j = 0; j < w; j++) { //go through every column from 0 to w
			for (int i = padHeight; i < (h + padHeight); i++) { // go to every row for i rows
				bufferH[i] = p3[(i - padHeight) * w + j];
			}

			//set padding using pixel replication
			for (int i = 0; i < padHeight; i++) {
				bufferH[i] = bufferH[padHeight];
			}
			for (int i = h; i < (h + filterH); i++) {
				bufferH[i] = bufferH[padHeight + h - 1];
			}

			//initalize first value of sum
			sum = 0;
			for (int i = 0; i < filterH; i++) {
				sum += bufferH[i];
			}

			//Calculate blur by taking in new value and getting rid of previous
			for (int i = 0; i < h; i++) {
				p4[(i)* w + j] = CLIP((sum / filterH), 0, MaxGray);
				sum += bufferH[i + filterH];
				sum -= bufferH[i];
			}
		}
	}
}
