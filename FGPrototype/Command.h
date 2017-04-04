//! \file
//! Command class declaration
#pragma once

#include "Input.h"
#include <iostream>
#include <string>
#include <vector>

//! Describes interpretable inputs for in-game actions
class Command
{
public:
	//! Constructor
	//!
	//! \param inputs The inputs comprising the command in chronological order.
	Command(std::vector<Input> inputs);
	//! Get the inputs comprising the command.
	const std::vector<Input>& GetData() const;
	//! Get a string representation of the command.
	const std::string ToString() const;
	static std::string ButtonName(Buttons b);
private:
	std::vector<Input> data;
};

std::ostream& operator <<(std::ostream& out, const Command& c);