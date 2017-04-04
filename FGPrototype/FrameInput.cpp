#include "FrameInput.h"

FrameInput::FrameInput()
{
	Dir = 0;
	Pressed = None;
}

FrameInput::FrameInput(Direction d, Buttons b)
{
	Dir = d;
	Pressed = b;
}

bool FrameInput::IsPressed(const FrameInput& x) const
{
	// if there are no buttons and the direction matches, return true
	// if there is no direction and the buttons are pressed, return true
	// if the direction matches and the buttons are pressed, return true
	return (x.Pressed == None && Dir.Numpad == x.Dir.Numpad)
		|| (IsPressed(x.Dir) && IsPressed(x.Pressed));
}

bool FrameInput::IsPressed(Direction d) const
{
	// if there is no direction, return true
	// if the direction matches, return true
	return (d.Numpad == 0) || (Dir.Numpad == d.Numpad);
}

bool FrameInput::IsPressed(Buttons b) const
{
	// if the buttons are pressed, return true
	return (Pressed & b) == b;
}

bool FrameInput::IsPressed(const std::vector<FrameInput>& x) const
{
	for (unsigned int a = 0, x_size = x.size(); a < x_size; a++)
	{
		if (IsPressed(x[a]))
		{
			return true;
		}
	}
	return false;
}

bool FrameInput::AnyPressed(Buttons b) const
{
	return (Pressed & b) != None;
}

bool FrameInput::IsReleased(const FrameInput& x) const
{
	Buttons buttons = x.Pressed;
	if (buttons == None)
	{
		return !IsPressed(x);
	}
	else
	{
		return !AnyPressed(buttons);
	}
}

bool FrameInput::IsReleased(const std::vector<FrameInput>& x) const
{
	for (unsigned int a = 0, x_size = x.size(); a < x_size; a++)
	{
		if (!IsReleased(x[a]))
		{
			return false;
		}
	}
	return true;
}

void FrameInput::Mirror()
{
	if (Dir.Numpad % 3 == 1)
	{
		Dir.Numpad += 2;
	}
	else if (Dir.Numpad % 3 == 0)
	{
		Dir.Numpad -= 2;
	}
}
