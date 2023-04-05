#pragma once

#include "lib.h"
#include "pipe.h"

#ifdef AI_LEARNING_INPUT
#include "GeneticAlgorithm.h"
#endif

using namespace std;

class doge :LTexture
{
public:
	bool init(bool isDark);

	void render();

	void Free();

	bool isDie()
	{
		return die;
	}

	void restart()
	{
		die = false;
		distance = 0.0f;
		score = 0;
	}

	void resetTime()
	{
		time = 0;
	}

	void fall();

	void update(short int pileWidth, short int pileHeight);

#ifdef AI_LEARNING_INPUT
	void setAIUnit(ANN::SUnit* u);
#endif

private:

#ifdef AI_LEARNING_INPUT
	ANN::SUnit* unit;
#endif

	int distance;
	int score;
	bool die;
	short int angle, time, x0;
	short int ahead = 0;
	int jumpTime;
	string saved_path = "";
	position posDoge;
};