#pragma once

#include <arg_pch.hpp>

namespace Arg
{
	namespace Input
	{
		constexpr int MOUSE_BUTTON_MIN = 0;
		constexpr int MOUSE_BUTTON_MAX = 7;

		enum MouseButton
		{
			Left = 0,
			Right = 1,
			Middle = 2,
		};
	}
}
