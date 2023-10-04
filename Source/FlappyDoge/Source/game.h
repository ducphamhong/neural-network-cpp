#pragma once

#include "doge.h"
#include "pipe.h"
#include "land.h"
#include "sound.h"
#include <string>
#include <fstream>
#include <sstream>

using namespace std;

class Game :LTexture
{
public:
	struct input
	{
		enum type { QUIT, PLAY, NONE, PAUSE };
		type Type;
	};

	input userInput;

	SDL_Event event;
	bool quit;

	doge shiba[MAX_AI_UNIT];

	Pipe pipe;
	Sound sound;
	Land land;

	LTexture imageNumberSmall[10];
	LTexture imageNumberLarge[10];
	LTexture imageMessage;
	LTexture imageBG;
	LTexture imageBGNight;
	LTexture imageLand;
	LTexture imageResume;
	LTexture imagePause;
	LTexture imagePauseTab;
	LTexture imageShibaLight;
	LTexture imageShibaDark;
	LTexture imageRight;
	LTexture imageLeft;
	LTexture imageGameOver;
	LTexture imageMedal[3];
	LTexture imageReplay;

public:
	Game();

	~Game();

	bool initGraphic();

	void log(const char* string);

	void initImages();

	void freeImages();

	bool isQuit()
	{
		return quit;
	}

	bool isDie();

	int getPipeWidth()
	{
		return pipe.width();
	}

	int getPipeHeight()
	{
		return pipe.height();
	}

	void takeInput();

	void display();

	void releaseGraphic();

	void renderScoreSmall();

	void renderScoreLarge();

#ifdef AI_LEARNING_INPUT
	void renderBestUnitID(int gen);
#endif

	void renderBestScore();

	void renderMessage();

	void renderBackground();

	void renderBackgroundNight();

	void renderLand();

	void resume();

	void pause();

	void renderPauseTab();

	void lightTheme();

	void darkTheme();

	void nextButton();

	bool changeTheme();

	void renderGameOver();

	void renderMedal();

	void replay();

	bool checkReplay();

private:
	const double scaleNumberS = 0.75;
	short int bestScore;
};