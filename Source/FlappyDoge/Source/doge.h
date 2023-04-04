#pragma once

#include "lib.h"
#include "pipe.h"

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
	}

	void resetTime()
	{
		time = 0;
	}

	void fall();

	void update(short int pileWidth, short int pileHeight);

private:
	bool die;
	short int angle, time, x0;
	short int ahead = 0;
	string saved_path = "";
	position posDoge;
};