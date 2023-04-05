#pragma once

#include "ANN.h"

namespace ANN
{
	struct SUnit
	{
		CANN* ANN;
		bool Good;
		double Scored;
		bool TopUnit;

		SUnit()
		{
			Scored = 0.0;
			TopUnit = false;
			Good = false;
			ANN = NULL;
		}
	};

	class CGeneticAlgorithm
	{
	protected:
		std::vector<int> m_network;
		std::vector<SUnit*> m_units;

		int m_maxUnit;
		int m_topUnit;
		double m_mutateRate;

	public:
		CGeneticAlgorithm(int topUnit = 4);

		virtual ~CGeneticAlgorithm();

		void destroyPopulation();

		void createPopulation(int numUnit, const int* dim, int numLayer);

		void evolvePopulation();

		void selection();

		SUnit* crossOver(SUnit* parentA, SUnit* parentB);

		SUnit* cloneUnit(SUnit* parent);

		void mutation(SUnit* unit);

		std::vector<SUnit*>& get()
		{
			return m_units;
		}

	protected:

	};
}