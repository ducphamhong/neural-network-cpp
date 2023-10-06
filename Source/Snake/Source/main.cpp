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
#ifdef _MSC_VER
#include <Windows.h>
#endif
#include <filesystem>

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

float getInput(float value, float maxRange = 20.0f)
{
	if (value > maxRange)
		value = maxRange;
	return value / maxRange;
}

double getBoolInput(bool b)
{
	return b ? 1.0 : -1.0;
}

int intAbs(int x)
{
	return x < 0 ? -x : x;
}

const char* dataInput = "Data/Snake/learning.txt";
const char* lastTrainData = "Data/Snake/last-training.data";

void getAIInputOutput(Snake& snake, Food& food, std::vector<Wall*>& walls, std::vector<double>& input, std::vector<double>* output)
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

	bool dangerLeft = isDanger(Snake::Direction::LEFT, snake, walls);
	bool dangerRight = isDanger(Snake::Direction::RIGHT, snake, walls);
	bool dangerUp = isDanger(Snake::Direction::UP, snake, walls);
	bool dangerDown = isDanger(Snake::Direction::DOWN, snake, walls);

	// 1. danger left	
	input.push_back(getBoolInput(dangerLeft));
	// 2. danger right
	input.push_back(getBoolInput(dangerRight));
	// 3. danger up
	input.push_back(getBoolInput(dangerUp));
	// 4. danger down
	input.push_back(getBoolInput(dangerDown));

	// 5. food is in left
	input.push_back(getBoolInput(foodX < x));
	// 6. food is in right
	input.push_back(getBoolInput(foodX > x));
	// 7. food is up
	input.push_back(getBoolInput(foodY > y));
	// 8. food is down
	input.push_back(getBoolInput(foodY < y));
	// 8. food is same row
	input.push_back(getBoolInput(foodX == x));
	// 10. food is same column
	input.push_back(getBoolInput(foodY == y));

	bool canEatFoodY = true;
	bool canEatFoodX = true;

	// 11. this way can eat food?
	{
		// distance to food
		int f = (int)((foodY - y) / (int)Section::S_SECTION_WIDTH);
		f = intAbs(f);
		if (foodY < y)
		{
			canEatFoodY = f < t;
			input.push_back(getBoolInput(canEatFoodY));
		}
		else
		{
			canEatFoodY = f < d;
			input.push_back(getBoolInput(canEatFoodY));
		}
	}

	// 12. this way can eat food?
	{
		// distance to food
		int f = (int)((foodX - x) / (int)Section::S_SECTION_WIDTH);
		f = intAbs(f);
		if (foodX > x)
		{
			canEatFoodX = f < r;
			input.push_back(getBoolInput(canEatFoodX));
		}
		else
		{
			canEatFoodX = f < l;
			input.push_back(getBoolInput(canEatFoodX));
		}
	}

	// 13. safe left/right
	if (canEatFoodX)
		input.push_back(0.0);
	else
		input.push_back(getBoolInput(l > r));

	// 14. safe up/down
	if (canEatFoodY)
		input.push_back(0.0);
	else
		input.push_back(getBoolInput(t > d));

	// OUTPUT
	if (output)
	{
		output->clear();
		output->push_back(getBoolInput(outputDirection == Snake::Direction::LEFT));
		output->push_back(getBoolInput(outputDirection == Snake::Direction::RIGHT));
		output->push_back(getBoolInput(outputDirection == Snake::Direction::UP));
		output->push_back(getBoolInput(outputDirection == Snake::Direction::DOWN));
	}
}

#ifdef AI_LEARNING_INPUT
std::vector<double> humanInput;
std::vector<double> humanOutput;
int numDataLearning = 0;

void readDataInputOutput()
{
	humanInput.clear();
	humanOutput.clear();

	// Read human played data
	FILE* f = fopen(dataInput, "rt");
	char lines[512];

	int numInput = 0;
	int numOutput = 0;
	int numRecord = 0;

	fgets(lines, 512, f);
	sscanf(lines, "%d %d %d", &numInput, &numOutput, &numRecord);

	// skip bad last 2 move
	numDataLearning = numRecord - 2;

	for (int j = 0; j < numDataLearning; j++)
	{
		for (int i = 0; i < numInput; i++)
		{
			double d;
			fgets(lines, 512, f);
			sscanf(lines, "%lf", &d);
			humanInput.push_back(d);
		}

		for (int i = 0; i < numOutput; i++)
		{
			double d;
			fgets(lines, 512, f);
			sscanf(lines, "%lf", &d);
			humanOutput.push_back(d);
		}
	}

	fclose(f);
}

void learnExpected(double* trainData, int trainId, double* expectedOutput, int numOutput)
{
	int id = trainId * numOutput;
	expectedOutput[0] = trainData[id];
	expectedOutput[1] = trainData[id + 1];
	expectedOutput[2] = trainData[id + 2];
	expectedOutput[3] = trainData[id + 3];
}

void train(ANN::CGeneticAlgorithm& aiGenetic, int learnCount)
{
	std::vector<ANN::SUnit*>& units = aiGenetic.get();
	int numTop = aiGenetic.getNumTopUnit();

	for (int i = 0, n = (int)units.size(); i < n; i++)
	{
		units[i]->ANN->LearnExpected = learnExpected;
		units[i]->Scored = 0;
	}

	for (int i = 0; i < numTop; i++)
	{
		units[i]->Scored = 100;
		for (; learnCount > 0; learnCount--)
		{
			units[i]->ANN->train(humanInput.data(), humanOutput.data(), numDataLearning);
		}
	}
}
#endif

#ifdef _MSC_VER
int CALLBACK WinMain(
	HINSTANCE   hInstance,
	HINSTANCE   hPrevInstance,
	LPSTR       lpCmdLine,
	int         nCmdShow)
#else
int main()
#endif
{
	srand(time(NULL));

	Screen screen;

	Snake snake[MAX_AI_UNIT];
	Food food[MAX_AI_UNIT];
	int foodId[MAX_AI_UNIT];
	int timeEatFood[MAX_AI_UNIT];

	std::vector<Food> allFoods;
	allFoods.push_back(Food());

	int waitStuckTime[MAX_AI_UNIT];

	int autoSaveInputTime = 2000;
	int autoSaveTime = autoSaveInputTime;

	std::vector<Wall*> walls[MAX_AI_UNIT];
	int score[MAX_AI_UNIT];
	int maxTime = 1000 * 20;
	for (int i = 0; i < MAX_AI_UNIT; i++)
	{
		createWalls(walls[i]);
		score[i] = 0;
		foodId[i] = 0;
		timeEatFood[i] = 0;
		waitStuckTime[i] = maxTime;
	}

	if (!screen.init()) {
		SDL_Log("Error initializing screen");
		return -1;
	}

	bool quit = false;
	bool starting = true;
	bool pause = false;
	int lastElapsed = 0;

	std::vector<double> allInput;
	std::vector<double> allOutput;

	int gen = 1;

#ifndef AI_LEARNING_INPUT
	int moveSpeed = 300;
#else
	int waitResetTime = 1000;
	int killAll = 60 * 10;
	bool autoSkipOldGeneration = false;
	int currentGenerationID[MAX_AI_UNIT];
	for (int i = 0; i < MAX_AI_UNIT; i++)
	{
		currentGenerationID[i] = -1;
	}

	ANN::CGeneticAlgorithm aiGenetic(ANN::EActivation::Tanh);
	const int dim[] = { 14, 128, 128, 4 };
	aiGenetic.createPopulation(MAX_AI_UNIT, dim, 4);
	aiGenetic.setMutate(0.25, 2.0);

	int moveSpeed = 60;

	bool trainState = true;

	int trainProgress = 0;
	int trainStep = 10;
	int fullTrainProgress = 1000 / trainStep;

	if (std::filesystem::exists(lastTrainData))
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

		// force finish train
		trainProgress = fullTrainProgress;
	}
	else
	{
		readDataInputOutput();
	}


	int maxTest = 1;
	int testTime = maxTest;
#endif

	int lastFrameElapsed = 0;
	int frameTime = 0;
	int numberTimeTestBadUnit = 2;

	int trainTime = 3000 / 100;

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
		if (frameTime > 100)
			frameTime = 100;

#ifdef AI_LEARNING_INPUT
		if (trainState)
		{
			if (++trainProgress < fullTrainProgress)
			{
				float percent = trainProgress / (float)fullTrainProgress;
				char title[512];
				sprintf(title, "Train procress: %d %%", (int)(percent * 100));
				screen.log(title);
				train(aiGenetic, trainStep);
			}
			else
			{
				// begin at Gen 1
				aiGenetic.evolvePopulation();
				gen = 1;

				allFoods.clear();
				allFoods.push_back(Food());

				std::vector<ANN::SUnit*>& units = aiGenetic.get();
				for (int i = 0, n = (int)units.size(); i < n; i++)
				{
					units[i]->Scored = 0;
					snake[i].setAIUnit(units[i]);
					snake[i].reset();
					foodId[i] = 0;
					timeEatFood[i] = 0;
					food[i] = allFoods[0];
					score[i] = 0;
					snake[i].live();
					waitStuckTime[i] = maxTime;
					currentGenerationID[i] = aiGenetic.get()[i]->ID;
				}

				char title[512];
				testTime = maxTest;
				sprintf(title, "Generation: %d - test: %d/%d - skipOldGen: %d", gen, maxTest - testTime + 1, maxTest, autoSkipOldGeneration);
				screen.log(title);
				trainState = false;
			}

			screen.clear();
			screen.update(-1, 0, false, -1, false);
			screen.present();
			lastFrameElapsed = elapsed;
			continue;
		}
#endif

#ifdef AI_LEARNING_INPUT
		// check evol
		int numSnakeDie = 0;
		int numBadUnit = 0;
		for (int agentId = 0; agentId < MAX_AI_UNIT; agentId++)
		{
			if (snake[agentId].isDie())
			{
				numSnakeDie++;

				if (snake[agentId].getAIUnit()->Scored == 0)
					numBadUnit++;
			}
		}

		if (numSnakeDie == MAX_AI_UNIT)
		{
			// all die
			if (--testTime <= 0)
			{
				testTime = 0;
				bool resetAll = false;
				if (numBadUnit > MAX_AI_UNIT - 1)
				{
					waitResetTime = waitResetTime - frameTime;
					if (waitResetTime < 0)
					{
						resetAll = true;

						// need retest 5 times
						if (--numberTimeTestBadUnit <= 0)
						{
							// destroy & train bad snake again
							aiGenetic.reset();

							// that will train again AI in next frame
							trainState = true;
							trainProgress = 0;

							numberTimeTestBadUnit = 2;
							testTime = maxTest;
						}
					}
				}
				else
				{
					waitResetTime = waitResetTime - frameTime;
					if (waitResetTime < 0)
					{
						aiGenetic.evolvePopulation();
						resetAll = true;

						gen++;
						char title[512];
						testTime = maxTest;
						sprintf(title, "Generation: %d - test: %d/%d - skipOldGen: %d", gen, maxTest - testTime + 1, maxTest, autoSkipOldGeneration);
						screen.log(title);

						// save process to file
						ANN::CMemoryStream data;
						aiGenetic.serialize(&data);
						FILE* f = fopen(lastTrainData, "wb");
						fwrite(data.getData(), data.getSize(), 1, f);
						fclose(f);
					}
				}

				if (resetAll)
				{
					waitResetTime = 1000;
					lastFrameElapsed = elapsed;

					allFoods.clear();
					allFoods.push_back(Food());

					std::vector<ANN::SUnit*>& units = aiGenetic.get();
					for (int i = 0, n = (int)units.size(); i < n; i++)
					{
						units[i]->Scored = 0;
						snake[i].setAIUnit(units[i]);
						snake[i].reset();
						foodId[i] = 0;
						timeEatFood[i] = 0;
						food[i] = allFoods[0];
						score[i] = 0;
						snake[i].live();
						waitStuckTime[i] = maxTime;
						currentGenerationID[i] = aiGenetic.get()[i]->ID;
					}
				}
			}
			else
			{
				{
					// continue test score
					allFoods.clear();
					allFoods.push_back(Food());

					std::vector<ANN::SUnit*>& units = aiGenetic.get();
					for (int i = 0, n = (int)units.size(); i < n; i++)
					{
						snake[i].reset();
						foodId[i] = 0;
						timeEatFood[i] = 0;
						food[i] = allFoods[0];
						snake[i].live();
						waitStuckTime[i] = maxTime;
					}

					char title[512];
					sprintf(title, "Generation: %d - test: %d/%d - skipOldGen: %d", gen, maxTest - testTime + 1, maxTest, autoSkipOldGeneration);
					screen.log(title);

					waitResetTime = 1000;
				}
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
#ifdef AI_LEARNING_INPUT
				if (!pause)
				{
					autoSkipOldGeneration = !autoSkipOldGeneration;

					char title[512];
					testTime = maxTest;
					sprintf(title, "Generation: %d - test: %d/%d - skipOldGen: %d", gen, maxTest - testTime + 1, maxTest, autoSkipOldGeneration);
					screen.log(title);
				}
#endif
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
					waitStuckTime[agentId] = maxTime;
				}
#endif
				break;
			};

			bool topUnit = false;
			int aiId = 0;

#ifdef AI_LEARNING_INPUT
			topUnit = snake[agentId].getAIUnit()->TopUnit;
			aiId = snake[agentId].getAIUnit()->ID;
#endif

			if (!snake[agentId].isDie())
			{
				screen.update(score[agentId], gen, false, agentId, topUnit, aiId);

#ifdef AI_LEARNING_INPUT
				std::vector<double> input;
				getAIInputOutput(snake[agentId], food[agentId], walls[agentId], input, NULL);

				ANN::SUnit* aiUnit = snake[agentId].getAIUnit();
				aiUnit->ANN->Predict = [](const double* output, int numOutput)
				{
					double max = output[0];
					int result = 0;

					for (int i = 1; i < numOutput; i++)
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
				waitStuckTime[agentId] = waitStuckTime[agentId] - frameTime;
				timeEatFood[agentId] = timeEatFood[agentId] + frameTime;

#ifndef AI_LEARNING_INPUT
				bool needSaveInput = false;
				autoSaveTime = autoSaveTime - frameTime;
				if (autoSaveTime < 0)
				{
					autoSaveTime = autoSaveInputTime;
					needSaveInput = true;
				}
#endif

				if (elapsed - lastElapsed > moveSpeed) {
#ifndef AI_LEARNING_INPUT
					std::vector<double>& input = snake[agentId].getInput();
					std::vector<double>& output = snake[agentId].getOutput();

					getAIInputOutput(snake[agentId], food[agentId], walls[agentId], input, &output);

					int numInput = (int)input.size();
					int numOutput = (int)output.size();
#endif
					bool changeInput = snake[agentId].getDirection() != snake[agentId].getLastDirection();

					if (!snake[agentId].move())
						gameOver(snake[agentId], food[agentId], starting);
					else {
						if (snake[agentId].collidesWith(food[agentId])) {

#ifndef AI_LEARNING_INPUT
							needSaveInput = true;
#endif
							foodId[agentId]++;
							int f = foodId[agentId];
							if (f >= allFoods.size())
								allFoods.push_back(Food());

							float timeEatFoodSec = timeEatFood[agentId] / 1000.0f;
							if (timeEatFoodSec < 1.0f)
								timeEatFoodSec = 1.0f;

							food[agentId] = allFoods[f];
							timeEatFood[agentId] = 0;

							// that will select the snake eat food fast
							score[agentId] += (int)(Food::S_VALUE / timeEatFoodSec);

							snake[agentId].addSection();

							// set limit time to eat food
							waitStuckTime[agentId] = maxTime;
						}

						for (auto wall : walls[agentId])
							if (snake[agentId].collidesWith(*wall))
								gameOver(snake[agentId], food[agentId], starting);

						for (int i = 1; i < snake[agentId].m_sections.size(); i++)
							if (snake[agentId].collidesWith(*snake[agentId].m_sections[i]))
								gameOver(snake[agentId], food[agentId], starting);
					}

					if (waitStuckTime[agentId] < 0)
					{
#ifdef AI_LEARNING_INPUT
						// note: ai stuck
						snake[agentId].getAIUnit()->Scored /= 2;
						score[agentId] /= 2;
#endif
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
				screen.update(score[agentId], gen, true, agentId, topUnit, aiId);
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

		if (elapsed - lastElapsed > moveSpeed)
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
