#include <arg_pch.hpp>
#include "RuntimeApplication.hpp"

#include "Debug/Log.hpp"
#include "Renderer/Renderer.hpp"

auto Arg::Client_CreateApplication() -> std::unique_ptr<Arg::Application>
{
	ARG_CONSOLE_LOG("Creating Runtime");
	std::unique_ptr<Application> app(new RuntimeApplication());
	return app;
}

void Arg::RuntimeApplication::VOnStartUp()
{
	const Arg::WindowSpec windowSpec{
		.Title = "Game",
		.Width = 1920,
		.Height = 1080,
		.Mode = WindowMode::Fullscreen,
		.bIsVSync = true,
	};
	m_pWindow = std::make_shared<Window>(windowSpec);
	m_pWindow->Create();

	const Runtime::RuntimeSpec runtimeSpec{
		.ConfigPath = "Config\\Game.aconfig",
		.pWindow = m_pWindow
	};
	m_pRuntime = std::make_unique<Runtime::Runtime>(runtimeSpec);
	m_pRuntime->Initialize();
}

void Arg::RuntimeApplication::VOnShutDown()
{
	m_pWindow->Destroy();
}

void Arg::RuntimeApplication::VOnRun()
{
	float deltaTime = 1.0f / 30.0f;
	m_pRuntime->InitPlay();
	m_pRuntime->BeginPlay();
	while (IsRunning() && !m_pWindow->ShouldClose())
	{
		const float frameStartTime = m_pWindow->GetElapsedTime();
		m_pWindow->Update();
		const auto& keyboardState = m_pWindow->GetInput().GetKeyboardState();
		if (keyboardState.IsKeyPressed(Input::KeyCode::F4, Input::KeyMods::Alt))
		{
			Close();
		}

		m_pRuntime->Update(deltaTime);
		m_pRuntime->Render();

		m_pWindow->SwapBuffers();

		const float frameEndTime = m_pWindow->GetElapsedTime();
		deltaTime = frameEndTime - frameStartTime;
		if (deltaTime > 1.0f / 30.0f)
		{
			deltaTime = 1.0f / 30.0f;
		}
	}
}
