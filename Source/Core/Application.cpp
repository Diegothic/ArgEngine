#include "Application.h"

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Arg/Debug.h"
#include "Window.h"

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

Arg::Application* Arg::CreateApplication()
{
	const Arg::WindowSpec windowSpec{
		.title = "ArgEngine",
		.width = 1920,
		.height = 1080,
	};
	Arg::Window* window = new Arg::Window(windowSpec);
	const bool windowCreated = window->Create();
	if (!windowCreated) {
		delete window;
		AE_CORE_LOG_ERR("Failed to create a window!");
		return nullptr;
	}

	AE_CORE_LOG_INFO("Initializing OpenGL");
	const int gladLoaderState = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	if (!gladLoaderState)
	{
		AE_CORE_LOG_ERR("Failed to initialize GLAD!");
		return nullptr;
	}

	// TODO: Move to renderer
	const char* graphicsVersion = (const char*)glGetString(GL_VERSION);
	const char* graphicsSLVersion = (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION);
	AE_CORE_LOG_INFO("Using OpenGL version:\n\t%s", graphicsVersion);
	AE_CORE_LOG_INFO("Using GLSL version:\n\t%s", graphicsSLVersion);

	const char* graphicsVendorName = (const char*)glGetString(GL_VENDOR);
	const char* graphicsRendererName = (const char*)glGetString(GL_RENDERER);
	AE_CORE_LOG_INFO("Detected graphics device:\n\t%s: %s",
		graphicsVendorName,
		graphicsRendererName
	);

	return new Arg::Application(window);
}

Arg::Application::Application(Window* window)
{
	m_pWindow = window;
}

Arg::Application::~Application()
{
	delete m_pWindow;
}

void Arg::Application::Run() const
{
	AE_CORE_LOG_INFO("Running the application");
	while (!m_pWindow->ShouldClose())
	{
		m_pWindow->Update();
	}

	m_pWindow->Destroy();
}
