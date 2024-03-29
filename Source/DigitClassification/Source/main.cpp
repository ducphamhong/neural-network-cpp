#include "pch.h"
#include "ANN.h"
#include "Random.h"
#include "ImageLoaderPNG.h"

#include <filesystem>
#include <algorithm>

namespace fs = std::filesystem;

double* getANNInput(SPNGImage* img)
{
	int size = img->Width * img->Height;

	double* ret = new double[size];
	unsigned char* data = img->Data;

	for (int i = 0; i < size; i++)
	{
		ret[i] = (data[0] / 255.0);
		data = data + img->BPP;
	}

	return ret;
}

void indexImageInFolder(std::vector<std::string>& lession, const char* folder)
{
	printf("Prepare image in folder: %s\n", folder);
	std::string p = folder;

	for (const auto& entry : fs::directory_iterator(p))
	{
		std::string path = entry.path().generic_string();
		std::string filePng(path.begin(), path.end());
		lession.push_back(filePng);
	}
}

void train(ANN::CANN* ann, const char* file, double targetValue)
{
	SPNGImage img;
	if (loadPNG(file, &img))
	{
		double* input = getANNInput(&img);
		double output[] = { targetValue };
		ann->train(input, output, 1);
		delete input;
	}
}

int main()
{
	// input: 784
	// hidden: 32
	// output: 10
	int dim[] = { 784, 32, 10 };

	printf("Init ANN\n");

	ANN::EActivation activation = ANN::EActivation::Sigmoid;

	// init 3 layers
	ANN::CANN ann(dim, 3, activation);

	ann.LearnExpected = [activation](double* trainData, int trainId, double* expectedOutput, int numOutput)
	{
		float minValue = activation == ANN::EActivation::Tanh ? -1.0 : 0.0;
		float maxValue = 1.0;

		// ex: 3 => [0 0 0 1 0 0 0 0 0 0]
		int valueExpected = (int)trainData[trainId];
		for (int i = 0; i < 10; i++)
		{
			expectedOutput[i] = (i == valueExpected) ? maxValue : minValue;
		}
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

	std::vector<std::string> lession[10];
	int lessionLearned[10];
	for (int i = 0; i < 10; i++)
		lessionLearned[i] = 0;

	indexImageInFolder(lession[0], "./0");
	indexImageInFolder(lession[1], "./1");
	indexImageInFolder(lession[2], "./2");
	indexImageInFolder(lession[3], "./3");
	indexImageInFolder(lession[4], "./4");
	indexImageInFolder(lession[5], "./5");
	indexImageInFolder(lession[6], "./6");
	indexImageInFolder(lession[7], "./7");
	indexImageInFolder(lession[8], "./8");
	indexImageInFolder(lession[9], "./9");

	printf("Training...\n");

	long trainPercent = 0;
	int trainCount = 0;
	for (int i = 0; i < 10; i++)
	{
		int n = (int)lession[i].size();
		if (trainCount < n)
		{
			trainCount = n;
		}
	}

	int totalLearnTime = 1;
	for (int learnTime = 0; learnTime < totalLearnTime; learnTime++)
	{
		// learning trainCount (~4000) file/number
		for (int j = 0; j < trainCount; j++)
		{
			float f = ((float)j / (float)trainCount) * 100.0f;
			long percent = (int)f;
			if (trainPercent != percent)
			{
				trainPercent = percent;
				printf("%d%% - step (%d/%d)\n", trainPercent, learnTime, totalLearnTime);
			}

			// parallel learning from 0 - 9
			for (int i = 0; i < 10; i++)
			{
				int fileId = 0;

				if (lessionLearned[i] < lession[i].size())
					fileId = lessionLearned[i]++;
				else
					fileId = ANN::getRandom() % lession[i].size();

				std::string& file = lession[i][fileId];
				train(&ann, file.c_str(), (double)i);
			}
		}

		// reset lession and begin retry learn again
		for (int i = 0; i < 10; i++)
			lessionLearned[i] = 0;
	}

	printf("Test classification\n");

	int wrongCount = 0;
	for (int i = 0; i < 100; i++)
	{
		int a = rand() % 10;
		int numFile = (int)lession[a].size();
		int b = rand() % numFile;

		std::string& pngResource = lession[a][b];

		SPNGImage img;
		if (loadPNG(pngResource.c_str(), &img))
		{
			double* input = getANNInput(&img);
			double value = ann.predict(input);
			printf("- %s -> %lf\n", pngResource.c_str(), value);

			if ((int)value != a)
			{
				// if it wrong, try debug result
				double* output = ann.predictOutput(input);
				for (int i = 0; i < 10; i++)
				{
					printf("    + %d -> %d%%\n", i, (int)(output[i] * 100));
				}
				delete output;

				wrongCount++;
			}

			delete input;
		}
	}

	printf("Wrong: %d%%\n", wrongCount);

	return 1;
}