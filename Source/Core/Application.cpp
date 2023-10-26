#include "Application.h"

#include <GLFW/glfw3.h>

#include "Window.h"
#include "Debug/CoreLogger.h"
#include "Memory/SmartPtr.h"

void Arg::StartUpCore()
{
	AE_CORE_LOG_INFO("Starting up Core");

	glfwInit();

	AE_CORE_LOG_INFO("Using GLFW version\n\t%s", glfwGetVersionString());
	GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
	AE_CORE_LOG_INFO("Detected primary monitor:\n\t%s",
		glfwGetMonitorName(primaryMonitor)
	);
	int monitorPosX, monitorPosY, monitorWidth, monitorHeight;
	glfwGetMonitorWorkarea(primaryMonitor,
		&monitorPosX,
		&monitorPosY,
		&monitorWidth,
		&monitorHeight
	);
	AE_CORE_LOG_INFO("Detected primary monitor Workspace:\n\tPosition: %d %d\n\tSize: %d %d",
		monitorPosX,
		monitorPosY,
		monitorWidth,
		monitorHeight
	);
}

void Arg::ShutDownCore()
{
	AE_CORE_LOG_INFO("Shuting down Core");
	glfwTerminate();
}

Arg::Box<Arg::Application> Arg::CreateApplication()
{
	const Arg::Rc<Arg::Window> window = Arg::Client_CreateWindow();
	const bool windowCreated = window->Create();
	if (!windowCreated) {
		AE_CORE_LOG_ERR("Failed to create a window!");
		return nullptr;
	}

	return NewBox<Arg::Application>(window);
}

Arg::Application::Application(const Rc<Window>& window)
{
	m_pWindow = window;
}

void Arg::Application::Run() const
{
	AE_CORE_LOG_INFO("Running the application");
	m_pWindow->Start();
	while (!m_pWindow->ShouldClose())
	{
		m_pWindow->Update();
	}

	m_pWindow->Destroy();
}
