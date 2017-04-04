//! \file
//! Input class declaration
#pragma once

#include "FrameInput.h"
#include <vector>

//! Describes a single input for a command.
class Input
{
public:
	enum InputOptions : unsigned short
	{
		None = 0x00,
		Required = 0x01,
		PressOK = 0x02,
		ReleaseOK = 0x04,
		HoldOK = 0x0A, // includes the PressOK flag
		IncreaseBuffer = 0x10
	};

	Input(std::vector<FrameInput> val, InputOptions o, int charge = 0);
	//! Returns the possible FrameInput values that can stand for this Input.
	const std::vector<FrameInput>& GetValue() const;
	//! This input is required in its command.
	bool IsRequired() const;
	//! This input can be activated by pressing it.
	bool IsPressOK() const;
	//! This input can be activated by releasing it.
	bool IsReleaseOK() const;
	//! This input can be activated by holding it.
	bool IsHoldOK() const;
	//! This input has an extra buffer between itself and the following input being activated.
	bool IsIncreaseBuffer() const;

	//! Required charge (hold) time of input
	int Charge;
private:
	std::vector<FrameInput> value;
	InputOptions options;
};