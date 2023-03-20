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

void indexImageInFolder(std::vector<std::string>& lession, const char* folder)
{
	printf("Prepare image in folder: %s\n", folder);
	std::string p = folder;

	for (const auto& entry : fs::directory_iterator(p))
	{
		std::wstring path = entry.path().c_str();
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

	int trainCount = 4000;
	long trainPercent = 0;

	// learning ~4000 file/number
	for (int j = 0; j < trainCount; j++)
	{
		float f = ((float)j / (float)trainCount) * 100.0f;
		long percent = (int)f;
		if (trainPercent != percent)
		{
			trainPercent = percent;
			printf("%d %%\n", trainPercent);
		}

		// parallel learning from 0 - 9
		for (int i = 0; i < 10; i++)
		{
			if (lessionLearned[i] < lession[i].size())
			{
				std::string& file = lession[i][lessionLearned[i]];
				train(&ann, file.c_str(), (double)i);
				lessionLearned[i]++;
			}
		}
	}

	printf("Test classification\n");
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
			delete input;
		}
	}

	return 1;
}