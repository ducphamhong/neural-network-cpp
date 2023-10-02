#include <Windows.h>
#include "game.h"
#include "lib.h"
#include <time.h>
#include <stdlib.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <filesystem>

#ifdef AI_LEARNING_INPUT
#include "GeneticAlgorithm.h"
#include "MemoryStream.h"
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
	Game g;
	bool isMenu = 0;
	bool isPause = 0;
	bool isSound = 1;
	bool isDark = 0;

	bool autoSkipGameOverDialog = false;
	int killAll = 60 * 10;
	bool firstTraining = true;

	context::gGame = &g;

#ifdef AI_LEARNING_INPUT
	ANN::CGeneticAlgorithm aiGenetic(ANN::EActivation::Tanh);
	const int dim[] = { 4, 6, 6, 1 };
	aiGenetic.createPopulation(MAX_AI_UNIT, dim, 4);

	// read last train session
	const char* lastTrainData = "FlappyDoge/data/last-training.data";
	const char* humanPlayedData = "FlappyDoge/data/learning.txt";

	firstTraining = !std::filesystem::exists(lastTrainData);

	if (firstTraining)
	{
		std::vector<double> input;
		std::vector<double> output;

		// See the function doge::reportDie
		// Read human played data
		FILE* f = fopen(humanPlayedData, "rt");
		char lines[512];
		while (!feof(f))
		{
			double i1, i2, i3, i4, o;
			fgets(lines, 512, f);
			sscanf(lines, "%lf %lf %lf %lf %lf", &i1, &i2, &i3, &i4, &o);
			input.push_back(i1);
			input.push_back(i2);
			input.push_back(i3);
			input.push_back(i4);
			output.push_back(o);
		}
		fclose(f);

		// learn expected function
		// need learning from human control for Gen 0
		std::vector<ANN::SUnit*>& units = aiGenetic.get();
		for (int i = 0, n = (int)units.size(); i < n; i++)
		{
			units[i]->ANN->LearnExpected = [](double* trainData, int trainId, double* expectedOutput, int numOutput)
			{
				expectedOutput[0] = trainData[trainId];
			};

			for (int learnCount = 500; learnCount > 0; learnCount--)
			{
				units[i]->ANN->train(input.data(), output.data(), (int)output.size());
			}
		}
	}
	else
	{
		// continue training after close program
		FILE* f = fopen(lastTrainData, "rb");
		fseek(f, 0, SEEK_END);
		long size = ftell(f);
		unsigned char* data = new unsigned char[size];
		fseek(f, 0, SEEK_SET);
		fread(data, size, 1, f);
		fclose(f);

		ANN::CMemoryStream stream(data, size);
		aiGenetic.deserialize(&stream);
		delete[]data;
	}

	int gen = 0;
	autoSkipGameOverDialog = false;

	int currentGenerationID[MAX_AI_UNIT];
	for (int i = 0; i < MAX_AI_UNIT; i++)
	{
		currentGenerationID[i] = -1;
	}
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
			g.userInput.Type = Game::input::NONE;
			while (g.isDie() && !g.isQuit())
			{
				g.takeInput();

				bool autoRestart = isMenu && autoSkipGameOverDialog && g.shiba[0].isFallInGround();

				if ((isMenu == 1 && g.userInput.Type == Game::input::PLAY) || autoRestart)
				{
					if (g.checkReplay() || autoRestart)
					{
						isMenu = 0;
					}
					g.userInput.Type = Game::input::NONE;
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

					if (g.userInput.Type == Game::input::PLAY || autoRestart)
					{
						context::score = 0;

#ifdef AI_LEARNING_INPUT
						autoSkipGameOverDialog = true;

						if (gen != 0)
						{
							aiGenetic.evolvePopulation();

							// save process to file
							ANN::CMemoryStream data;
							aiGenetic.serialize(&data);
							FILE* f = fopen(lastTrainData, "wb");
							fwrite(data.getData(), data.getSize(), 1, f);
							fclose(f);

							// continue train
							for (int i = 0; i < MAX_AI_UNIT; i++)
							{
								currentGenerationID[i] = aiGenetic.get()[i]->ID;
							}
						}
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
						g.userInput.Type = Game::input::NONE;
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

			if (g.userInput.Type == Game::input::PAUSE)
			{
				isPause = abs(1 - isPause);
				g.userInput.Type = Game::input::NONE;
			}

#ifdef AI_LEARNING_INPUT
			// let ai controller on shiba.update
			if (g.userInput.Type == Game::input::PLAY)
			{
				// if player click or press spacebar
				// we will show GameOver dialog after learning
				autoSkipGameOverDialog = false;
			}
#else
			if (isPause == 0 && g.userInput.Type == Game::input::PLAY)
			{
				if (isSound)
					g.sound.playBreath();
				g.shiba[0].resetTime();	// flap
				g.userInput.Type = Game::input::NONE;
			}
#endif

			if (!isDark)
				g.renderBackground();
			else
				g.renderBackgroundNight();

			g.pipe.render();
			g.land.render();

			g.renderScoreLarge();

#ifdef AI_LEARNING_INPUT
			g.renderBestUnitID(gen);
#endif

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

				if (g.userInput.Type == Game::input::PLAY)
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
					g.userInput.Type = Game::input::NONE;
				}
			}

#ifdef AI_LEARNING_INPUT
			int liveCount = 0;
			int topUnit = 4;
			int liveID[MAX_AI_UNIT];

			for (int i = 0; i < MAX_AI_UNIT; i++)
			{
				liveID[i] = -1;
			}

			for (int i = 0; i < MAX_AI_UNIT; i++)
			{
				if (!g.shiba[i].isDie())
				{
					g.shiba[i].render();
					liveID[liveCount++] = g.shiba[i].getAIUnit()->ID;
				}
			}

			if (autoSkipGameOverDialog && liveCount > 0)
			{
				bool foundNewGene = true;

				// check to skip alive shiba is in old gene in top Unit
				if (liveCount > topUnit)
					foundNewGene = true;
				else
				{
					int numOldGeneration = 0;

					for (int i = 0; i < liveCount; i++)
					{
						for (int j = 0; j < topUnit; j++)
						{
							if (liveID[i] == currentGenerationID[j])
							{
								numOldGeneration++;
								break;
							}
						}
					}

					if (liveCount == numOldGeneration)
					{
						foundNewGene = false;
					}
				}

				if (!foundNewGene)
				{
					if (--killAll < 0)
					{
						for (int i = 0; i < MAX_AI_UNIT; i++)
						{
							g.shiba[i].setFallInGround();

							if (!g.shiba[i].isDie())
							{
								g.shiba[i].kill();
							}
						}
						killAll = 60 * 10;
						// kill all after 600 frames (10s)
						// we no need spent time to wait old generation
					}
				}
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
			short d = frameDelay - frameTime;
			if (d > 0 && d < 1000)
				SDL_Delay(frameDelay - frameTime);
		}
	}
	return 0;
}
