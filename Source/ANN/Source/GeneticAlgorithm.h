#pragma once

#include "ANN.h"

namespace ANN
{
	struct Unit
	{
		CANN* ANN;

		bool Win;

		float Scored;

		bool TopUnit;

		Unit()
		{
			Scored = 0.0f;
			TopUnit = false;
			Win = false;
			ANN = NULL;
		}
	};

	class GeneticAlgorithm
	{
	protected:
		std::vector<int> m_network;
		std::vector<Unit*> m_units;

		int m_maxUnit;
		int m_topUnit;
		double m_mutateRate;

	public:
		GeneticAlgorithm(int topUnit = 4);

		virtual ~GeneticAlgorithm();

		void destroyPopulation();

		void createPopulation(int numUnit, const int* dim, int numLayer);

		void evolvePopulation();

		void selection();

		Unit* crossOver(Unit* parentA, Unit* parentB);

		Unit* cloneUnit(Unit* parent);

		void mutation(Unit* unit);

		std::vector<Unit*>& get()
		{
			return m_units;
		}

	protected:

	};
}