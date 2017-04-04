#include "Direction.h"
#include <cmath>

Direction::Direction(unsigned char num)
{
	Numpad = num;
}

int Direction::Distance(Direction dir1, Direction dir2)
{
	return std::abs((dir2.Numpad - 1) / 3 - (dir1.Numpad - 1) / 3)
		+ std::abs((dir2.Numpad - 1) % 3 - (dir1.Numpad - 1) % 3);
}