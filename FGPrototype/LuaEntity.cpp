#include "LuaEntity.h"
#include "Entity.h"

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

int Entity_time(lua_State *L)
{
	Entity *e = luaU_toEntity(L, 1);
	int lower = luaL_checkinteger(L, 2);
	int upper = luaL_checkinteger(L, 3);
	int t = e->Entities->action[e->Index].Time;
	bool b = (t >= lower && t <= upper);
	lua_pushboolean(L, b);
	return 1;
}

int Entity_cancel(lua_State *L)
{
	Entity *e = luaU_toEntity(L, 1);
	int l = luaL_checkinteger(L, 2);
	e->ActCancel(l);
	return 0;
}

int Entity_move(lua_State *L)
{
	Entity *e = luaU_toEntity(L, 1);
	int d = luaL_checkinteger(L, 2);
	e->ActMove(d);
	return 0;
}

static luaL_Reg Entity_table[] =
{
	{ NULL, NULL }
};

static luaL_Reg Entity_metatable[] =
{
	{ "time", Entity_time },
	{ "cancel", Entity_cancel },
	{ "move", Entity_move },
	{ NULL, NULL }
};

int luaopen_Entity(lua_State *L)
{
	luaL_newmetatable(L, "Entity");
	luaL_newlib(L, Entity_metatable);
	lua_setfield(L, -2, "__index");
	lua_pop(L, 1);
	return 1;
}

void luaU_pushEntity(lua_State *L, Entity *e)
{
	lua_createtable(L, 1, 0);
	lua_pushlightuserdata(L, (void *)e);
	lua_rawseti(L, -2, 1);
	luaL_setmetatable(L, "Entity");
}

Entity *luaU_toEntity(lua_State *L, int n)
{
	lua_rawgeti(L, n, 1);
	Entity *e = static_cast<Entity *>(lua_touserdata(L, -1));
	lua_pop(L, 1);
	return e;
}