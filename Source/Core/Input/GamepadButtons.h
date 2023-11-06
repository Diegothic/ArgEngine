#pragma once

#include <GLFW/glfw3.h>

namespace Arg
{
	enum GamepadButton
	{
		South = GLFW_GAMEPAD_BUTTON_A,
		East = GLFW_GAMEPAD_BUTTON_B,
		West = GLFW_GAMEPAD_BUTTON_X,
		North = GLFW_GAMEPAD_BUTTON_Y,
		LeftBumper = GLFW_GAMEPAD_BUTTON_LEFT_BUMPER,
		RightBumper = GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER,
		Back = GLFW_GAMEPAD_BUTTON_BACK,
		Start = GLFW_GAMEPAD_BUTTON_START,
		Guide = GLFW_GAMEPAD_BUTTON_GUIDE,
		LeftThumb = GLFW_GAMEPAD_BUTTON_LEFT_THUMB,
		RightThumb = GLFW_GAMEPAD_BUTTON_RIGHT_THUMB,
		DPadUp = GLFW_GAMEPAD_BUTTON_DPAD_UP,
		DPadRight = GLFW_GAMEPAD_BUTTON_DPAD_RIGHT,
		DPadDown = GLFW_GAMEPAD_BUTTON_DPAD_DOWN,
		DPadLeft = GLFW_GAMEPAD_BUTTON_DPAD_LEFT,
	};
}