#include "Window.h"

#include <iostream>
#include <ranges>
#include <GLFW/glfw3.h>

#include "Arg/Debug.h"

Arg::Window::Window(const WindowSpec& spec)
	: m_pWindowHandle(nullptr),
	m_pWindowInput(nullptr)
{
	m_Title = spec.title;
	m_Size = glm::uvec2(spec.width, spec.height);
}

bool Arg::Window::Create()
{
	AE_CORE_LOG_INFO("Creating a window");
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
		AE_CORE_LOG_ERR("Failed to create a window!");
		return false;
	}

	glfwMakeContextCurrent(m_pWindowHandle);
	glfwSwapInterval(1);

	// TODO: Move to renderer
	glViewport(
		0,
		0,
		static_cast<int>(GetWidth()),
		static_cast<int>(GetHeight())
	);

	m_pWindowInput = new WindowInput();
	m_pWindowInput->Initialize(m_pWindowHandle);

	VOnCreate();

	return true;
}

void Arg::Window::Update()
{
	m_pWindowInput->PrePullEvents();
	glfwPollEvents();
	m_pWindowInput->PostPullEvents();

	// TODO: Remove input test
	const KeyboardState* keyboardState = m_pWindowInput->GetKeyboardState();
	const MouseState* mouseState = m_pWindowInput->GetMouseState();
	const GamepadState* gamepadState = m_pWindowInput->GetGamepadState(0);

	if (keyboardState->IsKeyPressed(KeyCode::A))
	{
		std::cout << "A Pressed" << std::endl;
	}

	if (keyboardState->IsKeyPressed(KeyCode::A, KeyMods::Ctrl))
	{
		std::cout << "A Pressed with Ctrl" << std::endl;
	}

	if (keyboardState->IsKeyPressed(KeyCode::A, KeyMods::Ctrl | KeyMods::Shift))
	{
		std::cout << "A Pressed with Ctrl and Shift" << std::endl;
	}

	if (keyboardState->IsKeyDown(KeyCode::A))
	{
		std::cout << "A Down" << std::endl;
	}

	if (keyboardState->IsKeyReleased(KeyCode::A))
	{
		std::cout << "A Released" << std::endl;
	}

	if (keyboardState->IsKeyPressed(KeyCode::S, KeyMods::Ctrl))
	{
		std::cout << "S Pressed with Ctrl" << std::endl;
	}

	if (mouseState->IsButtonPressed(MouseButton::Right))
	{
		std::cout << "Right button Pressed" << std::endl;
	}

	if (mouseState->IsButtonDown(MouseButton::Right))
	{
		std::cout << "Right button Down" << std::endl;
	}

	if (mouseState->IsButtonReleased(MouseButton::Right))
	{
		std::cout << "Right button Released" << std::endl;
	}

	if (gamepadState != nullptr)
	{
		if (gamepadState->GetAxis2D(GamepadAxis2D::Left) != Vec2(0.0))
		{
			const Vec2 gamepadAxisLeft = gamepadState->GetAxis2DDelta(GamepadAxis2D::Left);
			std::cout << "Gamepad axis left: "
				<< gamepadAxisLeft.x
				<< " "
				<< gamepadAxisLeft.y
				<< std::endl;
		}

		if (gamepadState->IsButtonPressed(GamepadButton::South))
		{
			std::cout << "Gamepad A button Pressed" << std::endl;
		}

		if (gamepadState->IsButtonDown(GamepadButton::South))
		{
			std::cout << "Gamepad A button Down" << std::endl;
		}

		if (gamepadState->IsButtonReleased(GamepadButton::South))
		{
			std::cout << "Gamepad A button Released" << std::endl;
		}
	}

	// TODO: Move to renderer
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT
		| GL_DEPTH_BUFFER_BIT
		| GL_STENCIL_BUFFER_BIT
	);

	VOnUpdate();

	glfwSwapBuffers(m_pWindowHandle);
}

void Arg::Window::Destroy()
{
	AE_CORE_LOG_INFO("Destroying a window");

	VOnDestroy();

	if (m_pWindowInput != nullptr)
	{
		delete m_pWindowInput;
		m_pWindowInput = nullptr;
	}

	glfwDestroyWindow(m_pWindowHandle);
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
