//! \file
//! FrameInput class declaration
#pragma once

#include "Buttons.h"
#include "Direction.h"
#include <vector>

//! Stores one frame of player input.
class FrameInput
{
public:
	Direction Dir;
	Buttons Pressed : 6;

	FrameInput();
	FrameInput(Direction d, Buttons b);
	//! Determine whether a specific input is pressed.
	//!
	//! \param x      An input to check for.
	//! \retval true  One of the following conditions is met:
	//!                 - _x_ has no direction defined and all buttons are pressed
	//!                 - _x_ has no buttons defined and the direction is held
	//!                 - all buttons are pressed and the direction is held
	//! \retval false One of the following conditions is met:
	//!                 - any buttons defined by _x_ are not pressed
	//!                 - _x_ has a direction defined and it is not held
	bool IsPressed(const FrameInput& x) const;
	//! Determine whether a specific direction is pressed.
	bool IsPressed(Direction d) const;
	//! Determine whether all of the buttons defined by _b_ are pressed.
	bool IsPressed(Buttons b) const;
	//! Determine whether the inputs defined by any member of _x_ are pressed.
	//!
	//! \param x A vector of possible inputs.
	bool IsPressed(const std::vector<FrameInput>& x) const;
	//! Determine whether any of the buttons defined by _b_ are pressed.
	bool AnyPressed(Buttons b) const;
	//! Determine whether a specific input is released.
	bool IsReleased(const FrameInput& x) const;
	//! Determine whether the inputs defined by any member of _x_ are released.
	bool IsReleased(const std::vector<FrameInput>& x) const;
	//! Reverses the left-right direction of this input
	void Mirror();
};