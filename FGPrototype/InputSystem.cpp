#include "InputSystem.h"

void InputSystem::Update(GLFWwindow* window, EntityList& entities)
{
	for (int i = 0; i < num_entities; i++)
	{
		if (entities.enabled[i][IInput])
		{
			FrameInput f = GetFrameInput(window);
			entities.input[i].Inputs.AddFrame(f);
		}
	}
}

bool InputSystem::InputPressed(GLFWwindow* window, int input)
{
	if (input >= 0)
	{
		return glfwGetKey(window, input) == GLFW_PRESS;
	}
	return false;
}

FrameInput InputSystem::GetFrameInput(GLFWwindow* window)
{
	int direction = 5;
	Buttons b = None;

	if (InputPressed(window, GLFW_KEY_A))
	{
		direction -= 1;
	}
	if (InputPressed(window, GLFW_KEY_D))
	{
		direction += 1;
	}
	if (InputPressed(window, GLFW_KEY_S))
	{
		direction -= 3;
	}
	if (InputPressed(window, GLFW_KEY_W))
	{
		direction += 3;
	}

	if (InputPressed(window, GLFW_KEY_U))
	{
		b = (Buttons)(b | B1);
	}
	if (InputPressed(window, GLFW_KEY_I))
	{
		b = (Buttons)(b | B2);
	}
	if (InputPressed(window, GLFW_KEY_O))
	{
		b = (Buttons)(b | B3);
	}
	if (InputPressed(window, GLFW_KEY_J))
	{
		b = (Buttons)(b | B4);
	}
	if (InputPressed(window, GLFW_KEY_K))
	{
		b = (Buttons)(b | B5);
	}
	if (InputPressed(window, GLFW_KEY_L))
	{
		b = (Buttons)(b | B6);
	}

	return FrameInput(direction, b);
}
