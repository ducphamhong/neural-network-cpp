# neural-network-cpp
This project is the simple Artificial neural networks (ANN), which are computational systems that “learn” to perform tasks. This project is a library written in C++ language and it is easy to embed in another project.

## XOR
In this example teach the AI learns to compute XOR function.

Code example
``` C++
#include "pch.h"
#include "ANN.h"

int main()
{
	// input: 2
	// hidden: 4
	// output: 1
	int dim[3] = { 2,4,1 };

	// train data
	double input[] = {
		0,0,
		0,1,
		1,0,
		1,1
	};

	double output[] = {
		0,
		1,
		1,
		0
	};

	// init 3 layers
	ANN::CANN ann(dim, 3);

	ann.LearnExpected = [](double* trainData, int trainId, double* expectedOutput, int numOutput) {
		// XOR will return 0, 1
		expectedOutput[0] = trainData[trainId];
	};

	ann.Predict = [](const double* output, int numOutput)
	{
		// Convert output layer to XOR result
		if (output[0] >= 0.5)
			return 1;
		else
			return 0;
	};

	// train
	printf("Begin learning\n");
	for (int i = 0; i < 10000; i++)
	{
		ann.train(input, output, 4);
	}

	// test learning
	printf("Result\n");
	double* p = input;
	for (int i = 0; i < 4; i++)
	{
		double ret = ann.predict(p);
		printf("%d xor %d = %d\n", (int)p[0], (int)p[1], (int)ret);
		p += 2;
	}
	return 1;
}
```

## Digit Classification
For this example, the AI learns to recognize a small image file of 28x28 pixels.

You need extract the data resource

<img src="Image/DigitClassificationExtract.png"/>

The size image 28x28 = 784 inputs

<img src="Image/DigitNetwork.png"/>

```C++
// input: 784
// hidden: 32
// output: 10
int dim[] = { 784, 32, 10 };

// init 3 layers
ANN::CANN ann(dim, 3);

ann.LearnExpected = [](double* trainData, int trainId, double* expectedOutput, int numOutput)
{
	// ex: 3 => [0 0 0 1 0 0 0 0 0 0]
	int valueExpected = (int)trainData[trainId];
	for (int i = 0; i < 10; i++)
		expectedOutput[i] = (i == valueExpected) ? 1.0 : 0.0;
};

ann.Predict = [](const double* output, int numOutput)
{
	// ex: [0 0 0 1 0 0 0 0 0 0] => return 3
	double ret = 0;
	double max = output[0];
	for (int i = 0; i < 10; i++)
	{
		if (max < output[i])
		{
			max = output[i];
			ret = (double)i;
		}
	}
	return ret;
};

double* getANNInput(SPNGImage* img)
{
	int size = img->Width * img->Height;

	double* ret = new double[size];
	unsigned char* data = img->Data;

	for (int i = 0; i < size; i++)
	{
		ret[i] = data[0] / 255.0;
		data = data + img->BPP;
	}

	return ret;
}

void train(ANN::CANN* ann, const char* file, double targetValue)
{
	SPNGImage img;
	if (loadPNG(file, &img))
	{
		double* input = getANNInput(&img);
		double output[] = { targetValue };
		ann->train(input, output, 1);
		delete input;
	}
}
```

And the result test:

<img src="Image/DigitClassification.png"/>

## Flappy Doge

For this example, the AI ​​will learn how to control the game Flappy Doge from human control.

Then they will improve themselves to play better.

And after about 20 generations, it can play good this game.

I try to use 2 hidden layers with 6 neutrals

```C++
ANN::CGeneticAlgorithm aiGenetic;
const int dim[] = { 4, 6, 6, 1 };
aiGenetic.createPopulation(MAX_AI_UNIT, dim, 4);
```

<img src="Image/FlappyDoge.png"/>

## Snake Game

For this example, the AI ​​will learn how to control the game Snake.

https://github.com/ducphamhong/neural-network-cpp/assets/57616039/e262d585-394e-46c5-8472-70a0b09f47f7


```C++
ANN::CGeneticAlgorithm aiGenetic(ANN::EActivation::Tanh);
const int dim[] = { 14, 128, 128, 4 };
aiGenetic.createPopulation(MAX_AI_UNIT, dim, 4);

// And we use 14 inout and 4 output
// See the function getAIInputOutput in main.cpp

///////////////////////////////////
// INPUT
///////////////////////////////////

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

int t, d, l, r;
getSafeRange(snake, walls, t, d, l, r);

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

///////////////////////////////////
// OUTPUT
///////////////////////////////////

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
```

## References:
[Machine learning](https://zitaoshen.rbind.io/project/machine_learning/how-to-build-your-own-neural-net-from-the-scrach)

[ANN In C](https://github.com/gkrishnan724/ANN-IN-C)

[FlappyDoge project](https://github.com/Ducanger/Flappy-Doge-SDL2)

[Flappy Bird ML](https://github.com/ssusnic/Machine-Learning-Flappy-Bird)

[Snake game project](https://github.com/jcalvarezj/snake)
