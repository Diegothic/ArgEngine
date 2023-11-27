#pragma once

#include <arg_pch.hpp>

#include "Content/ResourceCache.hpp"
#include "Gameplay/GameWorld.hpp"

namespace Arg
{
	class GameEngine
	{
	public:
		GameEngine() {};
		GameEngine(const GameEngine&) = delete;
		~GameEngine() {};

		void StartUp() {};
		void Update() {};
		void Render() {};
		void ShutDown() {};

		void LoadWorld() {};

	private:
		Content::ResourceCache m_ResourceCache;
		std::shared_ptr<Gameplay::GameWorld> m_pGameWorld = nullptr;
	};
}
