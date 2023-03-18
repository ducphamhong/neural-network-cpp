#pragma once

#include <vector>
#include <functional>

namespace ANN
{
	struct SNeural
	{
		double Output;
		double Biases;
		double Delta;
		std::vector<double> Weights;
	};

	struct SLayer
	{
		int NumNeurals;
		std::vector<SNeural> Neurals;
	};

	struct SNetwork
	{
		int NumLayers;
		std::vector<SLayer> Layers;
	};

	class CANN
	{
	protected:
		SNetwork* m_network;

	public:

		// Return expected learning on neutonOutput
		std::function<double(double* trainData, int trainId, int neuralOutputId)> LearnExpected;

		// Convert output layer to double
		std::function<double(const SLayer&)> Predict;

	public:
		CANN(int* dim, int numLayer);

		virtual ~CANN();

		void train(double* inputs, double* targetOutput, int count, double learningRate = 0.25);

		void feedForward(double* inputs);

		double predict(double* inputs);

		bool save(const char* filename);

		bool load(const char* filename);
	};
}