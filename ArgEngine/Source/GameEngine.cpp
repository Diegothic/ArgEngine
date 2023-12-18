#include <arg_pch.hpp>
#include "GameEngine.hpp"

#include "Gameplay/GameContext.hpp"

void Arg::GameEngine::Initialize(const std::shared_ptr<Content::ResourceCache>& pResourceCache)
{
	m_pResourceCache = pResourceCache;

	m_ComponentRegistry.RegisterComponents();
	Gameplay::GameWorld::SetGameEngine(this);
}

void Arg::GameEngine::Deinitialize()
{
	if (IsWorldLoaded())
	{
		if (m_WorldHandle.IsValid())
		{
			m_WorldHandle.FreeRef();
		}

		m_pLoadedWorld = nullptr;
	}

	m_ComponentRegistry.Clear();
	m_pResourceCache = nullptr;
	m_bIsPlaying = false;
	m_GameTime = Gameplay::GameTime();
}

void Arg::GameEngine::LoadWorld(const std::string& worldName)
{
	LoadWorld(m_pResourceCache->CreateHandle<Content::WorldResource>(worldName));
}

void Arg::GameEngine::LoadWorld(const GUID& worldID)
{
	LoadWorld(m_pResourceCache->CreateHandle<Content::WorldResource>(worldID));
}

void Arg::GameEngine::InitializeWorld(Gameplay::GameWorld* pWorld)
{
	const Gameplay::GameContext context{
		.Resources = m_pResourceCache.get(),
		.Components = &m_ComponentRegistry
	};
	pWorld->Initialize(context);
}

void Arg::GameEngine::Update(const float deltaTime)
{
	m_GameTime.Tick(deltaTime);

	if (IsWorldLoaded() && m_bIsPlaying)
	{
		m_pLoadedWorld->Tick(m_GameTime);
		m_pLoadedWorld->ClearGarbage();
	}
}

void Arg::GameEngine::RenderEditor(Renderer::RenderContext& context) const
{
	if (IsWorldLoaded())
	{
		m_pLoadedWorld->Render(context);
	}
}

void Arg::GameEngine::ClearGarbage()
{
	if (IsWorldLoaded())
	{
		m_pLoadedWorld->ClearGarbage();
	}
}

void Arg::GameEngine::LoadWorld(const WorldHandle& worldHandle)
{
	if (IsWorldLoaded())
	{
		if (m_WorldHandle.IsValid())
		{
			m_WorldHandle.FreeRef();
		}

		m_pLoadedWorld = nullptr;
	}

	m_WorldHandle = worldHandle;
	if (!m_WorldHandle.IsValid())
	{
		ARG_CONSOLE_LOG_ERR("Tried to load world that does not exist!");
		m_pLoadedWorld = nullptr;
		return;
	}

	m_WorldHandle.AddRef();
	m_pLoadedWorld = m_WorldHandle.Get()->GetWorld();
}
