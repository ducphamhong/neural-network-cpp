#pragma once

#include <vector>
#include <functional>

namespace ANN
{
	struct SNeural
	{
		int NumWeights;
		double Biases;
		double* Weights;
		double Output;
		double Delta;

		SNeural()
		{
			NumWeights = 0;
			Biases = 0.0;
			Weights = NULL;
			Output = 0.0;
			Delta = 0.0;
		}

		~SNeural()
		{
			if (Weights)
				delete[]Weights;
		}
	};

	struct SLayer
	{
		int NumNeurals;
		SNeural* Neurals;

		SLayer()
		{
			NumNeurals = 0;
			Neurals = NULL;
		}

		~SLayer()
		{
			if (Neurals)
				delete[]Neurals;
		}
	};

	struct SNetwork
	{
		int NumLayers;
		SLayer* Layers;

		SNetwork()
		{
			NumLayers = 0;
			Layers = NULL;
		}

		~SNetwork()
		{
			if (Layers)
				delete[]Layers;
		}
	};

	class CANN
	{
	protected:
		SNetwork* m_network;

	public:

		// Return expected learning on neutonOutput
		std::function<void(double* trainData, int trainId, double* expectedOutput, int numOutput)> LearnExpected;

		// Convert output layer to double
		std::function<double(const double* output, int numOutput)> Predict;

	public:
		CANN(const int* dim, int numLayer);

		virtual ~CANN();

		void train(double* inputs, double* targetOutput, int count, double learningRate = 0.25);

		void feedForward(double* inputs);

		double predict(double* inputs);

		SNetwork* getNetwork()
		{
			return m_network;
		}
	};
}