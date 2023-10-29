#include "MouseState.h"

Arg::MouseState::MouseState()
	: m_Position(0.0f),
	m_LastPosition(0.0f),
	m_Scroll(0.0),
	m_LastScroll(0.0),
	m_Mods(0),
	m_CursorMode(CursorMode::Normal)
{
}

bool Arg::MouseState::IsButtonPressed(MouseButton button, int mods) const
{
	return IsButtonPressed(static_cast<int>(button), mods);
}

bool Arg::MouseState::IsButtonDown(MouseButton button) const
{
	return IsButtonDown(static_cast<int>(button));
}

bool Arg::MouseState::IsButtonUp(MouseButton button) const
{
	return IsButtonUp(static_cast<int>(button));
}

bool Arg::MouseState::IsButtonReleased(MouseButton button) const
{
	return IsButtonReleased(static_cast<int>(button));
}

bool Arg::MouseState::IsButtonPressed(int button, int mods) const
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

bool Arg::MouseState::IsButtonDown(int button) const
{
	if (button < MOUSE_BUTTON_MIN || button > MOUSE_BUTTON_MAX)
	{
		return false;
	}

	return GetButtonDownState(button);
}

bool Arg::MouseState::IsButtonUp(int button) const
{
	if (button < MOUSE_BUTTON_MIN || button > MOUSE_BUTTON_MAX)
	{
		return false;
	}

	return !GetButtonDownState(button);
}

bool Arg::MouseState::IsButtonReleased(int button) const
{
	if (button < MOUSE_BUTTON_MIN || button > MOUSE_BUTTON_MAX)
	{
		return false;
	}

	return !GetButtonDownState(button) && GetLastButtonDownState(button);
}

Arg::Vec2 Arg::MouseState::GetPosition() const
{
	return m_Position;
}

Arg::Vec2 Arg::MouseState::GetPositionDelta() const
{
	return m_Position - m_LastPosition;
}

double Arg::MouseState::GetScrollDelta() const
{
	return m_Scroll - m_LastScroll;
}

const CursorMode& Arg::MouseState::GetCursorMode() const
{
	return m_CursorMode;
}

void Arg::MouseState::OnCursorModeChanged(CursorMode cursorMode)
{
	m_CursorMode = cursorMode;
}

void Arg::MouseState::OnButtonPressed(int button)
{
	m_ButtonDownState[button] = true;
}

void Arg::MouseState::OnButtonReleased(int button)
{
	m_ButtonDownState[button] = false;
}

void Arg::MouseState::OnModsChanged(int mods)
{
	m_Mods = mods;
}

void Arg::MouseState::OnPositionChanged(Vec2 position)
{
	m_Position = position;
}

void Arg::MouseState::OnScrollChanged(double scroll)
{
	m_Scroll = scroll;
}

void Arg::MouseState::Update()
{
	m_LastButtonDownState = m_ButtonDownState;
	m_LastPosition = m_Position;
	m_LastScroll = m_Scroll;
}

bool Arg::MouseState::GetButtonDownState(int button) const
{
	if (!m_ButtonDownState.contains(button))
	{
		return false;
	}

	return m_ButtonDownState.at(button);
}

bool Arg::MouseState::GetLastButtonDownState(int button) const
{
	if (!m_LastButtonDownState.contains(button))
	{
		return false;
	}

	return m_LastButtonDownState.at(button);
}
