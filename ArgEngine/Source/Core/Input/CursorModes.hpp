#pragma once

#include <arg_pch.hpp>

namespace Arg
{
	namespace Input
	{
		enum CursorMode
		{
			Locked = 0x00034003,
			Hidden = 0x00034002,
			Normal = 0x00034001,
		};
	}
}
