#include "Window.h"

#include <iostream>

namespace Arg
{
	Window::Window(const WindowSpec& spec)
		: m_pWindowHandle(nullptr)
	{
		m_Title = spec.title;
		m_Size = glm::uvec2(spec.width, spec.height);
	}

	bool Window::Create()
	{
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

		m_pWindowHandle = glfwCreateWindow(
			static_cast<int>(m_Size.x),
			static_cast<int>(m_Size.y),
			m_Title.c_str(),
			nullptr,
			nullptr
		);
		if (m_pWindowHandle == nullptr)
		{
			return false;
		}

		glfwMakeContextCurrent(m_pWindowHandle);

		return true;
	}

	void Window::StartUp()
	{
		glfwSwapInterval(1);
	}

	void Window::Update() const
	{
		glfwPollEvents();

		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		glfwSwapBuffers(m_pWindowHandle);
	}

	void Window::ShutDown()
	{
		glfwDestroyWindow(m_pWindowHandle);
	}

	bool Window::ShouldClose() const
	{
		return glfwWindowShouldClose(m_pWindowHandle);
	}
}
