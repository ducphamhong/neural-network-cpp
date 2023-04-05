#include "pch.h"
#include "GeneticAlgorithm.h"
#include "Random.h"
#include <algorithm>

// Reference: https://github.com/ssusnic/Machine-Learning-Flappy-Bird

namespace ANN
{
	CGeneticAlgorithm::CGeneticAlgorithm(int topUnit) :
		m_topUnit(topUnit),
		m_maxUnit(0),
		m_mutateRate(0.2)
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

	void CGeneticAlgorithm::createPopulation(int numUnit, const int* dim, int numLayer)
	{
		m_maxUnit = numUnit;
		m_network.clear();

		for (int i = 0; i < numLayer; i++)
			m_network.push_back(dim[i]);

		for (int i = 0; i < numUnit; i++)
		{
			SUnit* unit = new SUnit();
			unit->ANN = new CANN(dim, numLayer);
			m_units.push_back(unit);
		}
	}

	void CGeneticAlgorithm::evolvePopulation()
	{
		selection();

		for (int i = m_topUnit; i < m_maxUnit; i++)
		{
			SUnit* newUnit = NULL;

			if (i == m_topUnit)
			{
				newUnit = crossOver(m_units[0], m_units[1]);
			}
			else if (i < m_maxUnit - 2)
			{
				int r1 = getRandom() % m_topUnit;
				int r2 = getRandom() % m_topUnit;

				newUnit = crossOver(m_units[r1], m_units[r2]);
			}
			else
			{
				newUnit = cloneUnit(m_units[0]);
			}

			mutation(newUnit);

			// destroy this bad unit
			delete m_units[i]->ANN;
			delete m_units[i];

			// replace good gene unit
			m_units[i] = newUnit;
		}

		// reset score and retest
		int numUnit = (int)m_units.size();
		for (int i = 0; i < numUnit; i++)
		{
			m_units[i]->Good = false;
			m_units[i]->Scored = 0.0;
		}
	}

	bool sort(const SUnit* a, const SUnit* b)
	{
		return a->Scored > b->Scored;
	}

	void CGeneticAlgorithm::selection()
	{
		std::sort(m_units.begin(), m_units.end(), sort);

		int numUnit = (int)m_units.size();
		for (int i = 0; i < numUnit; i++)
		{
			if (i < numUnit)
			{
				m_units[i]->TopUnit = true;
			}
			else
			{
				m_units[i]->TopUnit = false;
			}
		}
	}

	SUnit* CGeneticAlgorithm::crossOver(SUnit* parentA, SUnit* parentB)
	{
		SUnit* unit = new SUnit();

		int numLayer = (int)m_network.size();
		unit->ANN = new CANN(m_network.data(), numLayer);

		SNetwork* network = unit->ANN->getNetwork();
		SNetwork* networkA = parentA->ANN->getNetwork();
		SNetwork* networkB = parentA->ANN->getNetwork();

		for (int i = 1; i < numLayer - 1; i++)
		{
			SLayer& layer = network->Layers[i];
			SLayer& layerA = networkA->Layers[i];
			SLayer& layerB = networkB->Layers[i];

			int cutPoint = getRandom() % layer.NumNeurals;

			for (int i = 0; i < layer.NumNeurals; i++)
			{
				if (i < cutPoint)
					layer.Neurals[i].Biases = layerA.Neurals[i].Biases;
				else
					layer.Neurals[i].Biases = layerB.Neurals[i].Biases;

				int select = getRandom() % 2;
				for (int j = 0; j < layer.Neurals[i].NumWeights; j++)
				{
					double a = layerA.Neurals[i].Weights[j];
					double b = layerB.Neurals[i].Weights[j];

					layer.Neurals[i].Weights[j] = select == 1 ? a : b;
				}
			}
		}

		return unit;
	}

	SUnit* CGeneticAlgorithm::cloneUnit(SUnit* parent)
	{
		SUnit* unit = new SUnit();

		int numLayer = (int)m_network.size();
		unit->ANN = new CANN(m_network.data(), numLayer);

		SNetwork* network = unit->ANN->getNetwork();
		SNetwork* networkParent = parent->ANN->getNetwork();

		for (int i = 1; i < numLayer - 1; i++)
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

	double mutate(double gene, double mutateRate)
	{
		double r = getRandom01();
		if (r < mutateRate)
		{
			double mutateFactor = 1.0 + (getRandom01() - 0.5) * 0.001;
			gene *= mutateFactor;
		}
		return gene;
	}

	void CGeneticAlgorithm::mutation(SUnit* unit)
	{
		SNetwork* network = unit->ANN->getNetwork();
		int numLayer = (int)m_network.size();

		for (int i = 1; i < numLayer - 1; i++)
		{
			SLayer& layer = network->Layers[i];

			for (int i = 0; i < layer.NumNeurals; i++)
			{
				layer.Neurals[i].Biases = mutate(layer.Neurals[i].Biases, m_mutateRate);

				for (int j = 0; j < layer.Neurals[i].NumWeights; j++)
				{
					layer.Neurals[i].Weights[j] = mutate(layer.Neurals[i].Weights[j], m_mutateRate);
				}
			}
		}
	}
}