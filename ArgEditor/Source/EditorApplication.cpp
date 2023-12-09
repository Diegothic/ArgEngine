#include <arg_pch.hpp>
#include "EditorApplication.hpp"

#include "Debug/Log.hpp"

auto Arg::Client_CreateApplication() -> std::unique_ptr<Arg::Application>
{
	ARG_CONSOLE_LOG("Creating Editor");
	std::unique_ptr<Application> app(new EditorApplication());
	return app;
}

auto Arg::EditorApplication::VOnStartUp() -> void
{
	const WindowSpec windowSpec{
		.Title = "ArgEditor",
		.Width = 1920,
		.Height = 1080,
		.Mode = WindowMode::Windowed,
		.bIsVSync = true,
	};
	m_pWindow = std::make_shared<Window>(windowSpec);
	m_pWindow->Create();

	const Editor::EditorSpec editorSpec{
		.ConfigPath = "UserConfig\\Editor.aconfig",
		.pWindow = m_pWindow
	};
	m_pEditor = std::make_unique<Editor::Editor>(editorSpec);
	m_pEditor->Initialize();
}

auto Arg::EditorApplication::VOnShutDown() -> void
{
	m_pEditor->CleanUp();
	m_pWindow->Destroy();
}

auto Arg::EditorApplication::VOnRun() -> void
{
	float deltaTime = 1.0f / 30.0f;
	while (IsRunning() && !m_pWindow->ShouldClose())
	{
		const float frameStartTime = m_pWindow->GetElapsedTime();
		m_pWindow->Update();
		const auto& keyboardState = m_pWindow->GetInput().GetKeyboardState();
		if (keyboardState.IsKeyPressed(Input::KeyCode::F4, Input::KeyMods::Alt))
		{
			Close();
		}

		m_pEditor->Update(deltaTime);
		m_pEditor->Render();

		m_pWindow->SwapBuffers();

		const float frameEndTime = m_pWindow->GetElapsedTime();
		deltaTime = frameEndTime - frameStartTime;
		if (deltaTime > 1.0f / 30.0f)
		{
			deltaTime = 1.0f / 30.0f;
		}
	}
}
