#include "IP.h"
using namespace IP;

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW_quantize:
//
// Quantize I1 to specified number of levels. Apply dither if flag is set.
// Output is in I2.
//


void HW_quantize(ImagePtr I1, int levels, bool dither, ImagePtr I2)
{
	IP_copyImageHeader(I1, I2);

	int w = I1->width();
	int h = I1->height();
	int total = w * h;

	int lut[MXGRAY];
	double scale = (double)MXGRAY / levels;
	double bias = scale / 2;

	for (int i = 0; i < MXGRAY; i++) lut[i] = scale * (int)(i / scale) + bias;

	ChannelPtr<uchar> p1, p2;
	int type;

	if (!dither) {
		for (int ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {
			IP_getChannel(I2, ch, p2, type);
			for (int i = 0; i < total; i++)* p2++ = lut[*p1++];
		}
	}
	else {
		int sign, jitter, newVal;
		for (int ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {
			IP_getChannel(I2, ch, p2, type);

			for (int y = 0; y < h; y++) {
				sign = (y % 2) ? 1 : -1;

				for (int x = 0; x < w; x++) {
					jitter = ((double)rand() / RAND_MAX) * bias;
					newVal = *p1++ + jitter * sign;
					sign *= 1;
					*p2++ = lut[std::max(std::min(255, newVal), 0)];
				}
			}
		}
	}
}