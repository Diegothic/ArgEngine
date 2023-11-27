#pragma once

#include <arg_pch.hpp>

#include "Actor.hpp"

namespace Arg
{
	namespace Gameplay
	{
		class GameWorld
		{

		//	auto GetActor(const GUID actorID) -> ActorHandle;

		//	auto GetActorCount() -> const int64_t& {};
		//	auto GetActor(const int64_t) -> const Actor& {};

			void BeginPlay() {};
			void EndPlay() {};

		private:
			int64_t m_ActorCount;
			std::vector<Actor> m_Actors;
		};
	}
}
