#include "pch.h"
#include "ANN.h"

double getPredict(const double* output, int numOutput)
{
	double max = output[0];
	int result = 0;

	for (int i = 1; i < numOutput; i++)
	{
		if (max < output[i])
		{
			max = output[i];
			result = i;
		}
	}
	return (double)result;
}

void getExpected(double* trainData, int trainId, double* expectedOutput, int numOutput) {
	int id = trainId * numOutput;
	expectedOutput[0] = trainData[id];
	expectedOutput[1] = trainData[id + 1];
	expectedOutput[2] = trainData[id + 2];
	expectedOutput[3] = trainData[id + 3];
}

int main()
{
	std::vector<double> input;
	std::vector<double> output;

	// read input/output
	const char* dataInput = "Snake/learning.txt";

	printf("Read input/output: %s \n", dataInput);
	FILE* f = fopen(dataInput, "rt");
	char lines[512];

	int numInput = 0;
	int numOutput = 0;
	int numRecord = 0;

	fgets(lines, 512, f);
	sscanf(lines, "%d %d %d", &numInput, &numOutput, &numRecord);

	// skip bad last 2 move
	int numDataLearning = numRecord - 2;

	for (int j = 0; j < numDataLearning; j++)
	{
		for (int i = 0; i < numInput; i++)
		{
			double d;
			fgets(lines, 512, f);
			sscanf(lines, "%lf", &d);
			input.push_back(d);
		}

		for (int i = 0; i < numOutput; i++)
		{
			double d;
			fgets(lines, 512, f);
			sscanf(lines, "%lf", &d);
			output.push_back(d);
		}
	}

	fclose(f);

	const int dim1[] = { 14, 128, 64, 4 };
	const int dim2[] = { 14, 256, 128, 4 };

	// init 3 layers
	std::vector<ANN::CANN*> anns;
	std::vector<int> testResult;

	anns.push_back(new ANN::CANN(dim1, 4, ANN::EActivation::Tanh));
	anns.push_back(new ANN::CANN(dim2, 4, ANN::EActivation::Tanh));

	for (int i = 0, n = anns.size(); i < n; i++)
	{
		testResult.push_back(0);
		anns[i]->LearnExpected = getExpected;
		anns[i]->Predict = getPredict;
	}

	// train
	printf("Begin learning\n");
	int learnCount = 1000;
	int percent = 0;

	for (int i = 0, n = anns.size(); i < n; i++)
	{
		float p = 1.0f / (float)n;

		for (int j = 0; j < learnCount; j++)
		{
			anns[i]->train(input.data(), output.data(), numDataLearning);

			float p1 = j / (float)learnCount;

			float current = i * p;
			current = current + p1 * p;

			if (percent != (int)(current * 100.0))
			{
				percent = (int)(current * 100.0);
				printf("- Learning %d %%\n", percent);
			}
		}
	}

	// test learning
	printf("Test result\n");
	for (int i = 0; i < numDataLearning; i++)
	{
		double* inputTest = input.data() + i * numInput;
		double* outputTest = output.data() + i * numOutput;

		int result = (int)getPredict(outputTest, numOutput);

		for (int j = 0, n = anns.size(); j < n; j++)
		{
			int t = (int)anns[j]->predict(inputTest);
			if (result == t)
			{
				testResult[j]++;
			}
		}
	}

	for (int i = 0, n = anns.size(); i < n; i++)
	{
		float percent = testResult[i] / (float)numDataLearning;
		printf("- ANN [%d] - result: %d/%d - %d %%\n", i, testResult[i], numDataLearning, (int)(percent * 100));
	}

	return 1;
}