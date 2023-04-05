#include <Windows.h>
#include "game.h"
#include "lib.h"
#include <time.h>
#include <stdlib.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#ifdef AI_LEARNING_INPUT
#include "GeneticAlgorithm.h"
#endif


const short int FPS = 60;
const short int frameDelay = 1000 / FPS;

using namespace std;

int CALLBACK WinMain(
	HINSTANCE   hInstance,
	HINSTANCE   hPrevInstance,
	LPSTR       lpCmdLine,
	int         nCmdShow
)
{
	Uint32 frameStart;
	short int frameTime;
	game g;
	bool isMenu = 0;
	bool isPause = 0;
	bool isSound = 1;
	bool isDark = 0;

	context::gGame = &g;

#ifdef AI_LEARNING_INPUT
	ANN::CGeneticAlgorithm aiGenetic;
	const int dim[] = { 2, 6, 1 };
	aiGenetic.createPopulation(MAX_AI_UNIT, dim, 3);

	int gen = 0;
#endif

	while (!g.isQuit())
	{
		frameStart = SDL_GetTicks();

		if (g.isDie())
		{
			if (isMenu) {
				g.sound.playHit();
				g.shiba[0].render();
			}
			g.userInput.Type = game::input::NONE;
			while (g.isDie() && !g.isQuit())
			{
				g.takeInput();
				if (isMenu == 1 && g.userInput.Type == game::input::PLAY)
				{
					if (g.checkReplay())
					{
						isMenu = 0;
					}
					g.userInput.Type = game::input::NONE;
				}

				if (!isDark)
					g.renderBackground();
				else
					g.renderBackgroundNight();

				g.pipe.render();
				g.land.render();

				if (isMenu)
				{
					g.shiba[0].render();
					g.shiba[0].fall();

					g.renderGameOver();
					g.renderMedal();
					g.renderScoreSmall();
					g.renderBestScore();
					g.replay();
				}
				else
				{
					g.pipe.init();

#ifdef AI_LEARNING_INPUT					
					for (int i = 0; i < MAX_AI_UNIT; i++)
					{
						g.shiba[i].init(isDark);
						g.shiba[i].render();
					}
#else
					g.shiba[0].init(isDark);
					g.shiba[0].render();
#endif
					g.renderMessage();

					if (g.userInput.Type == game::input::PLAY)
					{
						context::score = 0;

#ifdef AI_LEARNING_INPUT
						if (gen != 0)
							aiGenetic.evolvePopulation();
						gen++;

						for (int i = 0; i < MAX_AI_UNIT; i++)
						{
							g.shiba[i].restart();
							g.shiba[i].setAIUnit(aiGenetic.get()[i]);
							g.shiba[i].resetTime();
						}
#else
						g.shiba[0].restart();
						g.shiba[0].resetTime();
#endif

						isMenu = 1;
						g.userInput.Type = game::input::NONE;
					}
					g.land.update();
				}
				g.display();
			}
			g.pipe.init();
		}
		else
		{
			g.takeInput();

			if (g.userInput.Type == game::input::PAUSE)
			{
				isPause = abs(1 - isPause);
				g.userInput.Type = game::input::NONE;
			}

#ifdef AI_LEARNING_INPUT
			// let ai controller on shiba.update
#else
			if (isPause == 0 && g.userInput.Type == game::input::PLAY)
			{
				if (isSound)
					g.sound.playBreath();
				g.shiba[0].resetTime();
				g.userInput.Type = game::input::NONE;
			}
#endif

			if (!isDark)
				g.renderBackground();
			else
				g.renderBackgroundNight();

			g.pipe.render();
			g.land.render();

			g.renderScoreLarge();

			if (!isPause)
			{
#ifdef AI_LEARNING_INPUT
				for (int i = 0; i < MAX_AI_UNIT; i++)
					g.shiba[i].update(g.getPipeWidth(), g.getPipeHeight());
#else
				g.shiba[0].update(g.getPipeWidth(), g.getPipeHeight());
#endif
				g.pipe.update();
				g.land.update();
				g.pause();
			}
			else
			{
				g.resume();
				g.renderPauseTab();
				g.renderScoreSmall();
				g.renderBestScore();
				g.replay();
				g.sound.renderSound();

				if (!isDark)
					g.lightTheme();
				else
					g.darkTheme();

				g.nextButton();

				if (g.userInput.Type == game::input::PLAY)
				{
					if (g.checkReplay())
					{
						isPause = 0;
					}
					else if (g.sound.checkSound())
					{
						isSound = abs(1 - isSound);
					}
					else if (g.changeTheme())
					{
						isDark = abs(1 - isDark);

#ifdef AI_LEARNING_INPUT
						for (int i = 0; i < MAX_AI_UNIT; i++)
						{
							g.shiba[i].init(isDark);
							g.shiba[i].setAIUnit(aiGenetic.get()[i]);
						}
#else
						g.shiba[0].init(isDark);
#endif
					}
					g.userInput.Type = game::input::NONE;
				}
			}

#ifdef AI_LEARNING_INPUT
			for (int i = 0; i < MAX_AI_UNIT; i++)
			{
				g.shiba[i].render();
			}
#else
			g.shiba[0].render();
#endif

			g.display();
		}

		// Limit FPS
		frameTime = SDL_GetTicks() - frameStart;
		if (frameDelay > frameTime)
		{
			SDL_Delay(frameDelay - frameTime);
		}
	}
	return 0;
}
