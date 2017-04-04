//! \file
//! Direction class declaration
#pragma once

//! Represents an 8-way direction.
struct Direction
{
	unsigned char Numpad;

	Direction(unsigned char num = 0);
	//! Calculate the taxicab distance between two directions.
	static int Distance(Direction dir1, Direction dir2);
};