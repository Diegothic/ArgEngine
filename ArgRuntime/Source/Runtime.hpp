#pragma once

#include <arg_pch.hpp>

#include "GameEngine.hpp"
#include "Content/Content.hpp"
#include "Core/Window/Window.hpp"

namespace Arg
{
	namespace Runtime
	{

		struct RuntimeSpec
		{
			std::filesystem::path ConfigPath;
			std::shared_ptr<Window> pWindow;
		};
		
		class Runtime
		{
		public:
			Runtime(const RuntimeSpec& spec);

			void Initialize();
			void CleanUp();

			void InitPlay();
			void BeginPlay();
			void Update(float deltaTime);
			void Render();

		private:
			std::filesystem::path m_ConfigPath;
			EngineConfig m_Config;

			std::shared_ptr<Window> m_pWindow = nullptr;
			std::shared_ptr<GameEngine> m_pGameEngine = nullptr;
			std::shared_ptr<Content::ResourceCache> m_pResourceCache = nullptr;
			std::unique_ptr<Content::Content> m_pContent = nullptr;
			std::shared_ptr<Renderer::Renderer> m_pRenderer = nullptr;
		};
	}
}
