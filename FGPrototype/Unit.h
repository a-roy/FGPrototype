//! \file
//! Unit class declaration
#pragma once

#include "Action.h"
#include <map>
#include <string>

class Unit
{
public:
	//! Adds a move to this character's move list.
	void AddMove(Action& move);
	//! Used for accessing vector-organized data
	int CharID;
	std::string Name;
	//! Contains all actions for this character, accessible by index.
	std::vector<Action> MoveList;
	//! Contains "cancel lists"
	std::vector<std::vector<int> > Groups;
	//! The default action, for a unit not doing anything
	int NeutralAction;

	//! Get the neutral action for this unit
	const Action& GetNeutralAction();
};