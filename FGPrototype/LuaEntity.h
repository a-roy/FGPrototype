#pragma once

class Entity;
struct lua_State;

int luaopen_Entity(lua_State *);
void luaU_pushEntity(lua_State *L, Entity *e);
Entity *luaU_toEntity(lua_State *L, int n);