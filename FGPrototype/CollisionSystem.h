#pragma once
#include "Component.h"
#include "Hitbox.h"

class CollisionSystem
{
	void Test(EntityList& entities);
};

void CollisionSystem::Test(EntityList& entities)
{
	for (int i = 0; i < num_entities; i++)
	{
		if (entities.enabled[i][IPlacement] && entities.enabled[i][IAction])
		{
			// lookup hitboxes
		}
	}
	// set up loop
	{
		Hitbox h1, h2;
		Hitbox *act;
		Hitbox *react;
		// for now we don't really care if both boxes have both flags
		if ((h1.Flags & Act) && (h2.Flags & React))
		{
			act = &h1;
			react = &h2;
		}
		else if ((h2.Flags & Act) && (h1.Flags & React))
		{
			act = &h2;
			react = &h1;
		}
		//else continue;
		HitboxFlags actflags = (HitboxFlags)(act->Flags & ~Act & ~React);
		HitboxFlags reactflags = (HitboxFlags)(react->Flags & ~Act & ~React);
		if ((reactflags & actflags) == actflags)
		{
			// something happens
		}
	}
}