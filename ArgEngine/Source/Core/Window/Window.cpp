#include <arg_pch.hpp>
#include "Window.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Debug/Assert.hpp"

auto Arg::WindowSystem::StartUp() -> void
{
	ARG_CONSOLE_LOG("Starting up Window System");
	glfwInit();
	ARG_CONSOLE_LOG("Using GLFW version\n\t%s", glfwGetVersionString());
	GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
	ARG_CONSOLE_LOG("Detected primary monitor:\n\t%s",
		glfwGetMonitorName(primaryMonitor)
	);
	int monitorPosX;
	int monitorPosY;
	int monitorWidth;
	int monitorHeight;
	glfwGetMonitorWorkarea(
		primaryMonitor,
		&monitorPosX,
		&monitorPosY,
		&monitorWidth,
		&monitorHeight
	);
	ARG_CONSOLE_LOG("Detected primary monitor Workspace:\n\tPosition: %d %d\n\tSize: %d %d",
		monitorPosX,
		monitorPosY,
		monitorWidth,
		monitorHeight
	);
}

auto Arg::WindowSystem::Shutdown() -> void
{
	ARG_CONSOLE_LOG("Shuting down Window System");
	glfwTerminate();
}

auto Arg::WindowSystem::InitializeGraphics() -> void
{
	ARG_ASSERT_M(glfwGetCurrentContext() != nullptr,
		"Please specify context before initializing GLAD!");
	ARG_CONSOLE_LOG("Initializing OpenGL");
	const auto gladLoaderState = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	ARG_ASSERT_M(gladLoaderState, "Failed to initialize GLAD!");

	const auto graphicsVersion = reinterpret_cast<const char*>(
		glGetString(GL_VERSION)
		);
	const auto graphicsSLVersion = reinterpret_cast<const char*>(
		glGetString(GL_SHADING_LANGUAGE_VERSION)
		);
	ARG_CONSOLE_LOG("Using OpenGL version:\n\t%s", graphicsVersion);
	ARG_CONSOLE_LOG("Using GLSL version:\n\t%s", graphicsSLVersion);

	const auto graphicsVendorName = reinterpret_cast<const char*>(
		glGetString(GL_VENDOR)
		);
	const auto graphicsRendererName = reinterpret_cast<const char*>(
		glGetString(GL_RENDERER)
		);
	ARG_CONSOLE_LOG("Detected graphics device:\n\t%s: %s",
		graphicsVendorName,
		graphicsRendererName
	);
}

bool Arg::Window::s_bHasInitializedGraphics = false;

Arg::Window::Window(const WindowSpec& spec)
	: m_Title(spec.Title),
	m_Width(spec.Width),
	m_Height(spec.Height),
	m_Mode(spec.Mode),
	m_bIsVSync(spec.bIsVSync)
{
}

auto Arg::Window::Create() -> bool
{
	ARG_ASSERT_M(m_pWindowHandle == nullptr, "Window is already created!");
	ARG_CONSOLE_LOG("Creating a window \"%s\"", m_Title.c_str());
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

	m_pWindowHandle = glfwCreateWindow(
		m_Width,
		m_Height,
		m_Title.c_str(),
		nullptr,
		nullptr
	);
	ARG_ASSERT_M(m_pWindowHandle != nullptr, "Failed to create a window!");

	glfwSetWindowUserPointer(
		static_cast<GLFWwindow*>(m_pWindowHandle),
		this
	);

	MakeCurrent();
	SetMode(m_Mode);
	SetVSyncEnabled(m_bIsVSync);

	glfwSetFramebufferSizeCallback(
		static_cast<GLFWwindow*>(m_pWindowHandle),
		[](
			GLFWwindow* windowHandle,
			int newWidth,
			int newHeight)
		{
			const auto windowUser = static_cast<Window*>(glfwGetWindowUserPointer(windowHandle));
			windowUser->SetWidth(newWidth);
			windowUser->SetHeight(newHeight);
		}
	);

	m_Input.Initialize();

	glfwSetKeyCallback(
		static_cast<GLFWwindow*>(m_pWindowHandle),

		[](
			GLFWwindow* windowHandle,
			int key,
			int scanCode,
			int action,
			int mods
			)
		{
			const auto windowUser = static_cast<Window*>(glfwGetWindowUserPointer(windowHandle));
			switch (action)
			{
			case GLFW_PRESS:
				windowUser->m_Input.OnKeyPressed(key, mods);
				break;
			case GLFW_RELEASE:
				windowUser->m_Input.OnKeyReleased(key);
				break;
			default:
				break;
			}
		}
	);
	glfwSetMouseButtonCallback(
		static_cast<GLFWwindow*>(m_pWindowHandle),
		[](
			GLFWwindow* windowHandle,
			int button,
			int action,
			int mods
			)
		{
			const auto windowUser = static_cast<Window*>(glfwGetWindowUserPointer(windowHandle));
			switch (action)
			{
			case GLFW_PRESS:
				windowUser->m_Input.OnMouseButtonPressed(button, mods);
				break;
			case GLFW_RELEASE:
				windowUser->m_Input.OnMouseButtonReleased(button);
				break;
			default:
				break;
			}
		}

	);
	glfwSetCursorPosCallback(
		static_cast<GLFWwindow*>(m_pWindowHandle),
		[](
			GLFWwindow* windowHandle,
			double posX,
			double posY
			)
		{
			const auto windowUser = static_cast<Window*>(glfwGetWindowUserPointer(windowHandle));
			windowUser->m_Input.OnMousePositionChanged(Vec2(posX, posY));
		}

	);
	glfwSetScrollCallback(
		static_cast<GLFWwindow*>(m_pWindowHandle),
		[](
			GLFWwindow* windowHandle,
			double horizontal,
			double vertical
			)
		{
			const auto windowUser = static_cast<Window*>(glfwGetWindowUserPointer(windowHandle));
			windowUser->m_Input.OnMouseScrollChanged(vertical);
		}
	);
	glfwSetJoystickCallback(
		[](int id, int event)
		{
			GLFWwindow* windowHandle = glfwGetCurrentContext();
			const auto windowUser = static_cast<Window*>(glfwGetWindowUserPointer(windowHandle));
			switch (event)
			{
			case GLFW_CONNECTED:
				windowUser->m_Input.OnGamepadConnected(id);
				break;
			case GLFW_DISCONNECTED:
				windowUser->m_Input.OnGamepadDisconnected(id);
				break;
			default:
				break;
			}
		}
	);

	return true;
}

void Arg::Window::MakeCurrent()
{
	ARG_ASSERT_M(m_pWindowHandle != nullptr, "Please create a window before using it!");
	glfwMakeContextCurrent(static_cast<GLFWwindow*>(m_pWindowHandle));
	if (!s_bHasInitializedGraphics)
	{
		Arg::WindowSystem::InitializeGraphics();
	}
}

void Arg::Window::SwapBuffers()
{
	ARG_ASSERT_M(m_pWindowHandle != nullptr, "Please create a window before using it!");
	glfwSwapBuffers(static_cast<GLFWwindow*>(m_pWindowHandle));
}

void Arg::Window::Destroy()
{
	ARG_ASSERT_M(m_pWindowHandle != nullptr, "Please create a window before using it!");
	ARG_CONSOLE_LOG("Destroying a window \"%s\"", m_Title.c_str());
	glfwDestroyWindow(static_cast<GLFWwindow*>(m_pWindowHandle));
}

void Arg::Window::Update()
{
	m_Input.PullEvents();
}

auto Arg::Window::ShouldClose() const -> bool
{
	ARG_ASSERT_M(m_pWindowHandle != nullptr, "Please create a window before using it!");
	return glfwWindowShouldClose(static_cast<GLFWwindow*>(m_pWindowHandle));
}

void Arg::Window::Close()
{
	ARG_ASSERT_M(m_pWindowHandle != nullptr, "Please create a window before using it!");
	glfwSetWindowShouldClose(static_cast<GLFWwindow*>(m_pWindowHandle), GLFW_TRUE);
}

auto Arg::Window::GetElapsedTime() const -> float
{
	return static_cast<float>(glfwGetTime());
}

void Arg::Window::SetTitle(const std::string& title)
{
	m_Title = title;
	glfwSetWindowTitle(static_cast<GLFWwindow*>(m_pWindowHandle), m_Title.c_str());
}

auto Arg::Window::GetWidth() const -> int32_t
{
	ARG_ASSERT_M(m_pWindowHandle != nullptr, "Please create a window before using it!");
	switch (m_Mode)
	{
	case Windowed:
		return m_Width;
	case Fullscreen:
		const auto primaryMonitor = glfwGetPrimaryMonitor();
		const auto monitorMode = glfwGetVideoMode(primaryMonitor);
		return monitorMode->width;
	}

	return 0;
}

void Arg::Window::SetWidth(int32_t width)
{
	ARG_ASSERT_M(m_pWindowHandle != nullptr, "Please create a window before using it!");
	ARG_ASSERT(width >= 0);
	switch (m_Mode)
	{
	case Windowed:
		m_Width = width;
		glfwSetWindowSize(
			static_cast<GLFWwindow*>(m_pWindowHandle),
			m_Width,
			m_Height
		);
		break;
	case Fullscreen:
		break;
	}
}

auto Arg::Window::GetHeight() const -> int32_t
{
	ARG_ASSERT_M(m_pWindowHandle != nullptr, "Please create a window before using it!");
	switch (m_Mode)
	{
	case Windowed:
		return m_Height;
	case Fullscreen:
		const auto primaryMonitor = glfwGetPrimaryMonitor();
		const auto monitorMode = glfwGetVideoMode(primaryMonitor);
		return monitorMode->height;
	}

	return 0;
}

void Arg::Window::SetHeight(int32_t height)
{
	ARG_ASSERT_M(m_pWindowHandle != nullptr, "Please create a window before using it!");
	ARG_ASSERT(height >= 0);
	switch (m_Mode)
	{
	case Windowed:
		m_Height = height;
		glfwSetWindowSize(
			static_cast<GLFWwindow*>(m_pWindowHandle),
			m_Width,
			m_Height
		);
		break;
	case Fullscreen:
		break;
	}
}

void Arg::Window::SetMode(WindowMode mode)
{
	ARG_ASSERT_M(m_pWindowHandle != nullptr, "Please create a window before using it!");
	m_Mode = mode;
	switch (m_Mode)
	{
	case Windowed:
	{
		const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
		const int monitorWidth = mode->width;
		const int monitorHeight = mode->height;
		const int xPos = (monitorWidth - m_Width) / 2;
		const int yPos = (monitorHeight - m_Height) / 2;
		glfwSetWindowMonitor(
			static_cast<GLFWwindow*>(m_pWindowHandle),
			nullptr,
			xPos,
			yPos,
			m_Width,
			m_Height,
			0
		);
		break;
	}
	case Fullscreen:
	{
		const auto primaryMonitor = glfwGetPrimaryMonitor();
		const auto monitorMode = glfwGetVideoMode(primaryMonitor);
		glfwSetWindowMonitor(
			static_cast<GLFWwindow*>(m_pWindowHandle),
			primaryMonitor,
			0,
			0,
			monitorMode->width,
			monitorMode->height,
			monitorMode->refreshRate
		);
		break;
	}
	}
}

void Arg::Window::SetVSyncEnabled(bool bIsEnabled)
{
	ARG_ASSERT_M(m_pWindowHandle != nullptr, "Please create a window before using it!");
	m_bIsVSync = bIsEnabled;
	glfwSwapInterval(m_bIsVSync ? 1 : 0);
}

void Arg::Window::SetCursorMode(Input::CursorMode cursorMode)
{
	glfwSetInputMode(static_cast<GLFWwindow*>(m_pWindowHandle), GLFW_CURSOR, cursorMode);
	m_Input.OnCursorModeChanged(cursorMode);
}
