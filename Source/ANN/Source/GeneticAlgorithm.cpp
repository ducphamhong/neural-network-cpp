#include "pch.h"
#include "GeneticAlgorithm.h"
#include <algorithm>

namespace ANN
{
	GeneticAlgorithm::GeneticAlgorithm(int topUnit) :
		m_topUnit(topUnit),
		m_mutateRate(1.0)
	{

	}

	GeneticAlgorithm::~GeneticAlgorithm()
	{
		destroyPopulation();
	}

	void GeneticAlgorithm::destroyPopulation()
	{
		for (Unit* unit : m_units)
		{
			delete unit->ANN;
			unit->ANN = NULL;
			delete unit;
		}
		m_units.clear();
	}

	void GeneticAlgorithm::createPopulation(int numUnit, const int* dim, int numLayer)
	{
		m_maxUnit = numUnit;
		m_network.clear();

		for (int i = 0; i < numLayer; i++)
			m_network.push_back(dim[i]);

		for (int i = 0; i < numUnit; i++)
		{
			Unit* unit = new Unit();
			unit->ANN = new CANN(dim, numLayer);
			m_units.push_back(unit);
		}
	}

	void GeneticAlgorithm::evolvePopulation()
	{
		selection();

		if (m_mutateRate == 1.0)
		{
			// first step
			if (m_units[0]->Win == false)
			{
				// We can destroy this bad population and try with another one.
				destroyPopulation();

				std::vector<int> n = m_network;
				createPopulation(m_maxUnit, n.data(), (int)n.size());
			}
		}
		else
		{
			m_mutateRate = 0.2;
		}

		for (int i = m_topUnit; i < m_maxUnit; i++)
		{
			Unit* newUnit = NULL;

			if (i == m_topUnit)
			{
				newUnit = crossOver(m_units[0], m_units[1]);
			}
			else if (i < m_maxUnit - 2)
			{
				int r1 = rand() % m_topUnit;
				int r2 = rand() % m_topUnit;

				newUnit = crossOver(m_units[r1], m_units[r2]);
			}
			else
			{
				int r = rand() % m_topUnit;

				newUnit = cloneUnit(m_units[r]);
			}

			mutation(newUnit);

			// destroy this bad unit
			delete m_units[i]->ANN;
			delete m_units[i];

			// replace good gene unit
			m_units[i] = newUnit;
		}
	}

	bool sort(const Unit* a, const Unit* b)
	{
		return a->Scored > a->Scored;
	}

	void GeneticAlgorithm::selection()
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

	Unit* GeneticAlgorithm::crossOver(Unit* parentA, Unit* parentB)
	{
		Unit* unit = new Unit();

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

			int cutPoint = rand() % layer.NumNeurals;

			for (int i = 0; i < layer.NumNeurals; i++)
			{
				if (i < cutPoint)
					layer.Neurals[i].Biases = layerA.Neurals[i].Biases;
				else
					layer.Neurals[i].Biases = layerB.Neurals[i].Biases;

				int select = rand() % 2;
				for (int j = 0; j < layer.Neurals[i].NumWeights; j++)
				{
					float a = layerA.Neurals[i].Weights[j];
					float b = layerB.Neurals[i].Weights[j];

					layer.Neurals[i].Weights[j] = select == 1 ? a : b;
				}
			}
		}

		return unit;
	}

	Unit* GeneticAlgorithm::cloneUnit(Unit* parent)
	{
		Unit* unit = new Unit();

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

	double rand01()
	{
		return (rand() % 10000) / 10000.0;
	}

	double mutate(double gene, double mutateRate)
	{
		double r = rand01();
		if (r < mutateRate)
		{
			double mutateFactor = 1 + ((rand01() - 0.5) * 3.0 + (rand01() - 0.5));
			gene *= mutateFactor;
		}
		return gene;
	}

	void GeneticAlgorithm::mutation(Unit* unit)
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