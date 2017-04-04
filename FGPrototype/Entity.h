#pragma once
#include "Component.h"

class Entity
{
public:
	Entity(EntityList *entities, int index) :
	  Entities(entities), Index(index) { }
	void ActCancel(int list);
	void ActMove(int distance);

	EntityList *Entities;
	int Index;
};
