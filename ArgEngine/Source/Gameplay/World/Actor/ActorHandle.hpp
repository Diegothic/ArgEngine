#pragma once

#include <arg_pch.hpp>

#include <Core/GUID.hpp>

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
			ActorHandle(GameWorld* pWorld, GUID actorID);
			ActorHandle(const ActorHandle& other);
			~ActorHandle() = default;

			auto operator=(const ActorHandle& other) -> ActorHandle&;
			auto operator==(const ActorHandle& other) const -> bool;

			auto GetActorID() const -> const GUID& { return m_ActorID; }
			auto IsValid() const -> bool;
			auto Get() const -> Actor&;

		private:
			GameWorld* m_pWorld = nullptr;
			GUID m_ActorID = GUID::Empty;
		};
	}
}
