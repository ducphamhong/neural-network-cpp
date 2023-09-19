/*
 * Snake game program using the SDL library
 *
 * @author J. Alvarez
 */
#include "Screen.hpp"
#include "Section.hpp"
#include "Snake.hpp"

namespace SnakeGame {

	cint Section::S_SECTION_WIDTH = (cint)(20 * S_SCALE);

	Section::Section() : Collideable(0, 0) {}

	Section::Section(int x, int y) :
		Collideable(x, y) {}

	void Section::draw(Screen& screen)
	{
		Uint8 r = ID == 0 ? Snake::S_HEAD_RED : Snake::S_SNK_RED;
		Uint8 g = ID == 0 ? Snake::S_HEAD_GREEN : Snake::S_SNK_GREEN;
		Uint8 b = ID == 0 ? Snake::S_HEAD_BLUE : Snake::S_SNK_BLUE;

		for (int i = 0; i < S_SECTION_WIDTH; i++)
		{
			for (int j = 0; j < S_SECTION_WIDTH; j++)
			{
				screen.setPixel((int)m_x + i, (int)m_y + j, r, g, b);
			}
		}
	}

	void Section::move(int direction) {
		switch (direction) {
		case Snake::Direction::UP:
			m_y -= S_SECTION_WIDTH;
			break;
		case Snake::Direction::DOWN:
			m_y += S_SECTION_WIDTH;
			break;
		case Snake::Direction::LEFT:
			m_x -= S_SECTION_WIDTH;
			break;
		case Snake::Direction::RIGHT:
			m_x += S_SECTION_WIDTH;
			break;
		}
	}

	int Section::calculateDirection(Section& other) {
		if (other.m_x - m_x == 0) {
			if (other.m_y - m_y < 0)
				return Snake::Direction::UP;
			else
				return Snake::Direction::DOWN;
		}
		else {
			if (other.m_x - m_x > 0)
				return Snake::Direction::RIGHT;
			else
				return Snake::Direction::LEFT;
		}
	}

	void Section::toString() { // TODO Remove - Used for debugging
		SDL_Log("<%d, %d>", m_x, m_y);
	}

} // namespace SnakeGame
