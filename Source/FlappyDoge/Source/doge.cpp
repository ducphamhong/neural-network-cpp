#include "doge.h"
#include <stdio.h>
#include <fstream>

bool doge::init(bool isDark)
{
	die = true;
	score = 0;
	jumpTime = 0;

#ifdef AI_LEARNING_INPUT
	unit = NULL;
#endif

	string shiba_path = "FlappyDoge/image/shiba.png";
	if (isDark) shiba_path = "FlappyDoge/image/shiba-dark.png";
	if (saved_path == shiba_path)
	{
		posDoge.getPos(75, SCREEN_HEIGHT / 2 - 10);
		ahead = 0;
		angle = 0;
	}
	if (isNULL() || saved_path != shiba_path)
	{
		saved_path = shiba_path;
		if (Load(shiba_path.c_str(), 1))
		{
			return true;
		}
		else
		{
			return false;
		}

	}
	return false;
}

void doge::Free()
{
	free();
}

void doge::render()
{
	Render(posDoge.x, posDoge.y, angle);
}

void doge::fall()
{
	if (die && posDoge.y < SCREEN_HEIGHT - LAND_HEIGHT - SHIBA_HEIGHT - 5)
	{
		if (time == 0)
		{
			x0 = posDoge.y;
			angle = -25;
		}
		else if (angle < 70 && time > 30)
		{
			angle += 3;
		}

		if (time >= 0)
		{
			posDoge.y = x0 + time * time * 0.18 - 7.3 * time;
			time++;
		}
	}
	else
		return;
}

#ifdef AI_LEARNING_INPUT
void doge::setAIUnit(ANN::SUnit* u)
{
	unit = u;
	unit->ANN->Predict = [](const double* output, int numOutput)
	{
		if (output[0] > 0.5)
			return 1.0;
		return 0.0;
	};
}
#endif

void doge::update(short int pipeWidth, short int pipeHeight)
{
	if (!die)
	{
		if (time == 0)
		{
			x0 = posDoge.y;
			angle = -25;
		}
		else if (angle < 70 && time > 30)
		{
			angle += 3;
		}

		if (time >= 0)
		{
			posDoge.y = x0 + time * time * 0.18 - 7.3 * time;
			time++;
		}

		distance += 3;

		int cx = posDoge.x + getWidth() / 2;
		int cy = posDoge.y + getHeight() / 2;
		int tx = posPipe[ahead].x + pipeWidth;
		int ty = posPipe[ahead].y + pipeHeight + PIPE_SPACE / 2;

		SDL_RenderDrawLine(context::gRenderer,
			cx, cy,
			tx, ty);

		int dx = tx - cx;
		int dy = -(ty - cy);

		double distanceToTarget = sqrt(dx * dx + dy * dy);

		double input[2];
		input[0] = dx / (double)SCREEN_WIDTH;
		input[1] = dy / (double)SCREEN_HEIGHT;

#ifdef AI_LEARNING_INPUT		
		if (!die && unit != NULL)
		{
			if (--jumpTime < 0)
			{
				jumpTime = 5;

				double output = unit->ANN->predict(input);
				if (output == 1.0)
				{
					resetTime(); // flap
				}
			}
		}
#else
		double output = 0.0;
		if (time <= 10)
		{
			output = 1.0;
		}

		// save data for learning from human play
		dataInput.push_back(input[0]);
		dataInput.push_back(input[1]);
		dataOutput.push_back(output);
#endif

		if ((posDoge.x + getWidth() > posPipe[ahead].x + 5) &&
			(posDoge.x + 5 < posPipe[ahead].x + pipeWidth) &&
			(posDoge.y + 5 < posPipe[ahead].y + pipeHeight || posDoge.y + getHeight() > posPipe[ahead].y + pipeHeight + PIPE_SPACE + 5))
		{
			die = true;
			reportDie(distanceToTarget);
		}
		else if (posDoge.x > posPipe[ahead].x + pipeWidth)
		{
			ahead = (ahead + 1) % TOTAL_PIPE;
			score++;
			if (score > context::score)
				context::score = score;
		}

		if (posDoge.y > SCREEN_HEIGHT - LAND_HEIGHT - SHIBA_HEIGHT - 5 || posDoge.y < -10)
		{
			die = true;
			reportDie(distanceToTarget);
		}
	}
}

void doge::reportDie(double distanceToTarget)
{
#ifdef AI_LEARNING_INPUT
	if (unit)
	{
		// report score
		double newScore = distance - distanceToTarget;
		if (newScore > unit->Scored)
			unit->Scored = newScore;

		if (unit->Scored < 0)
			unit->Good = false;
		else
			unit->Good = true;
	}
#else
	// save data for learning
	FILE* f = fopen("FlappyDoge/data/learning.txt", "wt");

	int numData = (int)dataOutput.size();
	for (int i = 0; i < numData; i++)
	{
		double i1 = dataInput[i * 2];
		double i2 = dataInput[i * 2 + 1];
		double o = dataOutput[i];

		fprintf(f, "%lf %lf %lf\n", i1, i2, o);
	}

	fclose(f);
#endif
}