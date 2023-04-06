#pragma once

#include "ANN.h"

namespace ANN
{
	struct SUnit
	{
		CANN* ANN;
		bool Good;
		double Scored;
		double BestScored;
		bool TopUnit;
		int ID;

		SUnit()
		{
			ID = 0;
			Scored = 0.0;
			BestScored = 0.0f;
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

		int m_id;
		int m_maxUnit;
		int m_topUnit;
		double m_mutateRate;
		double m_maxPertubation;

	public:
		CGeneticAlgorithm(int topUnit = 4);

		virtual ~CGeneticAlgorithm();

		void destroyPopulation();

		void createPopulation(int numUnit, const int* dim, int numLayer);

		void evolvePopulation();

		void selection();

		std::vector<SUnit*> crossOver(SUnit* parentA, SUnit* parentB);

		SUnit* cloneUnit(SUnit* parent);

		void mutation(SUnit* unit);

		std::vector<SUnit*>& get()
		{
			return m_units;
		}

	protected:

	};
}