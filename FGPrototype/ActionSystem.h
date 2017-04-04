#pragma once
#include "Component.h"
#include "Unit.h"
extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

class ActionSystem
{
public:
	void Advance(EntityList& entities);

	std::vector<Unit *> Characters;
	lua_State *L;
};
