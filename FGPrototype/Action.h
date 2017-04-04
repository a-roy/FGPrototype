//! \file
//! Action class declaration
#pragma once

#include "Command.h"
#include <string>
#include <vector>

//! Describes an action performable by an ActiveUnit
class Action
{
public:
	Action() { }
	Action(int id, int len);

	unsigned int ID;
	std::string Name;
	//! Commands for performing this action
	std::vector<Command> Commands;
	//! The action to perform once this action ends. Use Unit::MoveList to lookup.
	int RecoverTo;
	int Length;
};
