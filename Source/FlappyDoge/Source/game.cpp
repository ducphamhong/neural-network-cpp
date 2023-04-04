#include "game.h"
#include "lib.h"
#include <iostream>

void game::takeInput()
{
	while (SDL_PollEvent(&event) != 0)
	{
		if (event.type == SDL_QUIT)
		{
			userInput.Type = input::QUIT;
			quit = true;
		}
		else if (event.type == SDL_MOUSEBUTTONDOWN || (event.type == SDL_KEYDOWN &&
			(event.key.keysym.sym == SDLK_SPACE || event.key.keysym.sym == SDLK_UP) && event.key.repeat == 0))
		{
			userInput.Type = input::PLAY;
		}
		else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE && event.key.repeat == 0)
		{
			userInput.Type = input::PAUSE;
		}
	}
}

game::game()
{
	initGraphic();
	initImages();
	pipe.init();
	land.init();
	sound.init();
}

game::~game()
{
	for (int i = 0; i < 10; i++)
	{
		imageNumberSmall[i].free();
		imageNumberLarge[i].free();
	}

	shiba.Free();
	pipe.Free();
	land.Free();
	sound.Free();
	free();
	releaseGraphic();
}

void game::releaseGraphic()
{
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	SDL_DestroyRenderer(gRenderer);
	gRenderer = NULL;
	IMG_Quit();
	TTF_Quit();
	SDL_Quit();
}

bool game::initGraphic()
{
	bool success = true;

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		success = false;
	}
	else
	{
		if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
		{
			printf("Warning: Linear texture filtering not enabled!");
		}

		gWindow = SDL_CreateWindow("Flappy Doge", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

		if (gWindow == NULL)
		{
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
			if (gRenderer == NULL)
			{
				printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
				success = false;
			}
			else
			{
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

				int imgFlags = IMG_INIT_PNG;
				if (!(IMG_Init(imgFlags) & imgFlags))
				{
					printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
					success = false;
				}

				if (TTF_Init() == -1)
				{
					printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
					success = false;
				}
			}
		}
	}

	return success;
}

void game::initImages()
{
	char path[512];
	for (int i = 0; i < 10; i++)
	{
		sprintf(path, "FlappyDoge/number/small/%d.png", i);
		imageNumberSmall[i].Load(std::string(path), scaleNumberS);

		sprintf(path, "FlappyDoge/number/large/%d.png", i);
		imageNumberLarge[i].Load(std::string(path), scaleNumberS);
	}
}

void game::display()
{
	SDL_RenderPresent(gRenderer);
	SDL_RenderClear(gRenderer);
}

void game::renderScoreSmall()
{
	string s = to_string(score);
	signed char len = s.length();

	for (signed char i = len - 1; i >= 0; i--)
	{
		signed char number = s[i] - '0';
		imageNumberSmall[number].Render(260 - imageNumberSmall[number].getWidth() * (len - i - 1) * 0.75 - 5 * (len - i - 1), 268);
	}
}

void game::renderScoreLarge()
{
	string s = to_string(score);
	signed char len = s.length();

	for (signed char i = 0; i < len; i++)
	{
		signed char number = s[i] - '0';
		imageNumberLarge[number].Render((SCREEN_WIDTH - (imageNumberLarge[number].getWidth() * len + (len - 1) * 10)) / 2 + (i + 30) * i, 100);
	}
}

void game::renderBestScore()
{
	ifstream fileIn("FlappyDoge/data/bestScore.txt");
	fileIn >> bestScore;
	ofstream fileOut("FlappyDoge/data/bestScore.txt", ios::trunc);

	if (score > bestScore)
	{
		bestScore = score;
	}
	string s = to_string(bestScore);
	signed char len = s.length();

	for (signed char i = len - 1; i >= 0; i--)
	{
		signed char number = s[i] - '0';
		imageNumberSmall[number].Render(260 - imageNumberSmall[number].getWidth() * (len - i - 1) * 0.75 - 5 * (len - i - 1), 315);
	}

	fileOut << bestScore;
	fileIn.close();
	fileOut.close();
}

void game::renderMessage()
{
	imageMessage.Load("FlappyDoge/image/message.png", 1);
	imageMessage.Render((SCREEN_WIDTH - imageMessage.getWidth()) / 2, 180);
	imageMessage.free();
}

void game::renderBackground()
{
	imageBG.Load("FlappyDoge/image/background.png", 1);
	imageBG.Render(0, 0);
	imageBG.free();
}

void game::renderBackgroundNight()
{
	imageBGNight.Load("FlappyDoge/image/background-night.png", 1);
	imageBGNight.Render(0, 0);
	imageBGNight.free();
}

void game::renderLand()
{
	imageLand.Load("FlappyDoge/image/land.png", 1);
	imageLand.Render((SCREEN_WIDTH - imageLand.getWidth()) / 2, SCREEN_HEIGHT - imageLand.getHeight());
	imageLand.free();
}

void game::resume()
{
	imageResume.Load("FlappyDoge/image/resume.png", 1);
	imageResume.Render(SCREEN_WIDTH - 50, 20);
	imageResume.free();
}

void game::pause()
{
	imagePause.Load("FlappyDoge/image/pause.png", 1);
	imagePause.Render(SCREEN_WIDTH - 50, 20);
	imagePause.free();
}

void game::renderPauseTab()
{
	imagePauseTab.Load("FlappyDoge/image/pauseTab.png", 1);
	imagePauseTab.Render((SCREEN_WIDTH - imagePauseTab.getWidth()) / 2, 230);
	imagePauseTab.free();
}

void game::lightTheme()
{
	imageShibaLight.Load("FlappyDoge/image/shiba.png", 0.8);
	imageShibaLight.Render(105, 315);
	imageShibaLight.free();
}

void game::darkTheme()
{
	imageShibaDark.Load("FlappyDoge/image/shiba-dark.png", 0.8);
	imageShibaDark.Render(105, 315);
	imageShibaDark.free();
}

void game::nextButton()
{
	imageRight.Load("FlappyDoge/image/nextRight.png", 1);
	imageRight.Render(149, 322);
	imageRight.free();

	imageLeft.Load("FlappyDoge/image/nextLeft.png", 1);
	imageLeft.Render(88, 322);
	imageLeft.free();
}

bool game::changeTheme()
{
	int x, y;
	SDL_GetMouseState(&x, &y);
	if (((x > 149 && x < 149 + 13) || (x > 88 && x < 88 + 13)) && (y > 322 && y < 322 + 16))
	{
		return true;
	}
	return false;
}

void game::renderGameOver()
{
	imageGameOver.Load("FlappyDoge/image/gameOver.png", 1);
	imageGameOver.Render((SCREEN_WIDTH - imageGameOver.getWidth()) / 2, 150);
	imageGameOver.free();
}

void game::renderMedal()
{
	int i = 0;
	if (score > 20 && score <= 50)
	{
		imageMedal[0].Load("FlappyDoge/medal/silver.png", scaleNumberS);
		i = 0;
	}
	else if (score > 50)
	{
		imageMedal[1].Load("FlappyDoge/medal/gold.png", scaleNumberS);
		i = 1;
	}
	else
	{
		imageMedal[2].Load("FlappyDoge/medal/honor.png", scaleNumberS);
		i = 2;
	}

	imageMedal[i].Render(82, 275);
	imageMedal[i].free();
}

void game::replay()
{
	imageReplay.Load("FlappyDoge/image/replay.png", 1);
	imageReplay.Render((SCREEN_WIDTH - imageReplay.getWidth()) / 2, 380);
	imageReplay.free();
}

bool game::checkReplay()
{
	int x, y;
	SDL_GetMouseState(&x, &y);
	if (x > (SCREEN_WIDTH - 100) / 2 && x < (SCREEN_WIDTH + 100) / 2 && y > 380 && y < 380 + 60)
	{
		return true;
	}
	return false;
}

void game::Restart()
{
	die = false;
	score = 0;
	shiba.resetTime();
}