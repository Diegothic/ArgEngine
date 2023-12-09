#pragma once

#include <arg_pch.hpp>

#include "Content/ResourceCache.hpp"
#include "World/Actor/Component/ComponentRegistry.hpp"

namespace Arg
{
	namespace Gameplay
	{
		struct GameContext
		{
			Content::ResourceCache* Resources;
			ComponentRegistry* Components;
		};
	}
}
