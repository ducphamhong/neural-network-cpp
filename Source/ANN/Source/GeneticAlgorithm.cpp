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
		m_id(),
		m_mutateRate(0.3),
		m_maxPertubation(0.5)
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

			if (i == m_topUnit)
			{
				newUnit = crossOver(m_units[0], m_units[1]);
			}
			else if (i < m_maxUnit - 2)
			{
				int r1 = getRandom() % m_topUnit;
				int r2 = getRandom() % m_topUnit;

				newUnit = crossOver(m_units[0], m_units[r2]);
			}
			else
			{
				newUnit.push_back(cloneUnit(m_units[0]));
			}

			for (int j = 0, m = (int)newUnit.size(); j < m; j++)
			{
				mutation(newUnit[j]);

				// destroy this old generate unit
				if (i + j < m_maxUnit)
				{
					delete m_units[i + j]->ANN;
					delete m_units[i + j];

					// replace good gene unit
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
			m_units[i]->TopUnit = false;

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

	std::vector<SUnit*> CGeneticAlgorithm::crossOver(SUnit* parentA, SUnit* parentB)
	{
		std::vector<SUnit*> ret;

		SUnit* unit1 = new SUnit();
		SUnit* unit2 = new SUnit();

		int numLayer = (int)m_network.size();
		unit1->ANN = new CANN(m_network.data(), numLayer);
		unit2->ANN = new CANN(m_network.data(), numLayer);

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

		int numLayer = (int)m_network.size();
		unit->ANN = new CANN(m_network.data(), numLayer);

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
			double mutateFactor = getRandom01() * 2.0 - 1.0;
			gene = gene + mutateFactor * maxPertubation;
		}
		return gene;
	}

	void CGeneticAlgorithm::mutation(SUnit* unit)
	{
		SNetwork* network = unit->ANN->getNetwork();
		int numLayer = (int)m_network.size();

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
}