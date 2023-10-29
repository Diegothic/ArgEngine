#pragma once

#include <GLFW/glfw3.h>

namespace Arg
{
	enum GamepadAxis
	{
		LeftHorizontal = GLFW_GAMEPAD_AXIS_LEFT_X,
		LeftVertical = GLFW_GAMEPAD_AXIS_LEFT_Y,
		RightHorizontal = GLFW_GAMEPAD_AXIS_RIGHT_X,
		RightVertical = GLFW_GAMEPAD_AXIS_RIGHT_Y,
		LeftTrigger = GLFW_GAMEPAD_AXIS_LEFT_TRIGGER,
		RightTrigger = GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER,
	};

	enum GamepadAxis2D
	{
		Left,
		Right,
	};
}
