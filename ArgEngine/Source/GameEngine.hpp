#pragma once

#include <arg_pch.hpp>

#include "Gameplay/GameTime.hpp"
#include "Content/ResourceCache.hpp"
#include "Gameplay/World/GameWorld.hpp"
#include "Gameplay/World/Actor/Component/ComponentRegistry.hpp"
#include "Content/Resource/GameResources/WorldResource.hpp"
#include "Renderer/RenderContext.hpp"

namespace Arg
{
	using WorldHandle = Content::ResourceHandle<Content::WorldResource>;
	
	class GameEngine
	{
	public:
		GameEngine() = default;
		GameEngine(const GameEngine&) = delete;
		~GameEngine() = default;

		void Initialize(const std::shared_ptr<Content::ResourceCache>& pResourceCache);
		void Deinitialize();

		void LoadWorld(const std::string& worldName);
		void LoadWorld(const GUID& worldID);

		auto IsWorldLoaded() const -> bool { return m_pLoadedWorld != nullptr; }
		auto GetLoadedWorld() -> std::shared_ptr<Gameplay::GameWorld>& { return m_pLoadedWorld; }
		void InitializeWorld(Gameplay::GameWorld* pWorld);

		void Update(const float deltaTime);

		void RenderEditor(Renderer::RenderContext& context) const;

		void ClearGarbage();

	private:
		void LoadWorld(const WorldHandle& worldHandle);

	private:
		Gameplay::GameTime m_GameTime;
		bool m_bIsPlaying = false;

		std::shared_ptr<Content::ResourceCache> m_pResourceCache = nullptr;

		WorldHandle m_WorldHandle;
		std::shared_ptr<Gameplay::GameWorld> m_pLoadedWorld = nullptr;

		Gameplay::ComponentRegistry m_ComponentRegistry;
	};
}
