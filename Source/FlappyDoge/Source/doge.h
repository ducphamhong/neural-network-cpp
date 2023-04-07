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

	void kill()
	{
		willKill = true;
	}
	
	void setFallInGround()
	{
		fallInGround = true;
	}

	void restart()
	{
		die = false;
		fallInGround = false;
		distance = 0.0f;
		score = 0;
	}

	void resetTime()
	{
		time = 0;
	}

	void fall();

	void update(short int pileWidth, short int pileHeight);

	void reportDie(double distanceToTarget);

	bool isFallInGround()
	{
		return fallInGround;
	}

#ifdef AI_LEARNING_INPUT
	void setAIUnit(ANN::SUnit* u);

	ANN::SUnit* getAIUnit()
	{
		return unit;
	}
#endif

private:

#ifdef AI_LEARNING_INPUT
	ANN::SUnit* unit;
#else
	std::vector<double> dataInput;
	std::vector<double> dataOutput;
#endif

	int distance;
	int score;
	bool die;
	bool willKill;
	bool fallInGround;
	short int angle, time, x0;
	short int ahead = 0;
	int jumpTime;
	string saved_path = "";
	position posDoge;
};