#include <arg_pch.hpp>
#include "Input.hpp"

#include <GLFW/glfw3.h>

#include "Debug/Assert.hpp"
#include "Debug/Log.hpp"

void Arg::Input::Input::Initialize()
{
	ARG_CONSOLE_LOG("Initializing Input");
	for (int i = 0; i < GAMEPADS_CONNECTED_MAX; i++)
	{
		m_GamepadStateForID[i] = GamepadState(i);
		if (glfwJoystickPresent(i)
			&& glfwJoystickIsGamepad(i))
		{
			ARG_CONSOLE_LOG("Found connected Gamepad:\n\t(%d) %s",
				i,
				glfwGetGamepadName(i)
			);
			m_GamepadStateForID[i].OnGamepadConnected();
		}
	}
}

void Arg::Input::Input::PullEvents()
{
	PrePullEvents();
	glfwPollEvents();
	PostPullEvents();
}

void Arg::Input::Input::PrePullEvents()
{
	m_KeyboardState.Update();
	m_MouseState.Update();
	for (auto& gamepadState : m_GamepadStateForID | std::views::values)
	{
		gamepadState.Update();
	}
}

void Arg::Input::Input::PostPullEvents()
{
	GLFWgamepadstate glfwGamepadState;
	for (int i = 0; i < GAMEPADS_CONNECTED_MAX; i++)
	{
		if (!m_GamepadStateForID.contains(i))
		{
			continue;
		}

		auto& gamepadState = m_GamepadStateForID.at(i);
		if (glfwGetGamepadState(i, &glfwGamepadState))
		{
			for (int button = 0; button < 15; button++)
			{
				switch (glfwGamepadState.buttons[button])
				{
				case GLFW_PRESS:
					gamepadState.OnButtonPressed(button);
					break;
				case GLFW_RELEASE:
					gamepadState.OnButtonReleased(button);
					break;
				default:
					break;
				}
			}

			for (int axis = 0; axis < 6; axis++)
			{
				gamepadState.OnAxisChanged(axis, glfwGamepadState.axes[axis]);
			}
		}
	}
}

auto Arg::Input::Input::GetKeyboardState() const -> const KeyboardState&
{
	return m_KeyboardState;
}

auto Arg::Input::Input::GetMouseState() const -> const MouseState&
{
	return m_MouseState;
}

auto Arg::Input::Input::GetGamepadState(int id) const -> const GamepadState&
{
	ARG_ASSERT(m_GamepadStateForID.contains(id), "Gamepad state non-existent!");
	return m_GamepadStateForID.at(id);
}

void Arg::Input::Input::OnCursorModeChanged(CursorMode mode)
{
	m_MouseState.OnCursorModeChanged(mode);
}

void Arg::Input::Input::OnKeyPressed(int key, int mods)
{
	m_KeyboardState.OnKeyPressed(key);
	m_KeyboardState.OnModsChanged(mods);
}

void Arg::Input::Input::OnKeyReleased(int key)
{
	m_KeyboardState.OnKeyReleased(key);
}

void Arg::Input::Input::OnMouseButtonPressed(int button, int mods)
{
	m_MouseState.OnButtonPressed(button);
	m_MouseState.OnModsChanged(mods);
}

void Arg::Input::Input::OnMouseButtonReleased(int button)
{
	m_MouseState.OnButtonReleased(button);
}

void Arg::Input::Input::OnMousePositionChanged(Vec2 position)
{
	m_MouseState.OnPositionChanged(position);
}

void Arg::Input::Input::OnMouseScrollChanged(double scroll)
{
	m_MouseState.OnScrollChanged(scroll);
}

void Arg::Input::Input::OnGamepadConnected(int id)
{
	ARG_CONSOLE_LOG("Gamepad connected:\n\t(%d) %s",
		id,
		glfwGetGamepadName(id)
	);
	m_GamepadStateForID[id].OnGamepadConnected();
}

void Arg::Input::Input::OnGamepadDisconnected(int id)
{
	ARG_CONSOLE_LOG("Gamepad disconnected:\n\t(%d)", id);
	m_GamepadStateForID[id].OnGamepadDisconnected();
}
