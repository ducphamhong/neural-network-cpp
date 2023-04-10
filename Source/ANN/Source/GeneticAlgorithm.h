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
		std::vector<int> m_networkDim;
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

		void serialize(CMemoryStream* io);

		bool deserialize(CMemoryStream* io);
	};
}