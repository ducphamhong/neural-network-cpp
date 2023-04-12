/*
!@
MIT License

Copyright (c) 2021 Skylicht Technology CO., LTD

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction, including without limitation the Rights to use, copy, modify,
merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

This file is part of the "Skylicht Engine".
https://github.com/skylicht-lab/skylicht-engine
!#
*/

#include "pch.h"
#include "ANN.h"
#include "Random.h"

namespace ANN
{
	// note test
	// sigmoid: classification better
	// tanh: think and selection better

	double activationSigmoid(double x)
	{
		return 1 / (1 + exp(-x));
	}

	double derivativeSigmoid(double x)
	{
		return x * (1.0 - x);
	}

	double activationTanh(double x)
	{
		return std::tanh(x);
	}

	double derivativeTanh(double x)
	{
		double t = std::tanh(x);
		return 1.0 - t * t;
	}

	double activationRelu(double x)
	{
		return x > 0.0 ? x : 1.0;
	}

	double derivativeRelu(double x)
	{
		return x <= 0.0 ? 0.0 : 1.0;
	}

	CANN::CANN(const int* dim, int numLayer, EActivation a) :
		m_network(NULL),
		m_activation(a)
	{
		init(dim, numLayer);
	}

	CANN::~CANN()
	{
		delete m_network;
	}

	void CANN::init(const int* dim, int numLayer)
	{
		if (m_network)
			delete m_network;

		m_network = new SNetwork();
		m_network->NumLayers = numLayer;
		m_network->Layers = new SLayer[numLayer];

		for (int i = 0; i < numLayer; i++)
		{
			SLayer& layer = m_network->Layers[i];

			switch (m_activation)
			{
			case EActivation::Sigmoid:
				layer.Activation = EActivation::Sigmoid;
				layer.activation = &activationSigmoid;
				layer.derivative = &derivativeSigmoid;
				break;
			case EActivation::Tanh:
				layer.Activation = EActivation::Tanh;
				layer.activation = &activationTanh;
				layer.derivative = &derivativeTanh;
				break;
			case EActivation::Relu:
			{
				if (i == numLayer - 1)
				{
					// output layer
					// ref: https://github.com/manishdhakal/Backpropagation/blob/master/ANN/main.cpp
					// ref: https://www.jeremyong.com/cpp/machine-learning/2020/10/23/cpp-neural-network-in-a-weekend
					layer.Activation = EActivation::Sigmoid;
					layer.activation = &activationSigmoid;
					layer.derivative = &derivativeSigmoid;
				}
				else
				{
					// hidden layer
					layer.Activation = EActivation::Relu;
					layer.activation = &activationRelu;
					layer.derivative = &derivativeRelu;
				}
			}
			break;
			default:
				break;
			}

			// create neural
			int numNeural = dim[i];
			layer.NumNeurals = numNeural;
			layer.Neurals = new SNeural[numNeural];

			for (int j = 0; j < numNeural; j++)
			{
				// set Biases value
				double r = 0.0;

				if (layer.Activation == EActivation::Relu)
					r = 0.0;
				else
					r = getRandom01() * 2.0 - 1.0;

				layer.Neurals[j].Biases = i == 0 ? 0.0 : r;
			}

			// set connection weight for previous layer
			if (i >= 1)
			{
				SLayer& previousLayer = m_network->Layers[i - 1];
				for (int k = 0; k < previousLayer.NumNeurals; k++)
				{
					previousLayer.Neurals[k].NumWeights = numNeural;
					previousLayer.Neurals[k].Weights = new double[numNeural];

					for (int j = 0; j < numNeural; j++)
					{
						double r = 0.0;
						if (previousLayer.Activation == EActivation::Relu)
						{
							double n = (double)previousLayer.NumNeurals;
							r = getRandom01() * sqrt(2.0 / n);
						}
						else
						{
							double n = (double)layer.NumNeurals + (double)previousLayer.NumNeurals;
							r = getRandom01() * 2.0 - 1.0;
							r = r / sqrt(n);
						}
						previousLayer.Neurals[k].Weights[j] = r;
					}
				}
			}
		}
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
				layer.Neurals[j].Output = layer.activation(sum);
			}
		}
	}

	void CANN::train(double* inputs, double* targetOutput, int count, double learningRate, double momentum)
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

			double* expectedOutput = new double[outputLayer.NumNeurals];
			LearnExpected(targetOutput, learnId, expectedOutput, outputLayer.NumNeurals);

			// learning expect
			for (int i = 0; i < outputLayer.NumNeurals; i++)
			{
				double expectedValue = expectedOutput[i];
				double observedValue = outputLayer.Neurals[i].Output;
				outputLayer.Neurals[i].Delta = outputLayer.derivative(observedValue) * (expectedValue - observedValue);
			}

			delete[]expectedOutput;

			// hidden layers delta computation
			for (int i = m_network->NumLayers - 2; i >= 0; i--)
			{
				SLayer& layer = m_network->Layers[i];
				SLayer& nextLayer = m_network->Layers[i + 1];

				// https://github.com/huangzehao/SimpleNeuralNetwork/blob/master/src/neural-net.cpp
				// sumDOW
				for (int j = 0; j < layer.NumNeurals; j++)
				{
					double sum = 0.0;
					for (int k = 0; k < nextLayer.NumNeurals; k++)
					{
						sum = sum + layer.Neurals[j].Weights[k] * nextLayer.Neurals[k].Delta;
					}
					layer.Neurals[j].Delta = layer.derivative(layer.Neurals[j].Output) * sum;
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
						layer.Neurals[k].Weights[j] += learningRate * layer.Neurals[k].Output * nextLayer.Neurals[j].Delta;
					}

					nextLayer.Neurals[j].Biases += learningRate * nextLayer.Neurals[j].Delta;
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
		double* output = new double[outputLayer.NumNeurals];
		for (int i = 0; i < outputLayer.NumNeurals; i++)
			output[i] = outputLayer.Neurals[i].Output;
		double ret = Predict(output, outputLayer.NumNeurals);
		delete[]output;
		return ret;
	}

	double* CANN::predictOutput(double* inputs)
	{
		feedForward(inputs);

		SLayer& outputLayer = m_network->Layers[m_network->NumLayers - 1];
		double* outputs = new double[outputLayer.NumNeurals];
		for (int i = 0; i < outputLayer.NumNeurals; i++)
			outputs[i] = outputLayer.Neurals[i].Output;
		return outputs;
	}

	void CANN::serialize(CMemoryStream* io)
	{
		// write info of network
		io->writeInt(m_network->NumLayers);
		for (int i = 0; i < m_network->NumLayers; i++)
		{
			io->writeInt(m_network->Layers[i].NumNeurals);
		}

		// write neurals info
		for (int i = 0; i < m_network->NumLayers; i++)
		{
			SLayer& layer = m_network->Layers[i];

			int numNeural = layer.NumNeurals;
			for (int j = 0; j < numNeural; j++)
			{
				io->writeDouble(layer.Neurals[j].Biases);

				if (i < m_network->NumLayers - 1)
				{
					int numWeight = layer.Neurals[j].NumWeights;
					for (int k = 0; k < numWeight; k++)
						io->writeDouble(layer.Neurals[j].Weights[k]);
				}
			}
		}
	}

	bool CANN::deserialize(CMemoryStream* io)
	{
		// read and test network info
		int numLayer = io->readInt();
		if (numLayer != m_network->NumLayers)
			return false;

		int* dim = new int[numLayer];
		for (int i = 0; i < numLayer; i++)
		{
			if (m_network->Layers[i].NumNeurals != io->readInt())
				return false;
		}

		// read neurals info
		for (int i = 0; i < m_network->NumLayers; i++)
		{
			SLayer& layer = m_network->Layers[i];

			int numNeural = layer.NumNeurals;
			for (int j = 0; j < numNeural; j++)
			{
				layer.Neurals[j].Biases = io->readDouble();
				if (i < m_network->NumLayers - 1)
				{
					int numWeight = layer.Neurals[j].NumWeights;
					for (int k = 0; k < numWeight; k++)
						layer.Neurals[j].Weights[k] = io->readDouble();
				}
			}
		}

		return true;
	}
}