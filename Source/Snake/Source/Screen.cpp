/*
 * Snake game program using the SDL library
 *
 * @author J. Alvarez
 */
#include "Screen.hpp"
#include "SDL.h"
#include <iostream>
#include <cstring>
#include <sstream>

typedef const unsigned int cint;

#ifdef AI_LEARNING_INPUT
float S_SCALE = 0.25f;
#else
float S_SCALE = 1.0f;
#endif

namespace SnakeGame {
	cint Screen::S_WIDTH = (cint)(800 * S_SCALE);
	cint Screen::S_HEIGHT = (cint)(600 * S_SCALE);
	cint Screen::S_TEXT_X = (cint)(20 * S_SCALE);
	cint Screen::S_TEXT_Y = (cint)(0 * S_SCALE);
	const std::string Screen::S_SCORE_TEXT = "Score: ";

	Screen::Screen() : m_window(NULL), m_renderer(NULL), m_texture(NULL),
		m_mainBuffer(NULL), m_sansFont(NULL), m_textSurface(NULL),
		m_textTexture(NULL) {}

	bool Screen::init() {
		if (SDL_Init(SDL_INIT_VIDEO) < 0) {
			std::cout << "Could not initialize SDL graphics" << std::endl;
			return false;
		}

		TTF_Init();
		m_sansFont = TTF_OpenFont("Snake/Roboto-Regular.ttf", 18);

		if (!m_sansFont)
			SDL_Log("Error. Could not load font");

		m_window = SDL_CreateWindow("Snake Game",
			SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
			S_WIDTH / S_SCALE,
			S_HEIGHT / S_SCALE,
			SDL_WINDOW_SHOWN);

		if (!m_window) {
			SDL_Log("Could not create the window. ");
			SDL_Log("%s", SDL_GetError());
			TTF_Quit();
			SDL_Quit();
			return false;
		}

		m_renderer = SDL_CreateRenderer(m_window, -1,
			SDL_RENDERER_PRESENTVSYNC);

		if (!m_renderer) {
			SDL_Log("Could not create the renderer. ");
			SDL_Log("%s", SDL_GetError());
			SDL_DestroyWindow(m_window);
			TTF_Quit();
			SDL_Quit();
			return false;
		}

		m_texture = SDL_CreateTexture(m_renderer, SDL_PIXELFORMAT_RGBA8888,
			SDL_TEXTUREACCESS_STATIC, S_WIDTH, S_HEIGHT);

		if (!m_texture) {
			SDL_Log("Could not create the texture. ");
			SDL_Log("%s", SDL_GetError());
			SDL_DestroyRenderer(m_renderer);
			SDL_DestroyWindow(m_window);
			TTF_Quit();
			SDL_Quit();
			return false;
		}

		m_mainBuffer = new Uint32[S_WIDTH * S_HEIGHT];

		clear();

		return true;
	}

	int Screen::processEvents() {
		SDL_Event event;

		int action = -1;

		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT:
				action = Action::QUIT;
				break;
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym) {
				case SDLK_LEFT:
					action = Action::MOVE_LEFT;
					break;
				case SDLK_RIGHT:
					action = Action::MOVE_RIGHT;
					break;
				case SDLK_DOWN:
					action = Action::MOVE_DOWN;
					break;
				case SDLK_UP:
					action = Action::MOVE_UP;
					break;
				case SDLK_RETURN:
					action = Action::PAUSE;
					break;
				}
				break;
			}
		}

		return action;
	}

	void Screen::update(int score, bool isGameOver, int agentID) {
		SDL_UpdateTexture(m_texture, NULL, m_mainBuffer, S_WIDTH * sizeof(Uint32));

		if (agentID < 0)
		{
			SDL_RenderCopy(m_renderer, m_texture, NULL, NULL);
		}
		else
		{
			int row = agentID / 4;
			int col = agentID % 4;

			int w = S_WIDTH;
			int h = S_HEIGHT;

			SDL_Rect target;
			target.x = col * w;
			target.y = row * h;
			target.w = w;
			target.h = h;

			SDL_RenderCopy(m_renderer, m_texture, NULL, &target);
		}

#ifndef AI_LEARNING_INPUT
		drawText(score);

		if (isGameOver)
		{
			// game over

		}
#endif
	}

	void Screen::present()
	{
		SDL_RenderPresent(m_renderer);
	}

	void Screen::setPixel(int x, int y, Uint8 red, Uint8 green, Uint8 blue) {
		if (x >= 0 && x < S_WIDTH && y >= 0 && y < S_HEIGHT) {
			Uint32 color = 0;
			color += red;
			color <<= 8;
			color += green;
			color <<= 8;
			color += blue;
			color <<= 8;
			color += 0xFF;
			m_mainBuffer[y * S_WIDTH + x] = color;
		}
	}

	void Screen::clear() {
		memset(m_mainBuffer, 0, S_WIDTH * S_HEIGHT * sizeof(Uint32));
	}

	void Screen::freeText() {
		TTF_CloseFont(m_sansFont);
		// SDL_FreeSurface(m_textSurface);
		// SDL_DestroyTexture(m_textTexture);
	}

	void Screen::close() {
		delete[] m_mainBuffer;

		freeText();

		SDL_DestroyTexture(m_texture);
		SDL_DestroyRenderer(m_renderer);
		SDL_DestroyWindow(m_window);
		TTF_Quit();
		SDL_Quit();
	}

	std::string Screen::createText(int score) {
		std::stringstream sstr;
		sstr << S_SCORE_TEXT << score;
		return sstr.str();
	}

	void Screen::drawText(int score) {
		std::string text = createText(score);

		SDL_Color whiteColor = { 0xFF, 0xFF, 0xFF };
		m_textSurface = TTF_RenderText_Solid(m_sansFont, text.c_str(), whiteColor);
		m_textTexture = SDL_CreateTextureFromSurface(m_renderer, m_textSurface);

		int w, h;
		SDL_QueryTexture(m_textTexture, NULL, NULL, &w, &h);

		SDL_Rect rectangle = {
			S_TEXT_X, S_TEXT_Y, w, h
		};

		SDL_RenderCopy(m_renderer, m_textTexture, NULL, &rectangle);

		SDL_DestroyTexture(m_textTexture);
		SDL_FreeSurface(m_textSurface);
	}

} // namespace SnakeGame
