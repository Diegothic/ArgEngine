#pragma once

#include <arg_pch.hpp>

#include "Gameplay/GameTime.hpp"
#include "Content/ResourceCache.hpp"
#include "Gameplay/World/GameWorld.hpp"
#include "Gameplay/World/Actor/Component/ComponentRegistry.hpp"
#include "Content/Resource/GameResources/WorldResource.hpp"
#include "Renderer/RenderContext.hpp"
#include "Script\ScriptEngine.hpp"
#include "Sound/SoundEngine.hpp"

namespace Arg
{
	class Window;

	using WorldHandle = Content::ResourceHandle<Content::WorldResource>;

	struct EngineConfig : public Content::YamlSerializable
	{
		std::string WindowTitle = "Game";
		int32_t WindowWidth = 1920;
		int32_t WindowHeight = 1080;
		bool bIsFullscreen = true;
		bool bIsWindowVSync = true;

		std::string StartingMap;

	protected:
		auto VOnSerialize(YAML::Node& node) const -> bool override;
		auto VOnDeserialize(const YAML::Node& node) -> bool override;
	};

	class GameEngine
	{
	public:
		GameEngine() = default;
		GameEngine(const GameEngine&) = delete;
		~GameEngine() = default;

		void Initialize(
			Window* pWindow,
			const std::filesystem::path& rootDirectory,
			const std::shared_ptr<Content::ResourceCache>& pResourceCache
		);
		void Deinitialize();

		void RebuildScripts();
		void LoadScripts();

		auto GetGameTime() const -> const Gameplay::GameTime& { return m_GameTime; }
		auto GetSoundEngine() const -> Sound::SoundEngine& { return *m_pSoundEngine; }

		void LoadWorld(const std::string& worldName);
		void LoadWorld(const GUID& worldID);

		void RequestWorld(const std::string& worldName);

		auto IsWorldLoaded() const -> bool { return m_pLoadedWorld != nullptr; }
		auto GetLoadedWorld() -> std::shared_ptr<Gameplay::GameWorld>& { return m_pLoadedWorld; }
		void InitializeWorld(Gameplay::GameWorld* pWorld);

		void Play();
		void Stop();
		auto IsPlaying() const -> bool { return m_bIsPlaying; }

		void Update(const float& deltaTime, bool bIsFocused = true);
		void RenderGame(Renderer::RenderContext& context) const;
		void RenderEditor(Renderer::RenderContext& context) const;
		void RenderDebug(Renderer::RenderContext& context) const;

		void ClearGarbage();

	private:
		void LoadWorld(const WorldHandle& worldHandle);

	private:
		Window* m_pWindow = nullptr;

		Gameplay::GameTime m_GameTime;
		Gameplay::GameInput m_GameInput;
		bool m_bIsPlaying = false;
		bool m_bPlayRequested = false;

		std::shared_ptr<Content::ResourceCache> m_pResourceCache = nullptr;
		std::unique_ptr<Script::ScriptEngine> m_pScriptEngine = nullptr;
		std::unique_ptr<Sound::SoundEngine> m_pSoundEngine = nullptr;

		WorldHandle m_WorldHandle;
		std::shared_ptr<Gameplay::GameWorld> m_pLoadedWorld = nullptr;

		bool m_bWorldRequested = false;
		WorldHandle m_RequestedWorldHandle;

		Gameplay::ComponentRegistry m_ComponentRegistry;
	};
}
