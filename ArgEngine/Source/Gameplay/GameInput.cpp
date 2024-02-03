#include <arg_pch.hpp>
#include "GameInput.hpp"

void Arg::Gameplay::GameInput::Initialize(Window* pWindow)
{
	m_pWindow = pWindow;
}

void Arg::Gameplay::GameInput::BeginPlay()
{
	m_ScreenSize = Vec2(m_pWindow->GetWidth(), m_pWindow->GetHeight());
	m_CursorPos = m_ScreenSize * 0.5f;
}

void Arg::Gameplay::GameInput::Tick(float deltaTime)
{
	m_ScreenSize = Vec2(m_pWindow->GetWidth(), m_pWindow->GetHeight());

	const auto& mouseState = m_pWindow->GetInput().GetMouseState();
	const Vec2 mouseDelta = mouseState.GetPositionDelta();
	m_CursorPos += mouseDelta;

	m_CursorPos.x = Math::clamp(m_CursorPos.x, 0.0f, m_ScreenSize.x);
	m_CursorPos.y = Math::clamp(m_CursorPos.y, 0.0f, m_ScreenSize.y);
}

auto Arg::Gameplay::GameInput::GetKeyPressed(const Input::KeyCode& keyCode) const -> bool
{
	const auto& keyboardState = m_pWindow->GetInput().GetKeyboardState();
	return keyboardState.IsKeyPressed(keyCode);
}

auto Arg::Gameplay::GameInput::GetKeyReleased(const Input::KeyCode& keyCode) const -> bool
{
	const auto& keyboardState = m_pWindow->GetInput().GetKeyboardState();
	return keyboardState.IsKeyReleased(keyCode);
}

auto Arg::Gameplay::GameInput::GetKeyDown(const Input::KeyCode& keyCode) const -> bool
{
	const auto& keyboardState = m_pWindow->GetInput().GetKeyboardState();
	return keyboardState.IsKeyDown(keyCode);
}

auto Arg::Gameplay::GameInput::GetMousePos() const -> Vec2
{
	return {m_CursorPos.x / m_ScreenSize.x, m_CursorPos.y / m_ScreenSize.y};
}

auto Arg::Gameplay::GameInput::GetMouseDelta() const -> Vec2
{
	const auto& mouseState = m_pWindow->GetInput().GetMouseState();
	const Vec2 mouseDelta = mouseState.GetPositionDelta();
	return {mouseDelta.x / m_ScreenSize.x, mouseDelta.y / m_ScreenSize.y};
}

auto Arg::Gameplay::GameInput::GetMouseButtonPressed(const Input::MouseButton& mouseButton) const -> bool
{
	const auto& mouseState = m_pWindow->GetInput().GetMouseState();
	return mouseState.IsButtonPressed(mouseButton);
}

auto Arg::Gameplay::GameInput::GetMouseButtonReleased(const Input::MouseButton& mouseButton) const -> bool
{
	const auto& mouseState = m_pWindow->GetInput().GetMouseState();
	return mouseState.IsButtonReleased(mouseButton);
}

auto Arg::Gameplay::GameInput::GetMouseButtonDown(const Input::MouseButton& mouseButton) const -> bool
{
	const auto& mouseState = m_pWindow->GetInput().GetMouseState();
	return mouseState.IsButtonDown(mouseButton);
}

auto Arg::Gameplay::GameInput::IsGamepadConnected(int32_t ID) const -> bool
{
	ID = Math::clamp(ID, 0, Input::GAMEPADS_CONNECTED_MAX - 1);
	const auto& gamepadState = m_pWindow->GetInput().GetGamepadState(ID);
	return gamepadState.IsConnected();
}

auto Arg::Gameplay::GameInput::GetGamepadButtonPressed(
	int32_t ID,
	const Input::GamepadButton& gamepadButton
) const -> bool
{
	ID = Math::clamp(ID, 0, Input::GAMEPADS_CONNECTED_MAX - 1);
	const auto& gamepadState = m_pWindow->GetInput().GetGamepadState(ID);
	return gamepadState.IsButtonPressed(gamepadButton);
}

auto Arg::Gameplay::GameInput::GetGamepadButtonReleased(
	int32_t ID,
	const Input::GamepadButton& gamepadButton
) const -> bool
{
	ID = Math::clamp(ID, 0, Input::GAMEPADS_CONNECTED_MAX - 1);
	const auto& gamepadState = m_pWindow->GetInput().GetGamepadState(ID);
	return gamepadState.IsButtonReleased(gamepadButton);
}

auto Arg::Gameplay::GameInput::GetGamepadButtonDown(
	int32_t ID,
	const Input::GamepadButton& gamepadButton
) const -> bool
{
	ID = Math::clamp(ID, 0, Input::GAMEPADS_CONNECTED_MAX - 1);
	const auto& gamepadState = m_pWindow->GetInput().GetGamepadState(ID);
	return gamepadState.IsButtonDown(gamepadButton);
}

auto Arg::Gameplay::GameInput::GetGamepadAxis(
	int32_t ID,
	const Input::GamepadAxis& gamepadAxis
) const -> float
{
	ID = Math::clamp(ID, 0, Input::GAMEPADS_CONNECTED_MAX - 1);
	const auto& gamepadState = m_pWindow->GetInput().GetGamepadState(ID);
	return gamepadState.GetAxis(gamepadAxis);
}

auto Arg::Gameplay::GameInput::GetGamepadAxis2D(
	int32_t ID,
	const Input::GamepadAxis2D& gamepadAxis
) const -> Vec2
{
	ID = Math::clamp(ID, 0, Input::GAMEPADS_CONNECTED_MAX - 1);
	const auto& gamepadState = m_pWindow->GetInput().GetGamepadState(ID);
	return gamepadState.GetAxis2D(gamepadAxis);
}
