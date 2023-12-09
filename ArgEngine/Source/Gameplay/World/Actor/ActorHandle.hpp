#pragma once

#include <arg_pch.hpp>

#include "Core/GUID.hpp"

namespace Arg
{
	namespace Gameplay
	{
		class GameWorld;
		class Actor;

		class ActorHandle
		{
		public:
			ActorHandle() = default;
			ActorHandle(const GUID ID, GameWorld* world);
			ActorHandle(const ActorHandle& other);
			~ActorHandle() = default;

			auto operator=(const ActorHandle& other)->ActorHandle&;
			auto operator==(const ActorHandle& other) const -> bool;

			auto IsValid() const -> bool;
			auto Get() const->std::shared_ptr<Actor>;

		private:
			GUID m_ActorID = GUID::Empty;
			GameWorld* m_pWorld = nullptr;
		};
	}
}
