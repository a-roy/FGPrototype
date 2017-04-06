#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include "imgui_impl_glfw_gl3.h"
#include "Component.h"
#include "InputSystem.h"
#include "ActionSystem.h"
#include "GraphicsSystem.h"
#include "CmdTranslator.h"
#include "ModelImporter.h"
#include "LuaEntity.h"
#include "Hitbox.h"
#include "Buffer.h"
#include "VertexArray.h"
#include <iostream>

#define ShaderDir "../FGPrototype/shaders/"
#define ModelDir "../FGPrototype/models/"
#define ScriptDir "../FGPrototype/scripts/"

using namespace std;

int runGLFW();
int initGraphics();
int initActions();
int initEntity();

int width = 1600, height = 900;
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

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
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

	ImGui_ImplGlfwGL3_Init(window, true);

	initGraphics();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	GLuint FramebufferName = 0;
	glGenFramebuffers(1, &FramebufferName);
	glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
	// The texture we're going to render to
	GLuint renderedTexture;
	glGenTextures(1, &renderedTexture);

	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, renderedTexture);

	// Give an empty image to OpenGL ( the last "0" )
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1024, 576, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

	// Poor filtering. Needed !
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	// The depth buffer
	GLuint depthrenderbuffer;
	glGenRenderbuffers(1, &depthrenderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 1024, 576);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);
	// Set "renderedTexture" as our colour attachement #0
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedTexture, 0);

	// Set the list of draw buffers.
	GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers
								   // Render to our framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glEnable(GL_DEPTH_TEST);
	
	int num_actions = actionSys.Characters[0]->MoveList.size();
	int current_time = 0;
	int current_action = 0;
	int current_type = 1;
	int current_hitbox = 0;
	std::vector<std::vector<Hitbox> > hitboxes(num_actions);
	bool strike, gndthrow, airthrow, projectile, collision;
	int h_x = 0, h_w = 100, h_y = 0, h_h = 100, h_t = 0, h_l = 1;
	gfx::VertexArray h_vao;
	h_vao.Bind();
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	h_vao.Unbind();
	gfx::FloatBuffer vbo1(&h_vao, 2, 0);
	vbo1.Usage = GL_STREAM_DRAW;
	gfx::FloatBuffer vbo2(&h_vao, 4, 0);
	vbo2.Usage = GL_STREAM_DRAW;
	std::vector<GLfloat> coords;
	std::vector<GLfloat> colors;
	while (!glfwWindowShouldClose(window))
	{
		double time = glfwGetTime();
		glfwPollEvents();
		ImGui_ImplGlfwGL3_NewFrame();

		inputSys.Update(window, entities);
		actionSys.Advance(entities);
		graphicsSys.Render(entities);

		glm::mat4 sMatrix = glm::scale(glm::vec3(1.5f / 250.f));
		glm::mat4 tMatrix = glm::translate(glm::vec3(-3.f, -2.f, 0.f));
		glm::mat4 vMatrix = glm::lookAt(
			glm::vec3(0.f, 15, 20),
			glm::vec3(0, 5, 0),
			glm::vec3(0, 1, 0));
		glm::mat4 pMatrix = glm::perspective(
			glm::radians(45.0f), 16.0f / 9.0f, 1.f, 100.0f);
		glm::mat4 mvpMatrix = pMatrix * vMatrix * tMatrix * sMatrix;
		graphicsSys.HitboxProgram->GetUniform("uMVPMatrix").Set(mvpMatrix);
		for (std::size_t i = 0; i < hitboxes[current_action].size(); i++)
		{
			Hitbox& h = hitboxes[current_action][i];
			if (current_time >= h.Time && current_time < h.Time + h.Length)
			{
				coords.push_back(h.X);
				coords.push_back(h.Y);
				coords.push_back(h.X);
				coords.push_back(h.Y + h.Height);
				coords.push_back(h.X + h.Width);
				coords.push_back(h.Y);
				coords.push_back(h.X + h.Width);
				coords.push_back(h.Y);
				coords.push_back(h.X);
				coords.push_back(h.Y + h.Height);
				coords.push_back(h.X + h.Width);
				coords.push_back(h.Y + h.Height);
				GLfloat r, g, b, a;
				if ((h.Flags & (Act | React)) == (Act | React))
				{
					r = 0.6f;
					g = 0.6f;
					b = 0.6f;
					a = 0.6f;
				}
				else if (h.Flags & Act)
				{
					r = 0.6f;
					g = 0.0f;
					b = 0.0f;
					a = 0.6f;
				}
				else
				{
					r = 0.6f;
					g = 0.6f;
					b = 0.0f;
					a = 0.6f;
				}
				for (int j = 0; j < 6; j++)
				{
					colors.push_back(r);
					colors.push_back(g);
					colors.push_back(b);
					colors.push_back(a);
				}
			}
		}
		vbo1.NumItems = coords.size() / 2;
		vbo1.Data = coords.data();
		vbo1.SetData(coords.data(), false);
		vbo1.VertexAttribPointer(0);
		vbo2.NumItems = colors.size() / 4;
		vbo2.Data = colors.data();
		vbo2.SetData(colors.data(), false);
		vbo2.VertexAttribPointer(1);

		glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
		glViewport(0, 0, 1024, 576); // Render on the whole framebuffer
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		graphicsSys.RenderModel(0, current_action, current_time, 1, -3.f, -2.f, 1.5f);
		h_vao.Bind();
		glDisable(GL_DEPTH_TEST);
		graphicsSys.HitboxProgram->Install();
		glDrawArrays(GL_TRIANGLES, 0, vbo1.NumItems);
		graphicsSys.HitboxProgram->Uninstall();
		h_vao.Unbind();
		glEnable(GL_DEPTH_TEST);
		coords.clear();
		colors.clear();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);

		ImGui::Begin("Visual");
		ImGui::Image((void *)renderedTexture, ImVec2(1024, 576), ImVec2(0, 1), ImVec2(1, 0));
		ImGui::End();

		ImGui::Begin("Timeline");
		int num_frames = actionSys.Characters[0]->MoveList[current_action].Length;
		ImGui::SliderInt("Time", &current_time, 0, num_frames - 1);
		ImDrawList *draw_list = ImGui::GetWindowDrawList();
		ImGui::ListBoxHeader("Hitboxes");
		float total_width = ImGui::GetContentRegionAvailWidth();
		float frame_width = total_width / num_frames;
		const ImU32 gray = ImColor(0.6f, 0.6f, 0.6f);
		const ImU32 yellow = ImColor(0.6f, 0.6f, 0.0f);
		const ImU32 red = ImColor(0.6f, 0.0f, 0.0f);
		for (std::size_t i = 0; i < hitboxes[current_action].size(); i++)
		{
			const ImVec2 p = ImGui::GetCursorScreenPos();
			ImGui::Selectable("", i == current_hitbox);
			if (ImGui::IsItemClicked())
				current_hitbox = i;
			float x = p.x + hitboxes[current_action][i].Time * frame_width;
			float y = p.y;
			float w = hitboxes[current_action][i].Length * frame_width;
			float h = ImGui::GetTextLineHeight();
			ImU32 color;
			if ((hitboxes[current_action][i].Flags & (Act | React)) == (Act | React))
				color = gray;
			else if (hitboxes[current_action][i].Flags & Act)
				color = red;
			else
				color = yellow;
			draw_list->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), color, 4.f);
		}
		ImGui::ListBoxFooter();
		if (ImGui::Button("New hitbox"))
		{
			Hitbox h;
			h.X = 0;
			h.Width = 100;
			h.Y = 0;
			h.Height = 100;
			h.Time = current_time;
			h.Length = 1;
			h.Flags = (HitboxFlags)(React | Strike);
			hitboxes[current_action].push_back(h);
		}
		ImGui::SameLine();
		if (ImGui::Button("Delete hitbox") && current_hitbox < (int)hitboxes[current_action].size())
		{
			hitboxes[current_action].erase(hitboxes[current_action].begin() + current_hitbox);
		}
		ImGui::End();

		ImGui::Begin("Actions");
		std::size_t num_actions = actionSys.Characters[0]->MoveList.size();
		std::vector<const char *> action_list;
		for (std::size_t i = 0; i < num_actions; i++)
		{
			action_list.push_back(actionSys.Characters[0]->MoveList[i].Name.c_str());
		}
		ImGui::Combo("Action", &current_action, action_list.data(), num_actions);
		ImGui::End();

		if (current_hitbox >= 0 && current_hitbox < (int)hitboxes[current_action].size())
		{
			Hitbox& h = hitboxes[current_action][current_hitbox];
			if ((h.Flags & (Act | React)) == (Act | React))
				current_type = 2;
			else if (h.Flags & Act)
				current_type = 0;
			else
				current_type = 1;
			strike = (h.Flags & Strike) == Strike;
			gndthrow = (h.Flags & Throw) == Throw;
			airthrow = (h.Flags & AirThrow) == AirThrow;
			projectile = (h.Flags & Projectile) == Projectile;
			collision = (h.Flags & Collision) == Collision;
			ImGui::Begin("Properties");
			ImGui::InputInt("X", &h.X, 10);
			ImGui::InputInt("Width", &h.Width, 10);
			ImGui::InputInt("Y", &h.Y, 10);
			ImGui::InputInt("Height", &h.Height, 10);
			ImGui::InputInt("Time", &h.Time);
			ImGui::InputInt("Length", &h.Length);
			const char *const types[] = { "Act", "React", "Act & React" };
			ImGui::Combo("Type", &current_type, types, 3);
			ImGui::Checkbox("Strike", &strike);
			ImGui::Checkbox("Throw", &gndthrow);
			ImGui::Checkbox("Airthrow", &airthrow);
			ImGui::Checkbox("Projectile", &projectile);
			ImGui::Checkbox("Collision", &collision);
			ImGui::End();
			h.Flags = (HitboxFlags)0;
			if (current_type == 0 || current_type == 2)
				h.Flags = (HitboxFlags)(h.Flags | Act);
			if (current_type == 1 || current_type == 2)
				h.Flags = (HitboxFlags)(h.Flags | React);
			if (strike)
				h.Flags = (HitboxFlags)(h.Flags | Strike);
			if (gndthrow)
				h.Flags = (HitboxFlags)(h.Flags | Throw);
			if (airthrow)
				h.Flags = (HitboxFlags)(h.Flags | AirThrow);
			if (projectile)
				h.Flags = (HitboxFlags)(h.Flags | Projectile);
			if (collision)
				h.Flags = (HitboxFlags)(h.Flags | Collision);

			h.Width = std::max(h.Width, 0);
			h.Height = std::max(h.Height, 0);
			h.Time = std::max(h.Time, 0);
			h.Length = std::max(h.Length, 1);
			int max_time = num_frames - 1;
			h.Time = std::min(h.Time, max_time);
			int max_length = num_frames - h.Time;
			h.Length = std::min(h.Length, max_length);
		}

		ImGui::Render();
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
	shaders.push_back(new gfx::Shader(ShaderDir "Hitbox.vert"));
	shaders.push_back(new gfx::Shader(ShaderDir "Hitbox.frag"));
	graphicsSys.HitboxProgram = new gfx::ShaderProgram(shaders);
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
