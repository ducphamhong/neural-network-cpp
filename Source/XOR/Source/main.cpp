#include "pch.h"
#include "ANN.h"

int main()
{
	// input: 2
	// hidden: 4
	// output: 10
	int dim[3] = { 2,4,1 };

	// train data
	double input[] = {
		0,0,
		0,1,
		1,0,
		1,1
	};

	double output[] = {
		0,
		1,
		1,
		0
	};

	// init 3 layers
	ANN::CANN ann(dim, 3);

	ann.LearnExpected = [](double* trainData, int trainId, double* expectedOutput, int numOutput) {
		// XOR will return 0, 1
		expectedOutput[0] = trainData[trainId];
	};

	ann.Predict = [](const double* output, int numOutput)
	{
		// Convert output layer to XOR result
		if (output[0] >= 0.5)
			return 1;
		else
			return 0;
	};

	// train
	printf("Begin learning\n");
	for (int i = 0; i < 10000; i++)
	{
		ann.train(input, output, 4);
	}

	// test learning
	printf("Result\n");
	double* p = input;
	for (int i = 0; i < 4; i++)
	{
		double ret = ann.predict(p);
		printf("%d xor %d = %d\n", (int)p[0], (int)p[1], (int)ret);
		p += 2;
	}

	return 1;
}