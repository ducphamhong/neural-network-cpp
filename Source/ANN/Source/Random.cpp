#include "pch.h"
#include "Random.h"

#include <ctime>

namespace ANN
{
	time_t t;
	int seed = (int)time(&t);

	const int m = 2147483399;	// a non-Mersenne prime
	const int a = 40692;			// another spectral success story
	const int q = m / a;
	const int r = m % a;			// again less than q
	const int rMax = m - 1;

	void randomize(int value)
	{
		seed = value;
	}

	int getRandom()
	{
		seed = a * (seed % q) - r * (seed / q);
		if (seed < 0)
			seed += m;

		return seed;
	}

	double getRandom01()
	{
		return getRandom() * (1.0 / (double)rMax);
	}
}