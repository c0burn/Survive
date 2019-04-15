#include "survive.h"

/*==========
crand

return a random double from -1 to +1
==========*/
double crand(void)
{
	double r = (double)rand() / (double)RAND_MAX;
	return 2 * (r - 0.5);
}

/*==========
rand_range

return a random integer between min and max
==========*/
int rand_range(int min, int max)
{
	// check for max < min
	if (max < min)
	{
		printf("rand_range: max < min! swapping them.\n");
		int temp = max;
		max = min;
		min = temp;
	}

	return rand() % (max - min + 1) + min;
}

/*==========
clamp

clamp a value between min and max
==========*/
int clamp(int what, int min, int max)
{
	if (max < min)
	{
		printf("clamp: max < min!\n");
		int temp = max;
		max = min;
		min = temp;
	}
	if (what < min)	what = min;
	if (what > max)	what = max;
	return what;
}

/*=========
radtodeg

convert radians to degrees
========*/
double radtodeg(double rad)
{
	return rad * (180 / M_PI);
}

/*==========
degtorad

convert degrees to radians
==========*/
double degtorad(double deg)
{
	return deg * (M_PI / 180);
}

/*==========
anglemod

clamp an angle between 0 - 360
==========*/
double anglemod(double ang)
{
	while (ang >= 360)
		ang = ang - 360;
	while (ang < 0)
		ang = ang + 360;
	return floor(ang);
}
