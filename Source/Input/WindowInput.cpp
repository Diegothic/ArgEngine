#include "WindowInput.h"

#include <iostream>
#include <ranges>
#include <GLFW/glfw3.h>

#include "Arg/Debug.h"

std::map<GLFWwindow*, Arg::WindowInput*> Arg::WindowInput::s_WindowInputRegistry;

Arg::WindowInput::WindowInput()
	: m_pWindowHandle(nullptr),
	m_pKeyboardState(nullptr),
	m_pMouseState(nullptr)
{
}

Arg::WindowInput::~WindowInput()
{
	s_WindowInputRegistry.erase(m_pWindowHandle);
}

void Arg::WindowInput::Initialize(GLFWwindow* windowHandle)
{
	AE_CORE_LOG_INFO("Initializing Input");
	m_pWindowHandle = windowHandle;
	s_WindowInputRegistry[m_pWindowHandle] = this;

	CreateKeyboardState();
	CreateMouseState();
	for (int i = 0; i < GAMEPAD_MAX; i++)
	{
		if (glfwJoystickPresent(i)
			&& glfwJoystickIsGamepad(i))
		{
			AE_CORE_LOG_INFO("Found connected Gamepad:\n\t(%d) %s",
				i,
				glfwGetGamepadName(i)
			);
			CreateGamepadStateForID(i);
		}
	}

	glfwSetKeyCallback(
		m_pWindowHandle,
		WindowInput::InputKeyCallback
	);
	glfwSetCursorPosCallback(
		m_pWindowHandle,
		WindowInput::InputMousePositionCallback
	);
	glfwSetMouseButtonCallback(
		m_pWindowHandle,
		WindowInput::InputMouseButtonCallback
	);
	glfwSetScrollCallback(
		m_pWindowHandle,
		WindowInput::InputMouseScrollCallback
	);
	glfwSetJoystickCallback(
		WindowInput::InputGamepadCallback
	);
}

void Arg::WindowInput::PrePullEvents() const
{
	m_pKeyboardState->Update();
	m_pMouseState->Update();
	for (auto& gamepad : m_pGamepadStateForID | std::views::values)
	{
		gamepad->Update();
	}
}

void Arg::WindowInput::PostPullEvents() const
{
	GLFWgamepadstate glfwGamepadState;
	for (int i = 0; i < GAMEPAD_MAX; i++)
	{
		if (!m_pGamepadStateForID.contains(i))
		{
			continue;
		}
		const Rc<GamepadState>& gamepadState = m_pGamepadStateForID.at(i);
		if (gamepadState == nullptr)
		{
			continue;
		}

		if (glfwGetGamepadState(i, &glfwGamepadState))
		{
			for (int button = 0; button < 15; button++)
			{
				switch (glfwGamepadState.buttons[button])
				{
				case GLFW_PRESS:
					gamepadState->OnButtonPressed(button);
					break;
				case GLFW_RELEASE:
					gamepadState->OnButtonReleased(button);
					break;
				default:
					break;
				}
			}

			for (int axis = 0; axis < 6; axis++)
			{
				gamepadState->OnAxisChanged(axis, glfwGamepadState.axes[axis]);
			}
		}
	}
}

const Arg::Rc<Arg::KeyboardState>& Arg::WindowInput::GetKeyboardState() const
{
	return m_pKeyboardState;
}

const Arg::Rc<Arg::MouseState>& Arg::WindowInput::GetMouseState() const
{
	return m_pMouseState;
}

const Arg::Rc<Arg::GamepadState>& Arg::WindowInput::GetGamepadState(int id) const
{
	if (!m_pGamepadStateForID.contains(id))
	{
		return NullRc<Arg::GamepadState>;
	}

	return m_pGamepadStateForID.at(id);
}

void Arg::WindowInput::CreateKeyboardState()
{
	m_pKeyboardState = NewRc<KeyboardState>();
}

void Arg::WindowInput::CreateMouseState()
{
	m_pMouseState = NewRc<MouseState>();
}

void Arg::WindowInput::CreateGamepadStateForID(int id)
{
	if (m_pGamepadStateForID.contains(id))
	{
		m_pGamepadStateForID.erase(id);
	}

	m_pGamepadStateForID[id] = NewRc<GamepadState>(id);
}

void Arg::WindowInput::RemoveGamepadStateForID(int id)
{
	if (!m_pGamepadStateForID.contains(id))
	{
		return;
	}

	m_pGamepadStateForID.erase(id);
}

void Arg::WindowInput::OnKeyPressed(int key, int mods) const
{
	if (m_pKeyboardState == nullptr)
	{
		return;
	}

	m_pKeyboardState->OnKeyPressed(key);
	m_pKeyboardState->OnModsChanged(mods);
}

void Arg::WindowInput::OnKeyReleased(int key) const
{
	if (m_pKeyboardState == nullptr)
	{
		return;
	}

	m_pKeyboardState->OnKeyReleased(key);
}

void Arg::WindowInput::OnMouseButtonPressed(int button, int mods) const
{
	if (m_pMouseState == nullptr)
	{
		return;
	}

	m_pMouseState->OnButtonPressed(button);
	m_pMouseState->OnModsChanged(mods);
}

void Arg::WindowInput::OnMouseButtonReleased(int button) const
{
	if (m_pMouseState == nullptr)
	{
		return;
	}

	m_pMouseState->OnButtonReleased(button);
}

void Arg::WindowInput::OnMousePositionChanged(Vec2 position) const
{
	if (m_pMouseState == nullptr)
	{
		return;
	}

	m_pMouseState->OnPositionChanged(position);
}

void Arg::WindowInput::OnMouseScrollChanged(double scroll) const
{
	if (m_pMouseState == nullptr)
	{
		return;
	}

	m_pMouseState->OnScrollChanged(scroll);
}

void Arg::WindowInput::OnGamepadConnected(int id)
{
	AE_CORE_LOG_INFO("Gamepad connected:\n\t(%d) %s",
		id,
		glfwGetGamepadName(id)
	);
	CreateGamepadStateForID(id);
}

void Arg::WindowInput::OnGamepadDisconnected(int id)
{
	if (!m_pGamepadStateForID.contains(id))
	{
		return;
	}

	AE_CORE_LOG_INFO("Gamepad disconnected:\n\t(%d)",
		id
	);
	RemoveGamepadStateForID(id);
}

void Arg::WindowInput::InputKeyCallback(
	GLFWwindow* windowHandle,
	int key,
	int scanCode,
	int action,
	int mods
)
{
	if (!s_WindowInputRegistry.contains(windowHandle))
	{
		return;
	}

	const WindowInput* windowInput = s_WindowInputRegistry[windowHandle];
	switch (action)
	{
	case GLFW_PRESS:
		windowInput->OnKeyPressed(key, mods);
		break;
	case GLFW_RELEASE:
		windowInput->OnKeyReleased(key);
		break;
	default:
		break;
	}
}

void Arg::WindowInput::InputMouseButtonCallback(
	GLFWwindow* windowHandle,
	int button,
	int action,
	int mods
)
{

	if (!s_WindowInputRegistry.contains(windowHandle))
	{
		return;
	}

	const WindowInput* windowInput = s_WindowInputRegistry[windowHandle];
	switch (action)
	{
	case GLFW_PRESS:
		windowInput->OnMouseButtonPressed(button, mods);
		break;
	case GLFW_RELEASE:
		windowInput->OnMouseButtonReleased(button);
		break;
	default:
		break;
	}
}

void Arg::WindowInput::InputMousePositionCallback(
	GLFWwindow* windowHandle,
	double posX,
	double posY
)
{
	if (!s_WindowInputRegistry.contains(windowHandle))
	{
		return;
	}

	const WindowInput* windowInput = s_WindowInputRegistry[windowHandle];
	windowInput->OnMousePositionChanged(Vec2(posX, posY));
}

void Arg::WindowInput::InputMouseScrollCallback(
	GLFWwindow* windowHandle,
	double horizontal,
	double vertical
)
{
	if (!s_WindowInputRegistry.contains(windowHandle))
	{
		return;
	}

	const WindowInput* windowInput = s_WindowInputRegistry[windowHandle];
	windowInput->OnMouseScrollChanged(vertical);
}

void Arg::WindowInput::InputGamepadCallback(int id, int event)
{
	for (WindowInput* windowInput : s_WindowInputRegistry | std::views::values)
	{
		switch (event)
		{
		case GLFW_CONNECTED:
			windowInput->OnGamepadConnected(id);
			break;
		case GLFW_DISCONNECTED:
			windowInput->OnGamepadDisconnected(id);
			break;
		default:
			break;
		}
	}
}
