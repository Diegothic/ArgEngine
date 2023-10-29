#pragma once

#include <GLFW/glfw3.h>

constexpr int MOUSE_BUTTON_MIN = 0;
constexpr int MOUSE_BUTTON_MAX = GLFW_MOUSE_BUTTON_LAST;

enum MouseButton
{
	Left = GLFW_MOUSE_BUTTON_1,
	Right = GLFW_MOUSE_BUTTON_2,
	Middle = GLFW_MOUSE_BUTTON_3,
};
