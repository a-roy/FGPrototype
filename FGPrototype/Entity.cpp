#include "Entity.h"

void Entity::ActCancel(int list)
{
	Entities->action[Index].CancelList = list;
}

void Entity::ActMove(int distance)
{
	Entities->placement[Index].X += Entities->placement[Index].O * distance;
}