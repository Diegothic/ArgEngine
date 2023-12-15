#pragma once

#include <arg_pch.hpp>

#include "Core/Math/Math.hpp"
#include "Content/Resource/Resource.hpp"

namespace Arg
{
	namespace Editor
	{
		namespace GUI
		{
			void ResourceHandleProperty(
				const char* ID,
				Vec2 size,
				const char* resourceName,
				const std::function<void(GUID)>& onResourceDropped,
				const std::function<void()>& onResourceCleared
			);
		}
	}
}
