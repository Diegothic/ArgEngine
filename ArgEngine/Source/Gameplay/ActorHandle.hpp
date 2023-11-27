#pragma once

#include <arg_pch.hpp>

#include "GameWorld.hpp"

namespace Arg
{
	class ActorHandle
	{
		ActorHandle(const int64_t index, const std::shared_ptr<Gameplay::GameWorld>& m_GameWorld);
		ActorHandle(const ActorHandle& other);
		~ActorHandle() = default;

		//auto GetActor() const -> const Gameplay::Actor&;

	private:
		int64_t m_Index;
		std::shared_ptr<Gameplay::GameWorld> m_GameWorld;
	};
}
