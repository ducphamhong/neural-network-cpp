#include "pch.h"
#include "ANN.h"
#include <ctime>

namespace ANN
{
	CANN::CANN(int* dim, int numLayer)
	{
		m_network = new SNetwork();
		m_network->NumLayers = numLayer;
		m_network->Layers.resize(numLayer);

		time_t t;
		srand((unsigned int)time(&t));

		for (int i = 0; i < numLayer; i++)
		{
			SLayer& layer = m_network->Layers[i];

			// create neural
			int numNeural = dim[i];
			layer.NumNeurals = numNeural;
			layer.Neurals.resize(numNeural);

			for (int j = 0; j < numNeural; j++)
			{
				SNeural& neural = layer.Neurals[j];

				// set aiases value
				neural.Biases = i == 0 ? 0.0 : 1.0;
			}

			// set connection weight for previous layer
			if (i >= 1)
			{
				SLayer& previousLayer = m_network->Layers[i - 1];
				for (int k = 0; k < previousLayer.NumNeurals; k++)
				{
					previousLayer.Neurals[k].Weights.resize(numNeural);
					for (int j = 0; j < numNeural; j++)
					{
						// init random value
						double randomValue = ((double)rand() / (double)RAND_MAX) * (1.0 / sqrt((double(previousLayer.NumNeurals + numNeural))));

						// add connection from previous to layer
						previousLayer.Neurals[k].Weights[j] = randomValue;
					}
				}
			}
		}
	}

	CANN::~CANN()
	{
		delete m_network;
	}

	double sigmoid(double x)
	{
		return 1 / (1 + exp(-x));
	}

	void CANN::feedForward(double* inputs)
	{
		// input layer
		SLayer& inputLayer = m_network->Layers[0];
		for (int i = 0; i < inputLayer.NumNeurals; i++)
		{
			inputLayer.Neurals[i].Output = inputs[i];
		}

		// compute output
		for (int i = 1; i < m_network->NumLayers; i++)
		{
			SLayer& previousLayer = m_network->Layers[i - 1];
			SLayer& layer = m_network->Layers[i];

			for (int j = 0; j < layer.NumNeurals; j++)
			{
				double sum = 0.0;
				for (int k = 0; k < previousLayer.NumNeurals; k++)
				{
					sum = sum + previousLayer.Neurals[k].Weights[j] * previousLayer.Neurals[k].Output;
				}
				sum = sum + layer.Neurals[j].Biases;
				layer.Neurals[j].Output = sigmoid(sum);
			}
		}
	}

	void CANN::train(double* inputs, double* targetOutput, int count, double learningRate)
	{
		if (LearnExpected == nullptr)
			return;

		SLayer& inputLayer = m_network->Layers[0];
		SLayer& outputLayer = m_network->Layers[m_network->NumLayers - 1];

		double* p = inputs;

		for (int learnId = 0; learnId < count; learnId++)
		{
			// feed forward
			feedForward(p);

			// learning expect
			for (int i = 0; i < outputLayer.NumNeurals; i++) {

				double expectedValue = LearnExpected(targetOutput, learnId, i);
				double observedValue = outputLayer.Neurals[i].Output;

				outputLayer.Neurals[i].Delta = observedValue * (1.0 - observedValue) * (observedValue - expectedValue);
			}

			// hidden layers delta computation
			for (int i = m_network->NumLayers - 2; i >= 0; i--)
			{
				SLayer& layer = m_network->Layers[i];
				SLayer& nextLayer = m_network->Layers[i + 1];

				for (int j = 0; j < layer.NumNeurals; j++)
				{
					double sum = 0;
					for (int k = 0; k < nextLayer.NumNeurals; k++)
					{
						sum = sum + nextLayer.Neurals[k].Delta * layer.Neurals[j].Weights[k];
					}
					layer.Neurals[j].Delta = layer.Neurals[j].Output * (1.0 - layer.Neurals[j].Output) * sum;
				}
			}

			// updating weigths and biases
			for (int i = m_network->NumLayers - 2; i >= 0; i--)
			{
				SLayer& layer = m_network->Layers[i];
				SLayer& nextLayer = m_network->Layers[i + 1];

				for (int j = 0; j < nextLayer.NumNeurals; j++)
				{
					for (int k = 0; k < layer.NumNeurals; k++)
					{
						layer.Neurals[k].Weights[j] -= learningRate * layer.Neurals[k].Output * nextLayer.Neurals[j].Delta;
					}

					nextLayer.Neurals[j].Biases -= learningRate * nextLayer.Neurals[j].Delta;
				}
			}

			// next learn input
			p += inputLayer.NumNeurals;
		}
	}

	double CANN::predict(double* inputs)
	{
		if (Predict == nullptr)
			return 0.0;

		feedForward(inputs);

		SLayer& outputLayer = m_network->Layers[m_network->NumLayers - 1];
		return Predict(outputLayer);
	}

	bool CANN::save(const char* filename)
	{
		return false;
	}

	bool CANN::load(const char* filename)
	{
		return false;
	}
}