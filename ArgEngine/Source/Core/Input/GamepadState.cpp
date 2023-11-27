#include <arg_pch.hpp>
#include "GamepadState.hpp"

Arg::Input::GamepadState::GamepadState(int id)
	: m_ID(id)
{
}

auto Arg::Input::GamepadState::IsButtonPressed(GamepadButton button) const -> bool
{
	return GetButtonDownState(button) && !GetLastButtonDownState(button);
}

auto Arg::Input::GamepadState::IsButtonDown(GamepadButton button) const -> bool
{
	return GetButtonDownState(button);
}

auto Arg::Input::GamepadState::IsButtonUp(GamepadButton button) const -> bool
{
	return !GetButtonDownState(button);
}

auto Arg::Input::GamepadState::IsButtonReleased(GamepadButton button) const -> bool
{
	return !GetButtonDownState(button) && GetLastButtonDownState(button);
}

auto Arg::Input::GamepadState::GetAxis(GamepadAxis axis) const -> float
{
	return GetAxisState(axis);
}

auto Arg::Input::GamepadState::GetAxisDelta(GamepadAxis axis) const -> float
{
	return GetAxisState(axis) - GetLastAxisState(axis);
}

auto Arg::Input::GamepadState::GetAxis2D(GamepadAxis2D axis) const -> Vec2
{
	float axisX = 0.0f;
	float axisY = 0.0f;
	switch (axis)
	{
	case GamepadAxis2D::AxisLeft:
		axisX = GetAxisState(GamepadAxis::LeftHorizontal);
		axisY = GetAxisState(GamepadAxis::LeftVertical);
		break;
	case GamepadAxis2D::AxisRight:
		axisX = GetAxisState(GamepadAxis::RightHorizontal);
		axisY = GetAxisState(GamepadAxis::RightVertical);
		break;
	}

	return { axisX, axisY };
}

auto Arg::Input::GamepadState::GetAxis2DDelta(GamepadAxis2D axis) const -> Vec2
{
	float axisDeltaX = 0.0f;
	float axisDeltaY = 0.0f;
	switch (axis)
	{
	case GamepadAxis2D::AxisLeft:
		axisDeltaX = GetAxisState(GamepadAxis::LeftHorizontal)
			- GetLastAxisState(GamepadAxis::LeftHorizontal);
		axisDeltaY = GetAxisState(GamepadAxis::LeftVertical)
			- GetLastAxisState(GamepadAxis::LeftVertical);
		break;
	case GamepadAxis2D::AxisRight:
		axisDeltaX = GetAxisState(GamepadAxis::RightHorizontal)
			- GetLastAxisState(GamepadAxis::RightHorizontal);
		axisDeltaY = GetAxisState(GamepadAxis::RightVertical)
			- GetLastAxisState(GamepadAxis::RightVertical);
		break;
	}

	return { axisDeltaX, axisDeltaY };
}

void Arg::Input::GamepadState::OnButtonPressed(int button)
{
	m_ButtonDownState[button] = true;
}

void Arg::Input::GamepadState::OnButtonReleased(int button)
{
	m_ButtonDownState[button] = false;
}

void Arg::Input::GamepadState::OnAxisChanged(int axis, float value)
{
	m_AxisState[axis] = value;
}

auto Arg::Input::GamepadState::Update() -> void
{
	m_LastButtonDownState = m_ButtonDownState;
	m_LastAxisState = m_AxisState;
}

auto Arg::Input::GamepadState::GetButtonDownState(int button) const -> bool
{
	if (!m_ButtonDownState.contains(button))
	{
		return false;
	}

	return m_ButtonDownState.at(button);
}

auto Arg::Input::GamepadState::GetLastButtonDownState(int button) const -> bool
{
	if (!m_LastButtonDownState.contains(button))
	{
		return false;
	}

	return m_LastButtonDownState.at(button);
}

auto Arg::Input::GamepadState::GetAxisState(int axis) const -> float
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

auto Arg::Input::GamepadState::GetLastAxisState(int axis) const -> float
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
