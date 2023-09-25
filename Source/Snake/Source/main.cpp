/*
 * Snake game program using the SDL library
 *
 * @author J. Alvarez
 */
#include <iostream>
#include <cstring>
#include <vector>
#include <time.h>
#include <stdlib.h>
#include "Snake.hpp"
#include "Food.hpp"
#include "Wall.hpp"
#include "Screen.hpp"
#include "Section.hpp"
#include "SDL.h"
#include <Windows.h>

 // declare AI_LEARNING_INPUT in Screen.hpp
#ifdef AI_LEARNING_INPUT
#include "GeneticAlgorithm.h"
#endif

using namespace SnakeGame;

bool pauseGame(Screen& screen, bool& pause) {
	int startTime = SDL_GetTicks();
	bool quit = false;
	while (!quit && pause) {
		int action = screen.processEvents();
		switch (action) {
		case Screen::Action::QUIT:
			quit = true;
			break;
		case Screen::Action::PAUSE:
			pause = false;
			break;
		}

	}
	return quit;
}

void gameOver(Snake& snake, Food& food, bool& starting) {
	snake.die();
	starting = true;
}

void createWalls(std::vector<Wall*>& walls) {
	const int N_HORIZONTAL = Screen::S_WIDTH / Wall::S_WALL_WIDTH;
	const int N_VERTICAL = Screen::S_HEIGHT / Wall::S_WALL_WIDTH;

	for (int i = 0; i < N_HORIZONTAL; i++) {
		Wall* upperWall = new Wall(i * Wall::S_WALL_WIDTH, 0);
		Wall* lowerWall = new Wall(i * Wall::S_WALL_WIDTH,
			Screen::S_HEIGHT - 1 * Wall::S_WALL_WIDTH);
		walls.push_back(upperWall);
		walls.push_back(lowerWall);
	}
	for (int i = 1; i < N_VERTICAL; i++) {
		Wall* leftmostWall = new Wall(0, i * Wall::S_WALL_WIDTH);
		Wall* rightmostWall = new Wall(Screen::S_WIDTH - Wall::S_WALL_WIDTH, i * Wall::S_WALL_WIDTH);
		walls.push_back(leftmostWall);
		walls.push_back(rightmostWall);
	}
}

void drawWalls(std::vector<Wall*>& walls, Screen& screen) {
	for (auto wall : walls)
		wall->draw(screen);
}

void freeWalls(std::vector<Wall*>& walls) {
	for (auto wall : walls)
		delete wall;
	walls.clear();
}

double booleanInput(bool b)
{
	return b ? 1.0 : -1.0;
}

/*
bool isDanger(int direction, Snake& snake, std::vector<Wall*>& walls)
{
	snake.save();

	if (!snake.simulateMove(direction))
	{
		snake.load();
		return true;
	}

	for (auto wall : walls)
		if (snake.collidesWith(*wall))
		{
			snake.load();
			return true;
		}

	for (int i = 1; i < snake.m_sections.size(); i++)
		if (snake.collidesWith(*snake.m_sections[i]))
		{
			snake.load();
			return true;
		}

	snake.load();
	return false;
}
*/

bool checkCollide(int x, int y, Snake& snake, std::vector<Wall*>& walls)
{
	for (auto wall : walls)
	{
		if (wall->collidesWith(x, y))
		{
			return true;
		}
	}

	for (int i = 1; i < snake.m_sections.size(); i++)
	{
		if (snake.m_sections[i]->collidesWith(x, y))
			return true;
	}

	return false;
}

void getSafeRange(Snake& snake, std::vector<Wall*>& walls, int& top, int& down, int& left, int& right)
{
	int x = snake.getSections()[0]->m_x;
	int y = snake.getSections()[0]->m_y;

	top = down = left = right = 0;

	// safe top
	{
		int testX = x;
		int testY = y - Section::S_SECTION_WIDTH;

		while (!checkCollide(testX, testY, snake, walls))
		{
			top++;
			testY -= Section::S_SECTION_WIDTH;
		}
	}

	// safe down
	{
		int testX = x;
		int testY = y + Section::S_SECTION_WIDTH;

		while (!checkCollide(testX, testY, snake, walls))
		{
			down++;
			testY += Section::S_SECTION_WIDTH;
		}
	}

	// safe left
	{
		int testX = x - Section::S_SECTION_WIDTH;
		int testY = y;

		while (!checkCollide(testX, testY, snake, walls))
		{
			left++;
			testX -= Section::S_SECTION_WIDTH;
		}
	}

	// safe right
	{
		int testX = x + Section::S_SECTION_WIDTH;
		int testY = y;

		while (!checkCollide(testX, testY, snake, walls))
		{
			right++;
			testX += Section::S_SECTION_WIDTH;
		}
	}
}

#ifndef AI_LEARNING_INPUT
void getAIInputOutput(Snake& snake, Food& food, std::vector<Wall*>& walls)
{
	std::vector<double>& input = snake.getInput();
	std::vector<double>& output = snake.getOutput();
	int outputDirection = snake.getDirection();

	input.clear();
	output.clear();

	int x = snake.getSections()[0]->m_x;
	int y = snake.getSections()[0]->m_y;
	int foodX = food.m_x;
	int foodY = food.m_y;

	int t, d, l, r;
	getSafeRange(snake, walls, t, d, l, r);
	float maxRange = 100.0f;

	// safe left
	input.push_back(l / maxRange);
	// safe right
	input.push_back(r / maxRange);
	// safe up
	input.push_back(t / maxRange);
	// safe down
	input.push_back(d / maxRange);
	// food is in left
	input.push_back(booleanInput(foodX < x));
	// food is in right
	input.push_back(booleanInput(foodX > x));
	// food is up
	input.push_back(booleanInput(foodY > y));
	// food is down
	input.push_back(booleanInput(foodY < y));
	// food is same row
	input.push_back(booleanInput(foodX == x));
	// food is same column
	input.push_back(booleanInput(foodY == y));

	// OUTPUT: 3
	output.push_back(booleanInput(outputDirection == Snake::Direction::LEFT));
	output.push_back(booleanInput(outputDirection == Snake::Direction::RIGHT));
	output.push_back(booleanInput(outputDirection == Snake::Direction::UP));
	output.push_back(booleanInput(outputDirection == Snake::Direction::DOWN));
}
#else
void getAIInputOutput(Snake& snake, Food& food, std::vector<Wall*>& walls, std::vector<double>& input)
{
	int inputDirection = snake.getLastDirection();
	int outputDirection = snake.getDirection();

	input.clear();

	int x = snake.getSections()[0]->m_x;
	int y = snake.getSections()[0]->m_y;
	int foodX = food.m_x;
	int foodY = food.m_y;

	int t, d, l, r;
	getSafeRange(snake, walls, t, d, l, r);
	float maxRange = 100.0f;

	// safe left
	input.push_back(l / maxRange);
	// safe right
	input.push_back(r / maxRange);
	// safe up
	input.push_back(t / maxRange);
	// safe down
	input.push_back(d / maxRange);
	// food is in left
	input.push_back(booleanInput(foodX < x));
	// food is in right
	input.push_back(booleanInput(foodX > x));
	// food is up
	input.push_back(booleanInput(foodY > y));
	// food is down
	input.push_back(booleanInput(foodY < y));
	// food is same row
	input.push_back(booleanInput(foodX == x));
	// food is same column
	input.push_back(booleanInput(foodY == y));
}
#endif

int CALLBACK WinMain(
	HINSTANCE   hInstance,
	HINSTANCE   hPrevInstance,
	LPSTR       lpCmdLine,
	int         nCmdShow)
{
	srand(time(NULL));

	Screen screen;

	Snake snake[MAX_AI_UNIT];
	Food food[MAX_AI_UNIT];
	int deadTime[MAX_AI_UNIT];

	int autoSaveInputTime = 3000;
	int autoSaveTime = autoSaveInputTime;

	std::vector<Wall*> walls[MAX_AI_UNIT];
	int score[MAX_AI_UNIT];
	int maxTime = 1000 * 30;
	for (int i = 0; i < MAX_AI_UNIT; i++)
	{
		createWalls(walls[i]);
		score[i] = 0;
		deadTime[i] = maxTime;
	}

	if (!screen.init()) {
		SDL_Log("Error initializing screen");
		return -1;
	}

	bool quit = false;
	bool starting = true;
	bool pause = false;
	int lastElapsed = 0;

	const char* dataInput = "Snake/learning.txt";

	std::vector<double> allInput;
	std::vector<double> allOutput;

	int gen = 0;

#ifndef AI_LEARNING_INPUT
	int delay = 300;
#else
	int dieTime = 60 * 5;
	int killAll = 60 * 10;
	bool autoSkipOldGeneration = true;
	int currentGenerationID[MAX_AI_UNIT];
	for (int i = 0; i < MAX_AI_UNIT; i++)
	{
		currentGenerationID[i] = -1;
	}

	ANN::CGeneticAlgorithm aiGenetic(ANN::EActivation::Tanh);
	const int dim[] = { 10, 256, 4 };
	aiGenetic.createPopulation(MAX_AI_UNIT, dim, 3);

	int delay = 100;
	{
		std::vector<double> input;
		std::vector<double> output;

		// See the function doge::reportDie
		// Read human played data
		FILE* f = fopen(dataInput, "rt");
		char lines[512];

		int numInput = 0;
		int numOutput = 0;
		int numRecord = 0;

		fgets(lines, 512, f);
		sscanf(lines, "%d %d %d", &numInput, &numOutput, &numRecord);

		// skip bad last 2 move
		int numDataLearning = numRecord - 2;

		for (int j = 0; j < numDataLearning; j++)
		{
			for (int i = 0; i < numInput; i++)
			{
				double d;
				fgets(lines, 512, f);
				sscanf(lines, "%lf", &d);
				input.push_back(d);
			}

			for (int i = 0; i < numOutput; i++)
			{
				double d;
				fgets(lines, 512, f);
				sscanf(lines, "%lf", &d);
				output.push_back(d);
			}
		}

		fclose(f);

		// learn expected function
		// need learning from human control for Gen 0
		std::vector<ANN::SUnit*>& units = aiGenetic.get();
		for (int i = 0, n = (int)units.size(); i < n; i++)
		{
			units[i]->ANN->LearnExpected = [](double* trainData, int trainId, double* expectedOutput, int numOutput)
			{
				int id = trainId * numOutput;
				expectedOutput[0] = trainData[id];
				expectedOutput[1] = trainData[id + 1];
				expectedOutput[2] = trainData[id + 2];
				expectedOutput[3] = trainData[id + 3];
			};

			for (int learnCount = 500; learnCount > 0; learnCount--)
			{
				units[i]->ANN->train(input.data(), output.data(), numDataLearning);
			}

			snake[i].setAIUnit(units[i]);
			snake[i].live();
		}
	}
#endif

	int lastFrameElapsed = 0;
	int frameTime = 0;

	while (!quit) {
		int action = screen.processEvents();

		if (pause)
			quit = pauseGame(screen, pause);

		int elapsed = SDL_GetTicks();
		if (lastElapsed == 0)
			lastElapsed = elapsed;
		if (lastFrameElapsed == 0)
			lastFrameElapsed = elapsed;

		frameTime = elapsed - lastFrameElapsed;

#ifdef AI_LEARNING_INPUT
		// check evol
		int numSnakeDie = 0;
		for (int agentId = 0; agentId < MAX_AI_UNIT; agentId++)
		{
			if (snake[agentId].isDie())
			{
				numSnakeDie++;
			}
		}

		if (numSnakeDie == MAX_AI_UNIT)
		{
			dieTime = dieTime - frameTime;
			if (dieTime < 0)
			{
				dieTime = 60 * 5;

				aiGenetic.evolvePopulation();

				std::vector<ANN::SUnit*>& units = aiGenetic.get();
				for (int i = 0, n = (int)units.size(); i < n; i++)
				{
					snake[i].setAIUnit(units[i]);
					snake[i].reset();
					food[i] = Food();
					score[i] = 0;
					snake[i].live();
					deadTime[i] = maxTime;
				}

				for (int i = 0; i < MAX_AI_UNIT; i++)
				{
					currentGenerationID[i] = aiGenetic.get()[i]->ID;
				}

				gen++;
			}
		}
#endif

		for (int agentId = 0; agentId < MAX_AI_UNIT; agentId++)
		{
			screen.clear();

			drawWalls(walls[agentId], screen);
			snake[agentId].draw(screen);
			food[agentId].draw(screen);

			switch (action) {
			case Screen::Action::QUIT:
				quit = true;
				break;
			case Screen::Action::PAUSE:
				pause = true;
				break;
			case -1:
				break;
			default:
#ifndef AI_LEARNING_INPUT
				if (snake[agentId].isDie())
				{
					snake[agentId].reset();
					food[agentId] = Food();
					score[agentId] = 0;
					snake[agentId].live();
					deadTime[agentId] = maxTime;
				}
#endif
				break;
			};

			bool topUnit = false;

#ifdef AI_LEARNING_INPUT
			topUnit = snake[agentId].getAIUnit()->TopUnit;
#endif

			if (!snake[agentId].isDie())
			{
				screen.update(score[agentId], gen, false, agentId, topUnit);

#ifdef AI_LEARNING_INPUT
				std::vector<double> input;
				getAIInputOutput(snake[agentId], food[agentId], walls[agentId], input);

				ANN::SUnit* aiUnit = snake[agentId].getAIUnit();
				aiUnit->ANN->Predict = [](const double* output, int numOutput)
				{
					double max = -1.0;
					int result = 0;

					for (int i = 0; i < numOutput; i++)
					{
						if (max < output[i])
						{
							max = output[i];
							result = i;
						}
					}
					return (double)result;
				};

				// let AI control
				int output = (int)aiUnit->ANN->predict(input.data());
				switch (output)
				{
				case 0:
					if (!snake[agentId].m_hasUpdated)
						snake[agentId].updateDirection(Snake::Direction::LEFT);
					break;
				case 1:
					if (!snake[agentId].m_hasUpdated)
						snake[agentId].updateDirection(Snake::Direction::RIGHT);
					break;
				case 2:
					if (!snake[agentId].m_hasUpdated)
						snake[agentId].updateDirection(Snake::Direction::UP);
					break;
				case 3:
					if (!snake[agentId].m_hasUpdated)
						snake[agentId].updateDirection(Snake::Direction::DOWN);
					break;
				default:
					break;
				}
#else
				switch (action) {
				case Screen::Action::MOVE_UP:
					if (!snake[agentId].m_hasUpdated)
						snake[agentId].updateDirection(Snake::Direction::UP);
					break;
				case Screen::Action::MOVE_DOWN:
					if (!snake[agentId].m_hasUpdated)
						snake[agentId].updateDirection(Snake::Direction::DOWN);
					break;
				case Screen::Action::MOVE_LEFT:
					if (!snake[agentId].m_hasUpdated)
						snake[agentId].updateDirection(Snake::Direction::LEFT);
					break;
				case Screen::Action::MOVE_RIGHT:
					if (!snake[agentId].m_hasUpdated)
						snake[agentId].updateDirection(Snake::Direction::RIGHT);
					break;
				default:
					break;
				}
#endif
				deadTime[agentId] = deadTime[agentId] - frameTime;

#ifndef AI_LEARNING_INPUT
				bool needSaveInput = false;
				autoSaveTime = autoSaveTime - frameTime;
				if (autoSaveTime < 0)
				{
					autoSaveTime = autoSaveInputTime;
					needSaveInput = true;
				}
#endif

				if (elapsed - lastElapsed > delay) {
#ifndef AI_LEARNING_INPUT
					getAIInputOutput(snake[agentId], food[agentId], walls[agentId]);
#endif
					bool changeInput = snake[agentId].getDirection() != snake[agentId].getLastDirection();

					if (!snake[agentId].move())
						gameOver(snake[agentId], food[agentId], starting);
					else {
						if (snake[agentId].collidesWith(food[agentId])) {
							food[agentId] = Food();
							score[agentId] += Food::S_VALUE;
							snake[agentId].addSection();

							// set limit time to eat food
							deadTime[agentId] = maxTime;
						}

						for (auto wall : walls[agentId])
							if (snake[agentId].collidesWith(*wall))
								gameOver(snake[agentId], food[agentId], starting);

						for (int i = 1; i < snake[agentId].m_sections.size(); i++)
							if (snake[agentId].collidesWith(*snake[agentId].m_sections[i]))
								gameOver(snake[agentId], food[agentId], starting);
					}

					if (deadTime[agentId] < 0)
					{
						// stuck
						gameOver(snake[agentId], food[agentId], starting);
					}

#ifdef AI_LEARNING_INPUT
					// report score
					if (snake[agentId].isDie())
					{
						ANN::SUnit* unit = snake[agentId].getAIUnit();

						unit->Scored = score[agentId];

						if (score[agentId] > unit->BestScored)
							unit->BestScored = score[agentId];

						if (unit->Scored <= 0)
							unit->Good = false;
						else
							unit->Good = true;
					}
#else

					std::vector<double>& input = snake[agentId].getInput();
					std::vector<double>& output = snake[agentId].getOutput();
					int numInput = (int)input.size();
					int numOutput = (int)output.size();

					// save for learning
					if (!snake[agentId].isDie())
					{
						if (changeInput || needSaveInput)
						{
							for (int i = 0; i < numInput; i++)
								allInput.push_back(input[i]);
							for (int i = 0; i < numOutput; i++)
								allOutput.push_back(output[i]);
						}
					}
					else
					{
						int numAllInput = (int)allInput.size();
						int numRecord = (int)allInput.size() / numInput;

						FILE* f = fopen(dataInput, "wt");
						fprintf(f, "%d %d %d\n", numInput, numOutput, numRecord);

						int i1 = 0;
						int i2 = 0;

						for (int j = 0; j < numRecord; j++)
						{
							for (int i = 0; i < numInput; i++)
							{
								fprintf(f, "%lf\n", allInput[i1 + i]);
							}

							for (int i = 0; i < numOutput; i++)
							{
								fprintf(f, "%lf\n", allOutput[i2 + i]);
							}

							i1 += numInput;
							i2 += numOutput;
						}

						fclose(f);
					}
#endif
				}
			}
			else
			{
				// is die
				screen.update(score[agentId], gen, true, agentId, topUnit);
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
			if (!snake[i].isDie())
			{
				liveID[liveCount++] = snake[i].getAIUnit()->ID;
			}
		}

		if (autoSkipOldGeneration && liveCount > 0)
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
						if (!snake[i].isDie())
						{
							snake[i].die();
						}
					}
					killAll = 60 * 10;
					// kill all after 600 frames (10s)
					// we no need spent time to wait old generation
				}
			}
		}
#endif

		if (elapsed - lastElapsed > delay)
		{
			lastElapsed = elapsed;
		}

		lastFrameElapsed = elapsed;

		screen.present();
	}

	for (int i = 0; i < MAX_AI_UNIT; i++)
	{
		freeWalls(walls[i]);
	}

	screen.close();

	return 0;
}
