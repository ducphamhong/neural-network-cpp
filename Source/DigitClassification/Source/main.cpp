#include "pch.h"
#include "ANN.h"
#include "ImageLoaderPNG.h"

#include <filesystem>
namespace fs = std::filesystem;

double* getANNInput(SPNGImage* img)
{
	int size = img->Width * img->Height;

	double* ret = new double[size];
	unsigned char* data = img->Data;

	for (int i = 0; i < size; i++)
	{
		ret[i] = data[0] / 255.0;
		data = data + img->BPP;
	}

	return ret;
}

std::vector<std::string> learning;

void learnInFolder(ANN::CANN* ann, const char* folder, double targetValue)
{
	printf("Train: %s\n", folder);
	std::string p = folder;

	for (const auto& entry : fs::directory_iterator(p))
	{
		std::wstring path = entry.path().c_str();
		std::string filePng(path.begin(), path.end());

		SPNGImage img;
		if (loadPNG(filePng.c_str(), &img))
		{
			double* input = getANNInput(&img);
			double output[] = { targetValue };
			ann->train(input, output, 1);
			delete input;

			learning.push_back(filePng);
		}
	}
}

int main()
{
	// input: 784
	// hidden: 32
	// output: 10
	int dim[] = { 784, 32, 10 };

	printf("Init ANN\n");

	// init 3 layers
	ANN::CANN ann(dim, 3);

	ann.LearnExpected = [](double* trainData, int trainId, double* expectedOutput, int numOutput)
	{
		// ex: 3 => [0 0 0 1 0 0 0 0 0 0]
		int valueExpected = (int)trainData[trainId];
		for (int i = 0; i < 10; i++)
			expectedOutput[i] = (i == valueExpected) ? 1.0 : 0.0;
	};

	ann.Predict = [](const double* output, int numOutput)
	{
		// ex: [0 0 0 1 0 0 0 0 0 0] => return 3
		double ret = 0;
		double max = output[0];
		for (int i = 0; i < 10; i++)
		{
			if (max < output[i])
			{
				max = output[i];
				ret = (double)i;
			}
		}
		return ret;
	};

	learnInFolder(&ann, "./0", 0.0);
	learnInFolder(&ann, "./1", 1.0);
	learnInFolder(&ann, "./2", 2.0);
	learnInFolder(&ann, "./3", 3.0);
	learnInFolder(&ann, "./4", 4.0);
	learnInFolder(&ann, "./5", 5.0);
	learnInFolder(&ann, "./6", 6.0);
	learnInFolder(&ann, "./7", 7.0);
	learnInFolder(&ann, "./8", 8.0);
	learnInFolder(&ann, "./9", 9.0);

	printf("Test classification\n");
	int numLearning = (int)learning.size();
	for (int i = 0; i < 100; i++)
	{
		int randomTest = rand() % numLearning;
		std::string pngResource = learning[randomTest];

		SPNGImage img;
		if (loadPNG(pngResource.c_str(), &img))
		{
			double* input = getANNInput(&img);
			double value = ann.predict(input);
			printf("- %s -> %lf\n", pngResource.c_str(), value);
			delete input;
		}
	}

	return 1;
}