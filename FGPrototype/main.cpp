#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Component.h"
#include "InputSystem.h"
#include "ActionSystem.h"
#include "GraphicsSystem.h"
#include "CmdTranslator.h"
#include "ModelImporter.h"
#include "LuaEntity.h"
#include <iostream>

#define ShaderDir "../FGPrototype/shaders/"
#define ModelDir "../FGPrototype/models/"
#define ScriptDir "../FGPrototype/scripts/"

using namespace std;

int runGLFW();
int initGraphics();
int initActions();
int initEntity();

int width = 1024, height = 576;
int frame_rate = 60;

InputSystem inputSys;
ActionSystem actionSys;
GraphicsSystem graphicsSys;
EntityList entities;
Unit *testCharacter;
lua_State *L;

int main(int argc, char* argv[])
{
	L = luaL_newstate();
	luaL_openlibs(L);
	luaopen_Entity(L);
	if (luaL_dofile(L, ScriptDir "test.lua"))
	{
		std::cerr << lua_tostring(L, -1) << std::endl;
		return 1;
	}

	int result;
	result = initActions();
	if (result != 0)
		return result;
	result = initEntity();
	if (result != 0)
		return result;

	std::cout << "Welcome to the Fighting Game Prototype!" << std::endl;
	std::cout << "Controls:" << std::endl;
	std::cout << "  W - Up" << std::endl;
	std::cout << "  A - Left" << std::endl;
	std::cout << "  S - Down" << std::endl;
	std::cout << "  D - Right" << std::endl;
	std::cout << "  U - Button1" << std::endl;
	std::cout << "  I - Button2" << std::endl;
	std::cout << "Moves:" << std::endl;
	std::cout << "  B or F - Walking" << std::endl;
	std::cout << "  1 - Jab" << std::endl;
	std::cout << "  2 - Straight" << std::endl;
	std::cout << "  D,DF,F+1 - Hook" << std::endl;
	std::cout << "  F,D,DF+2 - Uppercut" << std::endl;
	std::cout << std::endl;

	result = runGLFW();
	delete testCharacter;
	return result;
}

int runGLFW()
{
	if (!glfwInit())
	{
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);

	GLFWwindow* window = glfwCreateWindow(width, height, "FGPrototype", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
	glfwSwapInterval(1);

	initGraphics();

	glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	
	while (!glfwWindowShouldClose(window))
	{
		double time = glfwGetTime();
		glfwPollEvents();

		inputSys.Update(window, entities);
		actionSys.Advance(entities);
		graphicsSys.Render(entities);

		glfwSwapBuffers(window);
	}

	glfwTerminate();

	return 0;
}

int initGraphics()
{
	// Compile shaders and load graphics data
	std::vector<gfx::Shader *> shaders;
	shaders.push_back(new gfx::Shader(ShaderDir "Animate.vert"));
	shaders.push_back(new gfx::Shader(ShaderDir "Animate.frag"));
	graphicsSys.CharProgram = new gfx::ShaderProgram(shaders);
	for (std::size_t i = 0; i < shaders.size(); i++)
		delete shaders[i];
	shaders.clear();

	gfx::ModelImporter imp;
	graphicsSys.Animations.resize(1);
	std::vector<gfx::Mesh *> meshes;
	imp.ImportModel(ModelDir "dummy.dae", meshes);
	gfx::Mesh *dummy = meshes.front();
	graphicsSys.Models.push_back(dummy);
	lua_getglobal(L, "actions");
	lua_len(L, -1);
	int num_actions = luaL_checkinteger(L, -1);
	lua_pop(L, 1);
	for (int i = 0; i < num_actions; i++)
	{
		lua_pushnumber(L, i + 1);
		lua_gettable(L, -2);
		lua_getfield(L, -1, "anim");
		const char *anim = luaL_checkstring(L, -1);
		lua_pop(L, 1);

		graphicsSys.Animations[0].push_back(imp.LoadAnimation(anim, dummy));
		lua_pop(L, 1);
	}
	lua_pop(L, 1);

	return 0;
}

int initActions()
{
	CmdTranslator tl;
	Unit *testCharacter = new Unit();
	testCharacter->CharID = 0;
	std::map<std::string, int> groups;
	lua_getglobal(L, "actions");
	lua_len(L, -1);
	int num_actions = luaL_checkinteger(L, -1);
	lua_pop(L, 1);
	for (int i = 0; i < num_actions; i++)
	{
		lua_rawgeti(L, -1, i + 1);
		lua_getfield(L, -1, "length");
		int length = luaL_checkinteger(L, -1);
		lua_pop(L, 1);
		Action act(i, length);
		lua_getfield(L, -1, "name");
		const char *name = luaL_checkstring(L, -1);
		lua_pop(L, 1);
		act.Name = name;
		if (act.Name == "Neutral")
			testCharacter->NeutralAction = i;

		lua_getfield(L, -1, "commands");
		lua_len(L, -1);
		int num_commands = luaL_checkinteger(L, -1);
		lua_pop(L, 1);
		for (int j = 0; j < num_commands; j++)
		{
			lua_pushnumber(L, j + 1);
			lua_gettable(L, -2);
			const char *cmd_str = luaL_checkstring(L, -1);
			act.Commands.push_back(tl.Translate(cmd_str));
			lua_pop(L, 1);
		}
		lua_pop(L, 1);

		lua_getfield(L, -1, "groups");
		lua_len(L, -1);
		int num_groups = luaL_checkinteger(L, -1);
		lua_pop(L, 1);
		for (int j = 0; j < num_groups; j++)
		{
			lua_pushnumber(L, j + 1);
			lua_gettable(L, -2);
			const char *grp_str = luaL_checkstring(L, -1);
			if (groups.find(grp_str) == groups.end())
			{
				int g = groups.size();
				groups[grp_str] = g;
				testCharacter->Groups.resize(g + 1);
				testCharacter->Groups[g].push_back(i);
			}
			else
			{
				int g = groups[grp_str];
				testCharacter->Groups[g].push_back(i);
			}
			lua_pop(L, 1);
		}
		lua_pop(L, 1);

		testCharacter->AddMove(act);
		lua_pop(L, 1);
	}
	lua_pop(L, 1);
	actionSys.Characters.push_back(testCharacter);
	actionSys.L = L;

	lua_createtable(L, 0, groups.size());
	for (std::map<std::string, int>::iterator it = groups.begin();
		it != groups.end(); ++it)
	{
		lua_pushnumber(L, it->second);
		lua_setfield(L, -2, it->first.c_str());
	}
	lua_setglobal(L, "groups");

	return 0;
}

int initEntity()
{
	// Put a character at index 0 of entity list
	entities.enabled[0].set(IPlacement);
	entities.enabled[0].set(IAction);
	entities.enabled[0].set(IInput);
	entities.enabled[0].set(IVisible);
	entities.placement[0].X = 0;
	entities.placement[0].Y = 0;
	entities.placement[0].O = 1;
	entities.action[0].Character = 0;
	entities.action[0].Current = 0;
	entities.action[0].Time = 0;
	entities.visible[0].Model = 0;

	return 0;
}
