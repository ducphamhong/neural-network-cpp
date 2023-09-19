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
#include "SDL.h"
#include <Windows.h>

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

void resetLevel(Snake& snake, Food& food, bool& starting) {
	snake.die();
	snake.reset();
	food = Food();
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

#ifndef AI_LEARNING_INPUT
double booleanInput(bool b)
{
	return b ? 1.0 : 0.0;
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

void getAIInputOutput(Snake& snake, Food& food, std::vector<Wall*>& walls)
{
	std::vector<double>& input = snake.getInput();
	std::vector<double>& output = snake.getOutput();
	int inputDirection = snake.getLastDirection();
	int outputDirection = snake.getDirection();

	input.clear();
	output.clear();

	int x = snake.getSections()[0]->m_x;
	int y = snake.getSections()[0]->m_y;
	int foodX = food.m_x;
	int foodY = food.m_y;

	// danger left	
	input.push_back(booleanInput(isDanger(Snake::Direction::LEFT, snake, walls)));
	// danger right
	input.push_back(booleanInput(isDanger(Snake::Direction::RIGHT, snake, walls)));
	// danger up
	input.push_back(booleanInput(isDanger(Snake::Direction::UP, snake, walls)));
	// danger down
	input.push_back(booleanInput(isDanger(Snake::Direction::DOWN, snake, walls)));
	// is current left	
	input.push_back(booleanInput(inputDirection == Snake::Direction::LEFT));
	// is current right
	input.push_back(booleanInput(inputDirection == Snake::Direction::RIGHT));
	// is current up
	input.push_back(booleanInput(inputDirection == Snake::Direction::UP));
	// is current down
	input.push_back(booleanInput(inputDirection == Snake::Direction::DOWN));
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

	std::vector<Wall*> walls[MAX_AI_UNIT];
	int score[MAX_AI_UNIT];

	for (int i = 0; i < MAX_AI_UNIT; i++)
	{
		createWalls(walls[i]);
		score[i] = 0;
	}

	if (!screen.init()) {
		SDL_Log("Error initializing screen");
		return -1;
	}

	bool quit = false;
	bool starting = true;
	bool pause = false;
	int lastElapsed = 0;

#ifndef AI_LEARNING_INPUT
	int delay = 200;
#else
	int delay = 100;
#endif

	while (!quit) {
		int action = screen.processEvents();

		if (pause)
			quit = pauseGame(screen, pause);

		int elapsed = SDL_GetTicks();
		if (lastElapsed == 0)
			lastElapsed = elapsed;

		for (int agentId = 0; agentId < MAX_AI_UNIT; agentId++)
		{
			screen.clear();

			snake[agentId].draw(screen);
			food[agentId].draw(screen);

			drawWalls(walls[agentId], screen);

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
				if (snake[agentId].isDie())
				{
					score[agentId] = 0;
					snake[agentId].live();
				}
				break;
			};

			if (!snake[agentId].isDie())
			{
				screen.update(score[agentId], false, agentId);

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

				if (elapsed - lastElapsed > delay) {
					lastElapsed = elapsed;

#ifndef AI_LEARNING_INPUT
					getAIInputOutput(snake[agentId], food[agentId], walls[agentId]);
#endif
					if (!snake[agentId].move())
						resetLevel(snake[agentId], food[agentId], starting);
					else {
						if (snake[agentId].collidesWith(food[agentId])) {
							food[agentId] = Food();
							score[agentId] += Food::S_VALUE;
							snake[agentId].addSection();
						}

						for (auto wall : walls[agentId])
							if (snake[agentId].collidesWith(*wall))
								resetLevel(snake[agentId], food[agentId], starting);

						for (int i = 1; i < snake[agentId].m_sections.size(); i++)
							if (snake[agentId].collidesWith(*snake[agentId].m_sections[i]))
								resetLevel(snake[agentId], food[agentId], starting);
					}
				}
			}
			else
			{
				screen.update(score[agentId], true, agentId);
			}
		}

		screen.present();
	}

	for (int i = 0; i < MAX_AI_UNIT; i++)
	{
		freeWalls(walls[i]);
	}

	screen.close();

	return 0;
}
