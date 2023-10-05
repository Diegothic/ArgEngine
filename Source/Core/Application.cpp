#include "Application.h"

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Window.h"

void Arg::StartUpCore()
{
	glfwInit();
}

void Arg::ShutDownCore()
{
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
		std::cout << "Error: Failed to create a window!" << std::endl;
		return nullptr;
	}

	const int gladLoaderState = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	if (!gladLoaderState)
	{
		std::cout << "Error: Failed to initialize GLAD!" << std::endl;
		return nullptr;
	}

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
	while (!m_pWindow->ShouldClose())
	{
		m_pWindow->Update();
	}

	m_pWindow->Destroy();
}
