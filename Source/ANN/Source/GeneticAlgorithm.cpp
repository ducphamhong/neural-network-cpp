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
#include "GeneticAlgorithm.h"
#include "Random.h"
#include <algorithm>

// Reference: https://github.com/ssusnic/Machine-Learning-Flappy-Bird

namespace ANN
{
	CGeneticAlgorithm::CGeneticAlgorithm(EActivation activation, int topUnit) :
		m_topUnit(topUnit),
		m_maxUnit(0),
		m_id(0),
		m_mutateRate(0.2),
		m_maxPertubation(4.0),
		m_activation(activation)
	{

	}

	CGeneticAlgorithm::~CGeneticAlgorithm()
	{
		destroyPopulation();
	}

	void CGeneticAlgorithm::destroyPopulation()
	{
		for (SUnit* unit : m_units)
		{
			delete unit->ANN;
			unit->ANN = NULL;
			delete unit;
		}
		m_units.clear();
	}

	void CGeneticAlgorithm::reset()
	{
		destroyPopulation();

		int numLayer = (int)m_networkDim.size();

		for (int i = 0; i < m_maxUnit; i++)
		{
			SUnit* unit = new SUnit();
			unit->ANN = new CANN(m_networkDim.data(), numLayer, m_activation);
			unit->ID = ++m_id;
			m_units.push_back(unit);
		}
	}

	void CGeneticAlgorithm::createPopulation(int numUnit, const int* dim, int numLayer)
	{
		m_maxUnit = numUnit;
		m_networkDim.clear();

		for (int i = 0; i < numLayer; i++)
			m_networkDim.push_back(dim[i]);

		for (int i = 0; i < numUnit; i++)
		{
			SUnit* unit = new SUnit();
			unit->ANN = new CANN(dim, numLayer, m_activation);
			unit->ID = ++m_id;
			m_units.push_back(unit);
		}
	}

	void CGeneticAlgorithm::evolvePopulation()
	{
		selection();

		for (int i = m_topUnit; i < m_maxUnit; i++)
		{
			std::vector<SUnit*> newUnit;

			if (i == m_topUnit && m_topUnit > 1)
			{
				newUnit = crossOver(m_units[0], m_units[1]);
			}
			else if (i < m_maxUnit - 2)
			{
				int r = getRandom() % m_topUnit;
				if (m_topUnit > 1)
				{
					while (r == 0)
					{
						r = getRandom() % m_topUnit;
					}
				}
				newUnit = crossOver(m_units[0], m_units[r]);
			}
			else
			{
				int randomTop = getRandom() % 2;
				newUnit.push_back(cloneUnit(m_units[randomTop]));
			}

			for (int j = 0, m = (int)newUnit.size(); j < m; j++)
			{
				mutation(newUnit[j]);

				// destroy this bad unit
				if (i + j < m_maxUnit)
				{
					delete m_units[i + j]->ANN;
					delete m_units[i + j];

					// replace new gene unit
					m_units[i + j] = cloneUnit(newUnit[j]);
				}

				delete newUnit[j];
			}

			// next num child
			i = i + ((int)newUnit.size() - 1);
		}

		// reset score and retest
		int numUnit = (int)m_units.size();
		for (int i = 0; i < numUnit; i++)
		{
			m_units[i]->Good = false;
			m_units[i]->TopUnit = i < m_topUnit;
			m_units[i]->Scored = 0.0;

			if (m_units[i]->ID == 0)
			{
				m_units[i]->ID = ++m_id;
			}
		}
	}

	bool sort(const SUnit* a, const SUnit* b)
	{
		float scoreA = a->Scored * 0.8;
		float scoreB = b->Scored * 0.8;

		if (a->BestScored > 0 && b->BestScored > 0)
		{
			scoreA = scoreA + a->BestScored * 0.2;
			scoreB = scoreB + b->BestScored * 0.2;
		}

		return scoreA > scoreB;
	}

	void CGeneticAlgorithm::selection()
	{
		std::sort(m_units.begin(), m_units.end(), sort);

		int numUnit = (int)m_units.size();
		for (int i = 0; i < numUnit; i++)
		{
			if (i < m_topUnit)
			{
				m_units[i]->TopUnit = true;
			}
			else
			{
				m_units[i]->TopUnit = false;
			}
		}
	}

	void CGeneticAlgorithm::setMutate(double mutateRate, double maxPertubation)
	{
		m_mutateRate = mutateRate;
		m_maxPertubation = maxPertubation;
	}

	std::vector<SUnit*> CGeneticAlgorithm::crossOver(SUnit* parentA, SUnit* parentB)
	{
		std::vector<SUnit*> ret;

		SUnit* unit1 = new SUnit();
		SUnit* unit2 = new SUnit();

		int numLayer = (int)m_networkDim.size();
		unit1->ANN = new CANN(m_networkDim.data(), numLayer, m_activation);
		unit2->ANN = new CANN(m_networkDim.data(), numLayer, m_activation);

		SNetwork* network1 = unit1->ANN->getNetwork();
		SNetwork* network2 = unit2->ANN->getNetwork();

		SNetwork* networkA = parentA->ANN->getNetwork();
		SNetwork* networkB = parentB->ANN->getNetwork();

		for (int i = 0; i < numLayer; i++)
		{
			SLayer& layer1 = network1->Layers[i];
			SLayer& layer2 = network2->Layers[i];

			SLayer& layerA = networkA->Layers[i];
			SLayer& layerB = networkB->Layers[i];

			int cutPoint = getRandom() % layer1.NumNeurals;

			for (int i = 0; i < layer1.NumNeurals; i++)
			{
				if (i < cutPoint)
				{
					layer1.Neurals[i].Biases = layerA.Neurals[i].Biases;
					layer2.Neurals[i].Biases = layerB.Neurals[i].Biases;
				}
				else
				{
					layer1.Neurals[i].Biases = layerB.Neurals[i].Biases;
					layer2.Neurals[i].Biases = layerA.Neurals[i].Biases;
				}

				for (int j = 0; j < layer1.Neurals[i].NumWeights; j++)
				{
					double a = layerA.Neurals[i].Weights[j];
					double b = layerB.Neurals[i].Weights[j];

					layer1.Neurals[i].Weights[j] = i < cutPoint ? a : b;
					layer2.Neurals[i].Weights[j] = i < cutPoint ? b : a;
				}
			}
		}

		ret.push_back(unit1);
		ret.push_back(unit2);

		return ret;
	}

	SUnit* CGeneticAlgorithm::cloneUnit(SUnit* parent)
	{
		SUnit* unit = new SUnit();

		int numLayer = (int)m_networkDim.size();
		unit->ANN = new CANN(m_networkDim.data(), numLayer, m_activation);

		SNetwork* network = unit->ANN->getNetwork();
		SNetwork* networkParent = parent->ANN->getNetwork();

		for (int i = 0; i < numLayer; i++)
		{
			SLayer& layer = network->Layers[i];
			SLayer& layerParent = networkParent->Layers[i];

			for (int i = 0; i < layer.NumNeurals; i++)
			{
				layer.Neurals[i].Biases = layerParent.Neurals[i].Biases;
				for (int j = 0; j < layer.Neurals[i].NumWeights; j++)
				{
					layer.Neurals[i].Weights[j] = layerParent.Neurals[i].Weights[j];
				}
			}
		}

		return unit;
	}

	double mutate(double gene, double mutateRate, double maxPertubation)
	{
		double r = getRandom01();
		if (r < mutateRate)
		{
			// double mutateFactor = getRandom01() * 2.0 - 1.0;
			// gene = gene + mutateFactor * maxPertubation;

			double mutateFactor = 1.0 + (getRandom01() - 0.5) * maxPertubation;
			gene = gene * mutateFactor;
		}
		return gene;
	}

	void CGeneticAlgorithm::mutation(SUnit* unit)
	{
		SNetwork* network = unit->ANN->getNetwork();
		int numLayer = (int)m_networkDim.size();

		for (int i = 0; i < numLayer; i++)
		{
			SLayer& layer = network->Layers[i];

			for (int i = 0; i < layer.NumNeurals; i++)
			{
				layer.Neurals[i].Biases = mutate(layer.Neurals[i].Biases, m_mutateRate, m_maxPertubation);

				for (int j = 0; j < layer.Neurals[i].NumWeights; j++)
				{
					layer.Neurals[i].Weights[j] = mutate(layer.Neurals[i].Weights[j], m_mutateRate, m_maxPertubation);
				}
			}
		}
	}

	void CGeneticAlgorithm::serialize(CMemoryStream* io)
	{
		// write info of network
		io->writeInt((int)m_networkDim.size());
		for (int i = 0, n = (int)m_networkDim.size(); i < n; i++)
		{
			io->writeInt(m_networkDim[i]);
		}

		// write generic
		io->writeInt(m_maxUnit);
		io->writeInt(m_topUnit);

		// write unit info
		int numUnit = (int)m_units.size();
		io->writeInt(numUnit);
		for (int i = 0; i < numUnit; i++)
		{
			SUnit* unit = m_units[i];
			io->writeDouble(unit->BestScored);
			io->writeInt(unit->ID);

			// write ANN data
			CMemoryStream m;
			m_units[i]->ANN->serialize(&m);
			io->writeInt(m.getSize());
			io->writeData(m.getData(), m.getSize());
		}
	}

	bool CGeneticAlgorithm::deserialize(CMemoryStream* io)
	{
		destroyPopulation();

		// read network dimension
		int n = io->readInt();
		m_networkDim.clear();
		for (int i = 0; i < n; i++)
		{
			m_networkDim.push_back(io->readInt());
		}

		// read info
		m_maxUnit = io->readInt();
		m_topUnit = io->readInt();

		int numUnit = io->readInt();
		for (int i = 0; i < numUnit; i++)
		{
			SUnit* unit = new SUnit();
			unit->BestScored = io->readDouble();
			unit->ID = io->readInt();

			// update global id
			if (m_id < unit->ID)
				m_id = unit->ID;

			// read ANN data
			int size = io->readInt();
			unsigned char* data = new unsigned char[size];
			io->readData(data, size);
			CMemoryStream m(data, size);
			unit->ANN = new CANN(m_networkDim.data(), (int)m_networkDim.size(), m_activation);
			unit->ANN->deserialize(&m);
			delete[]data;

			m_units.push_back(unit);
		}

		return false;
	}
}