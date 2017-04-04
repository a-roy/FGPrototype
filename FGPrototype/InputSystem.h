#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Component.h"
#include "FrameInput.h"

class InputSystem
{
public:
	void Update(GLFWwindow* window, EntityList& entities);
	bool InputPressed(GLFWwindow* window, int input);
	FrameInput GetFrameInput(GLFWwindow* window);
};
