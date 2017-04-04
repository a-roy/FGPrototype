#pragma once
#include "InputHistory.h"
#include <bitset>

struct PlacementComponent
{
	int X;
	int Y;
	int O;
};

struct ActionComponent
{
	int Character;
	int Current;
	int Time;
	int CancelList;
};

struct InputComponent
{
	int Controls;
	InputHistory Inputs;
};

struct VisibleComponent
{
	int Model;
};

const int num_entities = 20;
struct EntityList
{
	std::bitset<4> enabled[num_entities];
	PlacementComponent placement[num_entities];
	ActionComponent action[num_entities];
	InputComponent input[num_entities];
	VisibleComponent visible[num_entities];
};

enum ComponentIndex
{
	IPlacement = 0,
	IAction,
	IInput,
	IVisible
};