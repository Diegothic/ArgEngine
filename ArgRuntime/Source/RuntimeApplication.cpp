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
		.Mode = WindowMode::Windowed,
		.bIsVSync = true,
	};
	m_pWindow = std::make_unique<Window>(windowSpec);
	m_pWindow->Create();
}

void Arg::RuntimeApplication::VOnShutDown()
{
	m_pWindow->Destroy();
}

void Arg::RuntimeApplication::VOnRun()
{
	while (IsRunning() && !m_pWindow->ShouldClose())
	{
		m_pWindow->Update();
		const auto& keyboardState = m_pWindow->GetInput().GetKeyboardState();
		const auto& mouseState = m_pWindow->GetInput().GetMouseState();
		const auto& gamepadState = m_pWindow->GetInput().GetGamepadState(0);
		if (keyboardState.IsKeyPressed(Input::KeyCode::F4, Input::KeyMods::Alt))
		{
			Close();
		}

		m_pWindow->SwapBuffers();
	}
}
