/*
 * Snake game program using the SDL library
 *
 * @author J. Alvarez
 */
#include "Collideable.hpp"

namespace SnakeGame {

	Collideable::Collideable(int x, int y) : Drawable(x, y) {}

	bool Collideable::collidesWith(Collideable& other) {
		return m_x == other.m_x && m_y == other.m_y;
	}

	bool Collideable::collidesWith(int x, int y)
	{
		return m_x == x && m_y == y;
	}


} // namespace SnakeGame