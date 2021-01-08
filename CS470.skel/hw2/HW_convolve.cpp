#include "IP.h"
using namespace IP;

// ~~~~~~~~~~~~~~~~~~~~~~~~
// HW_convolve:
//
// Convolve I1 with filter kernel in Ikernel.
// Output is in I2.
//

void
Load_row(ChannelPtr<uchar> p1, short* buffer, int w, int kernalSize) { //function to load an entire paadded row of the input
	int padded_row = w + 2 * (kernalSize / 2);
	for (int i = 0; i < padded_row; i++) { //going through the row with the padding
		if (i < kernalSize / 2) buffer[i] = *p1;  //Loading the left-padded values
		else if (i < kernalSize / 2 + w - 1) buffer[i] = *p1++; //Loading the row values
		else buffer[i] = *p1; //Loading the right-padded  values
	}
}

void
HW_convolve(ImagePtr I1, ImagePtr Ikernel, ImagePtr I2)
{
	IP_copyImageHeader(I1, I2);
	int w = I1->width();
	int h = I1->height();
	int total = w * h;
	int kernalSize = Ikernel->width();
	int padding_needed = 2 * (kernalSize / 2);

	int type;
	ChannelPtr<uchar> p1, p2, last;
	ChannelPtr<float> pKernel; // kerne pointer
	int filter_size = kernalSize * kernalSize;
	if (kernalSize > 1) {
		IP_getChannel(Ikernel, 0, pKernel, type);
		const int padded_row = padding_needed + w; // size of buffer for each padded row
		// short buffers[kernalSize][padded_row]; //kernalSize array of padded_row arrays of buffers

		short* buffers[7];
		for (int i = 0; i < kernalSize; i++) {
			buffers[i] = new short[padded_row];
		}

		for (int ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {
			IP_getChannel(I2, ch, p2, type);
			last = p1 + total;

			// Loading first row to the first half kernal buffers, starting from top padding rows.
			for (int i = 0; i < padding_needed / 2; i++) Load_row(p1, buffers[i], w, kernalSize);

			// Loading the rest of rows to the next half kernal size buffers, starting from first row in the input.
			for (int i = padding_needed / 2; i < kernalSize; i++) {
				Load_row(p1, buffers[i], w, kernalSize);
				if (p1 < last - w) p1 += w;
			}

			for (int y = 0; y < h; y++) { //going through every row
				// going through each pixel in that row
				for (int x = 0; x < w; x++) {
					float sum = 0.0;
					for (int i = 0; i < kernalSize; i++) {
						for (int j = 0; j < kernalSize; j++) {
							sum += (*pKernel++ * buffers[i][j + x]);
						}
					}
					*p2++ = (int)CLIP(sum, 0, 255);
					pKernel -= filter_size; //reseting kernal

				}
				//circular buffer
				for (int i = 0; i < kernalSize; i++) {
					if (i < kernalSize - 1) {//shifting the buffer rows upward except for the last one.
						for (int j = 0; j < padded_row; j++) buffers[i][j] = buffers[i + 1][j];
					}
					else Load_row(p1, buffers[kernalSize - 1], w, kernalSize); // Loading the next unprocessed row to the last row in buffer.
				}

				if (p1 < last - w) p1 += w; //checking if we hit the end of the image.
			}
		}
	}
}
