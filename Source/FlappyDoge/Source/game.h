#pragma once

#include "doge.h"
#include "pipe.h"
#include "land.h"
#include "sound.h"
#include <string>
#include <fstream>
#include <sstream>

using namespace std;

class game :LTexture
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
	short int score;

	doge shiba;
	pipe pipe;
	sound sound;
	land land;

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
	game();

	~game();

	bool initGraphic();

	void initImages();

	void freeImages();

	bool isQuit()
	{
		return quit;
	}

	bool isDie()
	{
		return shiba.isDie();
	}

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

	void Restart();

private:
	const double scaleNumberS = 0.75;
	short int bestScore;
};