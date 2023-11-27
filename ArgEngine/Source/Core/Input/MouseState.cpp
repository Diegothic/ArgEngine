#include <arg_pch.hpp>
#include "MouseState.hpp"

auto Arg::Input::MouseState::IsButtonPressed(MouseButton button, int mods) const -> bool
{
	return IsButtonPressed(static_cast<int>(button), mods);
}

auto Arg::Input::MouseState::IsButtonDown(MouseButton button) const -> bool
{
	return IsButtonDown(static_cast<int>(button));
}

auto Arg::Input::MouseState::IsButtonUp(MouseButton button) const -> bool
{
	return IsButtonUp(static_cast<int>(button));
}

auto Arg::Input::MouseState::IsButtonReleased(MouseButton button) const -> bool
{
	return IsButtonReleased(static_cast<int>(button));
}

auto Arg::Input::MouseState::IsButtonPressed(int button, int mods) const -> bool
{
	if (button < MOUSE_BUTTON_MIN || button > MOUSE_BUTTON_MAX)
	{
		return false;
	}

	if (mods == 0 && m_Mods > 0)
	{
		return false;
	}

	if (mods > 0 && mods != m_Mods)
	{
		return false;
	}

	return GetButtonDownState(button) && !GetLastButtonDownState(button);
}

auto Arg::Input::MouseState::IsButtonDown(int button) const -> bool
{
	if (button < MOUSE_BUTTON_MIN || button > MOUSE_BUTTON_MAX)
	{
		return false;
	}

	return GetButtonDownState(button);
}

auto Arg::Input::MouseState::IsButtonUp(int button) const -> bool
{
	if (button < MOUSE_BUTTON_MIN || button > MOUSE_BUTTON_MAX)
	{
		return false;
	}

	return !GetButtonDownState(button);
}

auto Arg::Input::MouseState::IsButtonReleased(int button) const -> bool
{
	if (button < MOUSE_BUTTON_MIN || button > MOUSE_BUTTON_MAX)
	{
		return false;
	}

	return !GetButtonDownState(button) && GetLastButtonDownState(button);
}

auto Arg::Input::MouseState::GetPosition() const -> Vec2
{
	return m_Position;
}

auto Arg::Input::MouseState::GetPositionDelta() const -> Vec2
{
	return m_Position - m_LastPosition;
}

auto Arg::Input::MouseState::GetScrollDelta() const -> double
{
	return m_Scroll - m_LastScroll;
}

auto Arg::Input::MouseState::GetCursorMode() const -> const CursorMode&
{
	return m_CursorMode;
}

void Arg::Input::MouseState::OnCursorModeChanged(CursorMode cursorMode)
{
	m_CursorMode = cursorMode;
}

void Arg::Input::MouseState::OnButtonPressed(int button)
{
	m_ButtonDownState[button] = true;
}

void Arg::Input::MouseState::OnButtonReleased(int button)
{
	m_ButtonDownState[button] = false;
}

void Arg::Input::MouseState::OnModsChanged(int mods)
{
	m_Mods = mods;
}

void Arg::Input::MouseState::OnPositionChanged(Vec2 position)
{
	m_Position = position;
}

void Arg::Input::MouseState::OnScrollChanged(double scroll)
{
	m_Scroll = scroll;
}

void Arg::Input::MouseState::Update()
{
	m_LastButtonDownState = m_ButtonDownState;
	m_LastPosition = m_Position;
	m_LastScroll = m_Scroll;
}

auto Arg::Input::MouseState::GetButtonDownState(int button) const -> bool
{
	if (!m_ButtonDownState.contains(button))
	{
		return false;
	}

	return m_ButtonDownState.at(button);
}

auto Arg::Input::MouseState::GetLastButtonDownState(int button) const -> bool
{
	if (!m_LastButtonDownState.contains(button))
	{
		return false;
	}

	return m_LastButtonDownState.at(button);
}
