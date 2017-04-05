#include "LuaEntity.h"
#include "Component.h"

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

int Entity_time(lua_State *L)
{
	EntityList *e = luaU_toEntityList(L, 1);
	int i = luaU_toEntityI(L, 1);
	int lower = luaL_checkinteger(L, 2);
	int upper = luaL_checkinteger(L, 3);
	int t = e->action[i].Time;
	bool b = (t >= lower && t <= upper);
	lua_pushboolean(L, b);
	return 1;
}

int Entity_cancel(lua_State *L)
{
	EntityList *e = luaU_toEntityList(L, 1);
	int i = luaU_toEntityI(L, 1);
	int l = luaL_checkinteger(L, 2);
	e->action[i].CancelList = l;
	return 0;
}

int Entity_move(lua_State *L)
{
	EntityList *e = luaU_toEntityList(L, 1);
	int i = luaU_toEntityI(L, 1);
	int d = luaL_checkinteger(L, 2);
	e->placement[i].X += e->placement[i].O * d;
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

void luaU_pushEntity(lua_State *L, EntityList *e, int i)
{
	lua_createtable(L, 1, 0);
	lua_pushlightuserdata(L, (void *)e);
	lua_rawseti(L, -2, 1);
	lua_pushinteger(L, i);
	lua_rawseti(L, -2, 2);
	luaL_setmetatable(L, "Entity");
}

EntityList *luaU_toEntityList(lua_State *L, int n)
{
	lua_rawgeti(L, n, 1);
	EntityList *e = static_cast<EntityList *>(lua_touserdata(L, -1));
	lua_pop(L, 1);
	return e;
}

int luaU_toEntityI(lua_State *L, int n)
{
	lua_rawgeti(L, n, 2);
	int i = lua_tointeger(L, -1);
	lua_pop(L, 1);
	return i;
}