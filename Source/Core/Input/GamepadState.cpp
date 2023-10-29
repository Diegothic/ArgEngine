#include "GamepadState.h"

Arg::GamepadState::GamepadState(int id)
	: m_ID(id)
{
}

bool Arg::GamepadState::IsButtonPressed(GamepadButton button) const
{
	return GetButtonDownState(button) && !GetLastButtonDownState(button);
}

bool Arg::GamepadState::IsButtonDown(GamepadButton button) const
{
	return GetButtonDownState(button);
}

bool Arg::GamepadState::IsButtonUp(GamepadButton button) const
{
	return !GetButtonDownState(button);
}

bool Arg::GamepadState::IsButtonReleased(GamepadButton button) const
{
	return !GetButtonDownState(button) && GetLastButtonDownState(button);
}

float Arg::GamepadState::GetAxis(GamepadAxis axis) const
{
	return GetAxisState(axis);
}

float Arg::GamepadState::GetAxisDelta(GamepadAxis axis) const
{
	return GetAxisState(axis) - GetLastAxisState(axis);
}

Arg::Vec2 Arg::GamepadState::GetAxis2D(GamepadAxis2D axis) const
{
	float axisX = 0.0f;
	float axisY = 0.0f;
	switch (axis)
	{
	case Arg::Left:
		axisX = GetAxisState(GamepadAxis::LeftHorizontal);
		axisY = GetAxisState(GamepadAxis::LeftVertical);
		break;
	case Arg::Right:
		axisX = GetAxisState(GamepadAxis::RightHorizontal);
		axisY = GetAxisState(GamepadAxis::RightVertical);
		break;
	}

	return { axisX, axisY };
}

Arg::Vec2 Arg::GamepadState::GetAxis2DDelta(GamepadAxis2D axis) const
{
	float axisDeltaX = 0.0f;
	float axisDeltaY = 0.0f;
	switch (axis)
	{
	case Arg::Left:
		axisDeltaX = GetAxisState(GamepadAxis::LeftHorizontal)
			- GetLastAxisState(GamepadAxis::LeftHorizontal);
		axisDeltaY = GetAxisState(GamepadAxis::LeftVertical)
			- GetLastAxisState(GamepadAxis::LeftVertical);
		break;
	case Arg::Right:
		axisDeltaX = GetAxisState(GamepadAxis::RightHorizontal)
			- GetLastAxisState(GamepadAxis::RightHorizontal);
		axisDeltaY = GetAxisState(GamepadAxis::RightVertical)
			- GetLastAxisState(GamepadAxis::RightVertical);
		break;
	}

	return { axisDeltaX, axisDeltaY };
}

void Arg::GamepadState::OnButtonPressed(int button)
{
	m_ButtonDownState[button] = true;
}

void Arg::GamepadState::OnButtonReleased(int button)
{
	m_ButtonDownState[button] = false;
}

void Arg::GamepadState::OnAxisChanged(int axis, float value)
{
	m_AxisState[axis] = value;
}

void Arg::GamepadState::Update()
{
	m_LastButtonDownState = m_ButtonDownState;
	m_LastAxisState = m_AxisState;
}

bool Arg::GamepadState::GetButtonDownState(int button) const
{
	if (!m_ButtonDownState.contains(button))
	{
		return false;
	}

	return m_ButtonDownState.at(button);
}

bool Arg::GamepadState::GetLastButtonDownState(int button) const
{
	if (!m_LastButtonDownState.contains(button))
	{
		return false;
	}

	return m_LastButtonDownState.at(button);
}

float Arg::GamepadState::GetAxisState(int axis) const
{
	if (!m_AxisState.contains(axis))
	{
		return false;
	}

	const float axisState = m_AxisState.at(axis);
	return axisState < GAMEPAD_AXIS_DEADZONE && axisState > -GAMEPAD_AXIS_DEADZONE
		? 0.0f
		: axisState;
}

float Arg::GamepadState::GetLastAxisState(int axis) const
{
	if (!m_LastAxisState.contains(axis))
	{
		return false;
	}

	const float axisState = m_LastAxisState.at(axis);
	return axisState < GAMEPAD_AXIS_DEADZONE && axisState > -GAMEPAD_AXIS_DEADZONE
		? 0.0f
		: axisState;
}
