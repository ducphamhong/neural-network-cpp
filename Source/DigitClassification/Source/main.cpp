#include "pch.h"
#include "ANN.h"
#include "ImageLoaderPNG.h"

int main()
{
	// input: 774
	// hidden: 32
	// output: 10
	int dim[] = { 784, 32, 10 };

	// init 3 layers
	ANN::CANN ann(dim, 3);

	return 1;
}