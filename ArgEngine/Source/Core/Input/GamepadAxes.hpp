#pragma once

#include <arg_pch.hpp>

namespace Arg
{
	namespace Input
	{
		enum GamepadAxis
		{
			LeftHorizontal = 0,
			LeftVertical = 1,
			RightHorizontal = 2,
			RightVertical = 3,
			LeftTrigger = 4,
			RightTrigger = 5,
		};

		enum GamepadAxis2D
		{
			AxisLeft,
			AxisRight,
		};
	}
}
