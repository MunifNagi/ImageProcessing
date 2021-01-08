#include <algorithm>
#include <vector>
#include "IP.h"
using namespace IP;
using std::vector;
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW_median:
//
// Apply median filter of size sz x sz to I1.
// Clamp sz to 9.
// Output is in I2.

void HW_median(ImagePtr I1, int sz, ImagePtr I2) {

	// copy image header (width, height) of input image I1 to output image I2
	IP_copyImageHeader(I1, I2);

	// init vars for width, height, and total number of pixels
	int w = I1->width();
	int h = I1->height();

	// validate size of filter
	if (sz > 9) sz = 9; 
	if (sz % 2 == 0) sz++;

	// define space for the buffer
	uchar* buffer[9];

	// each row in the filter should have a width of w for all the pixels in the row
	// plus the width of the filter itself - 1 which would be sz - 1
	for (int i = 0; i < sz; i++) buffer[i] = new uchar[w - 1 + sz];

	// creates a maximum of 9 rows for the filter
	uchar* rows[9];
	for (int i = 0; i < sz; i++) rows[i] = buffer[i] + sz / 2; // points the starting point of the row past the padding

	std::vector<uchar> filter;
	ChannelPtr<uchar> p1, p2;
	int type, j;

	// visit all image channels and evaluate output image
	for (int ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {	// get input  pointer for channel ch
		IP_getChannel(I2, ch, p2, type);		// get output pointer for channel ch
		
		// copies the first row of teh image into the first row of rows
		for (int x = 0; x < w; x++) rows[0][x] = *p1++;

		// copies over the left most pixel to the padding
		for (int x = sz / 2; x >= 1; x--) rows[0][-x] = rows[0][0];

		// copies over the rightmost pixel to the padding
		for (int x = w; x < w + (sz / 2); x++) rows[0][x] = rows[0][w - 1];

		// copy row 0 down to the amount of rows that is half the size of the filter
		// this will populate the top padding for when teh center is at row 0
		for (int i = 0; i <= sz / 2; i++) {
			for (j = 0; j < w + ((sz / 2) - 1); j++) {
				buffer[i][j] = buffer[0][j];
			}
		}

		// copy the rest of the important rows into buffer
		for (int i = (sz / 2) + 1; i < sz; i++) {

			// copy each row from input to corresponding row in buffer
			for (int x = 0; x < w; x++) rows[i][x] = *p1++;
			
			// add padding to left side by replicating leftmost pixel
			for (int x = 1; x <= sz / 2; x++) rows[i][-x] = rows[i][0];
			
			// add padding to right side by replicating rightmost pixel
			for (int x = w; x < w + (sz / 2); x++) rows[i][x] = rows[i][w - 1];
			
		}

		// iterate through each pixel in input image
		for (int y = 0; y < h; y++) {
			for (int x = 0; x < w; x++) {
				filter.clear();

				// copy neighborhood into buffer
				for (int i = 0; i < sz; i++) {
					// half the filter size to the left of the current pixel and half the filter size to the right
					for (int p = -(sz / 2); p <= sz / 2; p++) filter.push_back(rows[i][x + p]);
				}

				// sort filter
				std::sort(filter.begin(), filter.end());

				// copy median into corresponding output pixel
				p2[w * y + x] = filter[sz * sz / 2];
			}

			// advance rows
			for (j = 0; j < sz - 1; j++) rows[j] = rows[j + 1];

			// regularly cycle the position of the new row and points it past the padding
			rows[j] = buffer[y % sz] + sz / 2;

			// populate that row with the next set of pixels in the image
			if (y < h - sz)
				for (int x = 0; x < w; x++) rows[j][x] = *p1++;
		}
	}
}
