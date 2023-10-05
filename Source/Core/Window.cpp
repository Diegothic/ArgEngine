#include "Window.h"

#include <iostream>
#include <GLFW/glfw3.h>

std::map<GLFWwindow*, Arg::Window*> Arg::Window::s_WindowRegistry;

Arg::Window::Window(const WindowSpec& spec)
	: m_pWindowHandle(nullptr),
	m_KeyboardState()
{
	m_Title = spec.title;
	m_Size = glm::uvec2(spec.width, spec.height);
}

bool Arg::Window::Create()
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

	s_WindowRegistry[m_pWindowHandle] = this;

	glfwMakeContextCurrent(m_pWindowHandle);
	glfwSwapInterval(1);

	// TODO: Move to renderer
	glViewport(
		0,
		0,
		static_cast<int>(GetWidth()),
		static_cast<int>(GetHeight())
	);

	glfwSetFramebufferSizeCallback(m_pWindowHandle,
		Window::WindowResizeCallback
	);
	glfwSetKeyCallback(m_pWindowHandle,
		Window::InputKeyCallback
	);

	VOnCreate();

	return true;
}

void Arg::Window::Update()
{
	glfwPollEvents();

	// TODO: Remove keyboard test
	if (m_KeyboardState.IsKeyPressed(KeyCode::A))
	{
		std::cout << "A Pressed" << std::endl;
	}

	if (m_KeyboardState.IsKeyPressed(KeyCode::A, KeyMods::Ctrl))
	{
		std::cout << "A Pressed with Ctrl" << std::endl;
	}

	if (m_KeyboardState.IsKeyPressed(KeyCode::A, KeyMods::Ctrl | KeyMods::Shift))
	{
		std::cout << "A Pressed with Ctrl and Shift" << std::endl;
	}

	if (m_KeyboardState.IsKeyDown(KeyCode::A))
	{
		std::cout << "A Down" << std::endl;
	}

	if (m_KeyboardState.IsKeyReleased(KeyCode::A))
	{
		std::cout << "A Released" << std::endl;
	}

	if (m_KeyboardState.IsKeyPressed(KeyCode::S, KeyMods::Ctrl))
	{
		std::cout << "S Pressed with Ctrl" << std::endl;
	}

	// TODO: Move to renderer
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT
		| GL_DEPTH_BUFFER_BIT
		| GL_STENCIL_BUFFER_BIT
	);

	VOnUpdate();

	glfwSwapBuffers(m_pWindowHandle);

	m_KeyboardState.Update();
}

void Arg::Window::Destroy()
{
	VOnDestroy();

	glfwDestroyWindow(m_pWindowHandle);

	s_WindowRegistry.erase(m_pWindowHandle);
}

bool Arg::Window::ShouldClose() const
{
	return glfwWindowShouldClose(m_pWindowHandle);
}

void Arg::Window::OnResized(int newWidth, int newHeight)
{
	m_Size.x = newWidth;
	m_Size.y = newHeight;

	// TODO: Move to renderer
	glViewport(
		0,
		0,
		static_cast<int>(GetWidth()),
		static_cast<int>(GetHeight())
	);

	VOnResized();
}

void Arg::Window::OnKeyPressed(int key, int mods)
{
	m_KeyboardState.OnKeyPressed(key);
	m_KeyboardState.SetMods(mods);
}

void Arg::Window::OnKeyReleased(int key)
{
	m_KeyboardState.OnKeyReleased(key);
}

void Arg::Window::WindowResizeCallback(GLFWwindow* windowHandle,
	int newWidth,
	int newHeight
)
{
	if (!s_WindowRegistry.contains(windowHandle))
	{
		return;
	}

	Window* window = s_WindowRegistry[windowHandle];
	window->OnResized(newWidth, newHeight);
}

void Arg::Window::InputKeyCallback(GLFWwindow* windowHandle,
	int key,
	int scanCode,
	int action,
	int mods
)
{
	if (!s_WindowRegistry.contains(windowHandle))
	{
		return;
	}

	Window* window = s_WindowRegistry[windowHandle];
	switch (action)
	{
	case GLFW_PRESS:
		window->OnKeyPressed(key, mods);
		break;
	case GLFW_RELEASE:
		window->OnKeyReleased(key);
		break;
	default:
		break;
	}
}
