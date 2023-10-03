#include "Window.h"

#include <iostream>
#include <GLFW/glfw3.h>

namespace Arg
{
	Window* Window::s_CurrentWindow = nullptr;

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

		s_CurrentWindow = this;

		glfwMakeContextCurrent(m_pWindowHandle);
		glfwSwapInterval(1);

		// TODO: Move to renderer
		glViewport(0, 0, static_cast<int>(GetWidth()), static_cast<int>(GetHeight()));

		glfwSetFramebufferSizeCallback(m_pWindowHandle, Window::WindowResizeCallback);

		VOnCreate();

		return true;
	}

	void Window::Update()
	{
		glfwPollEvents();
		// TODO: Update keyboard state

		// TODO: Move to renderer
		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		VOnUpdate();

		glfwSwapBuffers(m_pWindowHandle);
	}

	void Window::Destroy()
	{
		VOnDestroy();

		glfwDestroyWindow(m_pWindowHandle);

		s_CurrentWindow = nullptr;
	}

	bool Window::ShouldClose() const
	{
		return glfwWindowShouldClose(m_pWindowHandle);
	}

	void Window::OnResized(int newWidth, int newHeight)
	{
		m_Size.x = newWidth;
		m_Size.y = newHeight;

		// TODO: Move to renderer
		glViewport(0, 0, static_cast<int>(GetWidth()), static_cast<int>(GetHeight()));

		VOnResized();
	}

	void Window::WindowResizeCallback(GLFWwindow* windowHandle, int newWidth, int newHeight)
	{
		if (s_CurrentWindow == nullptr)
		{
			return;
		}

		s_CurrentWindow->OnResized(newWidth, newHeight);
	}
}
