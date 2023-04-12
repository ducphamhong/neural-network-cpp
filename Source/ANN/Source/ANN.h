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

#pragma once

#include <vector>
#include <functional>

#include "MemoryStream.h"

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

	enum class EActivation
	{
		Sigmoid,
		Tanh
	};

	class CANN
	{
	protected:
		SNetwork* m_network;
		EActivation m_activation;

		double (*activation)(double);
		double (*derivative)(double);

	public:

		// Return expected learning on neutonOutput
		std::function<void(double* trainData, int trainId, double* expectedOutput, int numOutput)> LearnExpected;

		// Convert output layer to double
		std::function<double(const double* output, int numOutput)> Predict;

	public:
		CANN(const int* dim, int numLayer, EActivation activation = EActivation::Sigmoid);

		void init(const int* dim, int numLayer);

		virtual ~CANN();

		void train(double* inputs, double* targetOutput, int count, double learningRate = 0.25, double momentum = 0.2);

		void feedForward(double* inputs);

		double predict(double* inputs);

		double* predictOutput(double* inputs);

		SNetwork* getNetwork()
		{
			return m_network;
		}

		void serialize(CMemoryStream* io);

		bool deserialize(CMemoryStream* io);
	};
}