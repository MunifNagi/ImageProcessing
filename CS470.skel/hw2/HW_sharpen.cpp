#include "IP.h"
using namespace IP;

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW_sharpen:
//
// Sharpen image I1. Output is in I2.
//

extern void
HW_blur(ImagePtr I1, int filterW, int filterH, ImagePtr I2);
void
HW_sharpen(ImagePtr I1, int size, double factor, ImagePtr I2)
{
	// copy image header (width, height) of input image I1 to output image I2
	IP_copyImageHeader(I1, I2);

	// init vars for width, height, and sum which will be used as rolling sum
	int w = I1->width();
	int h = I1->height();

	//Create new image to store the row blurred Image
	ImagePtr BlurredImage;
	HW_blur(I1, size, size, I2);

	// declarations for image channel pointers and datatype
	ChannelPtr<uchar> p1, p2, p3;
	int type;
	// visit all image channels and evaluate output image, usinfg the rowBlurredImage` as the input
	for (int ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {	// get input  pointer for channel ch
		//IP_getChannel(BlurredImage, ch, p2, type);
		IP_getChannel(I2, ch, p3, type);		// get output pointer for channel ch
		for (int i = 0; i < w * h; i++) {
			*p3 = CLIP(((factor * (*p1 - *p3)) + *p1), 0, MaxGray);// CLIP(((factor*(*p1 - *p2)) + *p1), 0, MaxGray);
			p1++;
			//p2++;
			p3++;
		}
	}
}
