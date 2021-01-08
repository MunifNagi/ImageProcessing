#include <iostream>
#include <math.h>
#include <algorithm>
#define PI 3.1415928

enum FILTERS
{
	BOX,
	TRIANGLE,
	CUBIC_CONV,
	LANCZOS,
	HANN,
	HAMMING
};
double boxFilter(double, double);
double triFilter(double, double);
double cubicConv(double, double);
double lanczos(double, double);
double hann(double, double);
double hamming(double, double);
void resize1D(float *IN, float *OUT, int inLen, int outLen, int kernel_type, double param);

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// resize1D:
//
// Scale 1D scanline. float version.
// Input  consists of  inLen elements in *IN.
// Output consists of outLen elements in OUT.
// The inter-pixel distance in IN and dst is offst: 1 for rows or row_width for columns
// The resampling filter used is specified by kernel_type (BOX, TRIANGLE, CUBIC_CONV, LANCZOS, HANN, HAMMING)
//

int main()
{

	float inMag[32], outMag[256], inMin[128], outMin[16];

	int i = 0;
	for (; i < 32; i++)
		i == 16 ? inMag[i] = 200 : inMag[i] = 100;
	
	i = 0;
	for(; i < 128; i++){
		inMin[i] = (sin(i * PI / 4) + 1) * 127.5;
	}
	
	int filter = 0;
	for (; filter < 6; filter++)
	{
		if(filter == 2){
			resize1D(inMag, outMag, 32, 256, filter, -1);
			resize1D(inMin, outMin, 128, 16, filter, -1);
		} else {
			resize1D(inMag, outMag, 32, 256, filter, 3);
			resize1D(inMin, outMin, 128, 16, filter, 3);
		}

		std::cout << "\n\nMagnification\n";

		std::cout << "\nInput \n";
		i = 0;
		for (; i < 32; i++)
		{
			std::cout << inMag[i] << ", ";
		}

		i = 0;
		std::cout << "\n\nFilter: " << filter << "\n";

		std::cout << "\nOutput \n";
		i = 0;
		for (; i < 256; i++)
		{
			std::cout << outMag[i] << ", ";
		}


		std::cout << "\n\nMinification\n";

		i = 0;
		std::cout << "\nInput \n";
		for (; i < 128; i++)
		{
			std::cout << inMin[i] << ", ";
		}

		i = 0;
		std::cout << "\n\nFilter: " << filter << "\n";

		std::cout << "\nOutput \n";
		i = 0;
		for (; i < 16; i++)
		{
			std::cout << outMin[i] << ", ";
		}
	}
}

void resize1D(float *IN, float *OUT, int inLen, int outLen, int kernel_type, double param)
{

	// copy IN to dst if no scale change
	if (inLen == outLen)
	{
		for (int i = 0; i < inLen; ++i)
		{
			*OUT = *IN;
			*IN++;
		}
		return;
	}

	// filter function and filter support (kernel half-width) declarations
	double (*filter)(double, double); // ptr to filter fct
	double filterSupport;							// 1-sided filter length

	// default values for filter and its half-width support
	filter = triFilter;
	filterSupport = 1;

	// compute filterSupport: the half-width of the filter
	switch (kernel_type)
	{
	case FILTERS::BOX:
		filter = boxFilter;
		filterSupport = .5;
		break;
	case FILTERS::TRIANGLE:
		filter = triFilter;
		filterSupport = 1;
		break;
	case FILTERS::CUBIC_CONV:
		filter = cubicConv;
		filterSupport = 2;
		break;
	case FILTERS::LANCZOS:
		filter = lanczos;
		filterSupport = param;
		break;
	case FILTERS::HANN:
		filter = hann;
		filterSupport = param;
		break;
	case FILTERS::HAMMING:
		filter = hamming;
		filterSupport = param;
		break;
	default:
		std::cout << (stderr, "resize1D: Bad filter choice %d\n", kernel_type);
		return;
	}

	// init filter amplitude (fscale) and width (fwidth), and scale change
	double fwidth = filterSupport;
	double fscale = 1.0;
	double scale = (double) outLen / inLen; // resampling scale factor

	// image minification: update fwidth and fscale;
	// else kernel remains intact for magnification
	if (scale < 1.0)
	{																	// minification: h(x) -> h(x*scale) * scale
		fwidth = filterSupport / scale; // broaden  filter
		fscale = scale;									// lower amplitude
	}

	// evaluate size of padding and buffer length
	int padlen = ceil(fwidth);			 // buffer pad length
	int buflen = inLen + 2 * padlen; // buffer length

	float *buf = (float *)malloc(sizeof(float) * buflen);

	// copy *IN into OUT; save space for padding
	float *p1 = IN;
	float *p2 = buf + padlen;
	for (int x = 0; x < inLen; ++x, p1++)
		p2[x] = *p1;

	// pad left and right columns
	int v1, v2;
	p1 = buf + padlen;
	p2 = p1 + inLen - 1;

	// replicate border
	v1 = p1[0];
	v2 = p2[0];
	for (int x = 1; x <= padlen; ++x)
	{
		p1[-x] = v1;
		p2[x] = v2;
	}

	// init srcp to point to first non-padded pixel in padded buffer
	// ChannelPtr<uchar> srcp = buf + padlen;
	float *srcp = buf + padlen;

	// compute all output pixels
	int left, right; // kernel extent in input
	double u;				 // input coordinate u
	double acc;			 // convolution accumulator
	double pixel;		 // fetched pixel value
	double weight;	 // filter kernel weight

	for (int x = 0; x < outLen; ++x)
	{
		// map output x to input u: inverse mapping
		u = x / scale;

		// left and right extent of kernel centered at u

		(u - fwidth < 0) ? left = floor(u - fwidth)
										 : left = ceil(u - fwidth);

		right = floor(u + fwidth);

		// reset acc for collecting convolution products
		acc = 0;

		// weigh input pixels around u with kernel
		double sumWeight = 0;
		for (int i = left; i <= right; ++i)
		{
			// fetch pixel
			// padding replaces pixel = srcp[CLIP(i, 0, inLen-1)] to pixel = srcp[i]
			pixel = srcp[i];

			// evaluate weight; multiply it with pixel and add it to accumulator
			weight = (*filter)((u - i) * fscale, param);
			sumWeight += weight;
			acc += (pixel * weight);
		}

		// assign weighted accumulator to OUT

		*OUT = (int)std::min(std::max(round(acc / sumWeight), (double)0), (double)255);
		OUT++;
	}
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// boxFilter:
//
// Box (nearest neighbor) filter.
//
double
boxFilter(double t, double /*param*/)
{
	if ((t > -.5) && (t <= .5))
		return (1.0);
	return (0.0);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// triFilter:
//
// Triangle filter (used for linear interpolation).
//
double
triFilter(double t, double /*param*/)
{
	if (t < 0)
		t = -t;
	if (t < 1.0)
		return (1.0 - t);
	return (0.0);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// cubicConv:
//
// Cubic convolution filter.
//
double
cubicConv(double t, double param = -1)
{
	float A;
	double t2, t3;

	if (t < 0)
		t = -t;
	t2 = t * t;
	t3 = t2 * t;

	A = param;
	if (t < 1.0)
		return ((A + 2) * t3 - (A + 3) * t2 + 1);
	if (t < 2.0)
		return (A * (t3 - 5 * t2 + 8 * t - 4));
	return (0.0);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// sinc:
//
// Sinc function.
//
double
sinc(double t)
{
	t *= PI;
	if (t != 0)
		return (sin(t) / t);
	return (1.0);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// lanczos:
//
// Lanczos filter.
//
double
lanczos(double t, double param = 3)
{
	int N;

	N = (int)param;
	if (t < 0)
		t = -t;
	if (t < N)
		return (sinc(t) * sinc(t / N));
	return (0.0);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// hann:
//
// Hann windowed sinc function.
//
double
hann(double t, double param = 3)
{
	int N;

	N = (int)param;
	if (t < 0)
		t = -t;
	if (t < N)
		return (sinc(t) * (.5 + .5 * cos(PI * t / N)));
	return (0.0);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// hamming:
//
// hamming windowed sinc function.
//
double
hamming(double t, double param = 3)
{
	int N;

	N = (int)param;
	if (t < 0)
		t = -t;
	if (t < N)
		return (sinc(t) * (.54 + .46 * cos(PI * t / N)));
	return (0.0);
}
