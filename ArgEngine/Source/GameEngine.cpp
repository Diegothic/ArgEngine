#include <arg_pch.hpp>
#include "GameEngine.hpp"

#include "Gameplay/GameContext.hpp"

auto Arg::EngineConfig::VOnSerialize(YAML::Node& node) const -> bool
{
	auto windowHeader = node["Window"];
	windowHeader["Width"] = WindowWidth;
	windowHeader["Height"] = WindowHeight;
	windowHeader["IsFullscreen"] = bIsFullscreen;
	windowHeader["IsVSync"] = bIsWindowVSync;
	node["Window"] = windowHeader;

	auto gameHeader = node["Game"];
	gameHeader["Map"] = StartingMap;
	node["Game"] = gameHeader;

	return true;
}

auto Arg::EngineConfig::VOnDeserialize(const YAML::Node& node) -> bool
{
	const auto windowHeader = node["Window"];
	if (windowHeader)
	{
		WindowWidth = ValueOr<int32_t>(windowHeader["Width"], 1920);
		WindowHeight = ValueOr<int32_t>(windowHeader["Height"], 1080);
		bIsFullscreen = ValueOr<bool>(windowHeader["IsFullscreen"], true);
		bIsWindowVSync = ValueOr<bool>(windowHeader["IsVSync"], true);
	}

	const auto gameHeader = node["Game"];
	if (windowHeader)
	{
		StartingMap = ValueOr<std::string>(gameHeader["Map"], "");
	}

	return true;
}

void Arg::GameEngine::Initialize(
	Window* pWindow,
	const std::filesystem::path& rootDirectory,
	const std::shared_ptr<Content::ResourceCache>& pResourceCache
)
{
	m_pWindow = pWindow;
	m_GameInput.Initialize(m_pWindow);

	m_pResourceCache = pResourceCache;

	m_ComponentRegistry.RegisterComponents();
	Gameplay::GameWorld::SetGameEngine(this);

	const Script::ScriptEngineSpec scriptEngineSpec{
		.RootDirectory = rootDirectory / "Scripts",
		.SourceFileName = "Game",
		.ComponentRegistry = &m_ComponentRegistry
	};
	m_pScriptEngine = std::make_unique<Script::ScriptEngine>(scriptEngineSpec);
	m_pScriptEngine->Initialize();

	m_pSoundEngine = std::make_unique<Sound::SoundEngine>();
	m_pSoundEngine->Initialize();
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

	if (m_pSoundEngine != nullptr)
	{
		m_pSoundEngine->CleanUp();
	}

	m_ComponentRegistry.Clear();
	m_pResourceCache = nullptr;
	m_bIsPlaying = false;
	m_GameTime = Gameplay::GameTime();
}

void Arg::GameEngine::RebuildScripts()
{
	ARG_ASSERT(m_pScriptEngine != nullptr);
	m_pScriptEngine->Rebuild();

	m_ComponentRegistry.Clear();
	m_ComponentRegistry.RegisterComponents();

	m_pScriptEngine->Load();
}

void Arg::GameEngine::LoadScripts()
{
	ARG_ASSERT(m_pScriptEngine != nullptr);
	m_pScriptEngine->Load();
}

void Arg::GameEngine::LoadWorld(const std::string& worldName)
{
	LoadWorld(m_pResourceCache->CreateHandle<Content::WorldResource>(worldName));
}

void Arg::GameEngine::LoadWorld(const GUID& worldID)
{
	LoadWorld(m_pResourceCache->CreateHandle<Content::WorldResource>(worldID));
}

void Arg::GameEngine::RequestWorld(const std::string& worldName)
{
	m_bWorldRequested = true;
	m_RequestedWorldHandle = m_pResourceCache->CreateHandle<Content::WorldResource>(worldName);
}

void Arg::GameEngine::InitializeWorld(Gameplay::GameWorld* pWorld)
{
	const Gameplay::GameContext context{
		.Resources = m_pResourceCache.get(),
		.Components = &m_ComponentRegistry
	};
	pWorld->Initialize(context);
}

void Arg::GameEngine::Play()
{
	m_bIsPlaying = false;
	m_bPlayRequested = true;
}

void Arg::GameEngine::Stop()
{
	m_bIsPlaying = false;
	m_bPlayRequested = false;

	if (IsWorldLoaded())
	{
		m_pLoadedWorld->EndPlay();
	}

	if (m_pSoundEngine != nullptr)
	{
		m_pSoundEngine->StopAllSounds();
		m_pSoundEngine->SetListenerPosition(
			Vec3(0.0f),
			Vec3(1.0f, 0.0f, 0.0f),
			Vec3(0.0f, 0.0f, 1.0f)
		);
	}
}

void Arg::GameEngine::Update(const float& deltaTime, bool bIsFocused)
{
	if (m_bWorldRequested)
	{
		m_bWorldRequested = false;
		if (m_RequestedWorldHandle.IsValid())
		{
			LoadWorld(m_RequestedWorldHandle);
			m_pLoadedWorld->BeginPlay();
			return;
		}
	}

	m_GameTime.Tick(deltaTime);
	m_GameInput.Tick(deltaTime, bIsFocused);

	if (!IsWorldLoaded())
	{
		return;
	}

	if (!m_bIsPlaying && m_bPlayRequested)
	{
		m_bPlayRequested = false;
		m_bIsPlaying = true;
		m_GameInput.BeginPlay();
		m_pLoadedWorld->BeginPlay();
	}

	if (m_bIsPlaying)
	{
		if (m_pSoundEngine != nullptr)
		{
			const auto mainCamera = m_pLoadedWorld->GetMainCamera();
			if (mainCamera.IsValid())
			{
				const Renderer::Camera* pCamera = mainCamera.Get().GetCamera();
				m_pSoundEngine->SetListenerPosition(
					pCamera->GetPosition(),
					pCamera->GetForwardVector(),
					pCamera->GetUpVector()
				);
			}
			else
			{
				m_pSoundEngine->SetListenerPosition(
					Vec3(0.0f),
					Vec3(1.0f, 0.0f, 0.0f),
					Vec3(0.0f, 0.0f, 1.0f)
				);
			}
		}

		m_pLoadedWorld->Tick(m_GameTime, m_GameInput);
	}
}

void Arg::GameEngine::RenderGame(Renderer::RenderContext& context) const
{
	if (IsWorldLoaded())
	{
		const auto mainCamera = m_pLoadedWorld->GetMainCamera();
		if (mainCamera.IsValid())
		{
			context.SetCamera(mainCamera.Get().GetCamera());
		}

		m_pLoadedWorld->Render(context);
	}
}

void Arg::GameEngine::RenderEditor(Renderer::RenderContext& context) const
{
	if (IsWorldLoaded())
	{
		m_pLoadedWorld->Render(context);
	}
}

void Arg::GameEngine::RenderDebug(Renderer::RenderContext& context) const
{
	if (IsWorldLoaded())
	{
		m_pLoadedWorld->DrawDebug(context);
	}
}

void Arg::GameEngine::ClearGarbage()
{
	if (IsWorldLoaded())
	{
		m_pLoadedWorld->ClearGarbage();
	}

	if (m_pSoundEngine != nullptr)
	{
		m_pSoundEngine->ClearGarbage();
	}
}

void Arg::GameEngine::LoadWorld(const WorldHandle& worldHandle)
{
	if (m_pSoundEngine != nullptr)
	{
		m_pSoundEngine->StopAllSounds();
	}
	
	if (IsWorldLoaded())
	{
		if (m_WorldHandle.IsValid())
		{
			m_pLoadedWorld->Destroy();
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
