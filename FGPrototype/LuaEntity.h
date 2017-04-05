#pragma once

struct EntityList;
struct lua_State;

int luaopen_Entity(lua_State *);
void luaU_pushEntity(lua_State *L, EntityList *e, int i);
EntityList *luaU_toEntityList(lua_State *L, int n);
int luaU_toEntityI(lua_State *L, int n);