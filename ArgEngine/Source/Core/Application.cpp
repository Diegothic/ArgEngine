#include <arg_pch.hpp>
#include "Application.hpp"

#include "Debug/Log.hpp"
#include "Window/Window.hpp"

void Arg::Application::StartUp()
{
	ARG_CONSOLE_LOG("Starting up Core");
	Arg::WindowSystem::StartUp();
	VOnStartUp();
}

void Arg::Application::Shutdown()
{
	ARG_CONSOLE_LOG("Shuting down Core");
	VOnShutDown();
	Arg::WindowSystem::Shutdown();
}

void Arg::Application::Run()
{
	ARG_CONSOLE_LOG("Running the application");
	m_bIsRunning = true;
	VOnRun();
}

void Arg::Application::Close()
{
	m_bIsRunning = false;
}
