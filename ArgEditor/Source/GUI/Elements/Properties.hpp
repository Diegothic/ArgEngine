#pragma once

#include <arg_pch.hpp>

#include "Core/Math/Math.hpp"
#include "Gameplay/World/Actor/ActorHandle.hpp"

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

			void ActorHandleProperty(
				const char* ID,
				Vec2 size,
				const char* actorName,
				const std::function<void(GUID)>& onActorDropped,
				const std::function<void()>& onActorCleared
			);

			void ActorComponentHandleProperty(
				const char* ID,
				Vec2 size,
				const char* actorComponentName,
				const std::function<void(GUID)>& onComponentDropped,
				const std::function<void()>& onComponentCleared
			);
		}
	}
}
