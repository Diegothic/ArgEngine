#pragma once

#include <arg_pch.hpp>

#include "Core/Input/Input.hpp"
#include "Core/Window/Window.hpp"

namespace Arg
{
	namespace Gameplay
	{
		class GameInput
		{
		public:
			GameInput() = default;

		public:
			void Initialize(Window* pWindow);

			void BeginPlay();
			void Tick(float deltaTime, bool bIsGameFocused);

		public:
			auto GetKeyPressed(const Input::KeyCode& keyCode) const -> bool;
			auto GetKeyReleased(const Input::KeyCode& keyCode) const -> bool;
			auto GetKeyDown(const Input::KeyCode& keyCode) const -> bool;

			auto GetMousePos() const -> Vec2;
			auto GetMouseDelta() const -> Vec2;

			auto GetMouseButtonPressed(const Input::MouseButton& mouseButton) const -> bool;
			auto GetMouseButtonReleased(const Input::MouseButton& mouseButton) const -> bool;
			auto GetMouseButtonDown(const Input::MouseButton& mouseButton) const -> bool;

			auto IsGamepadConnected(int32_t ID) const -> bool;
			
			auto GetGamepadButtonPressed(int32_t ID, const Input::GamepadButton& gamepadButton) const -> bool;
			auto GetGamepadButtonReleased(int32_t ID, const Input::GamepadButton& gamepadButton) const -> bool;
			auto GetGamepadButtonDown(int32_t ID, const Input::GamepadButton& gamepadButton) const -> bool;

			auto GetGamepadAxis(int32_t ID, const Input::GamepadAxis& gamepadAxis) const -> float;
			auto GetGamepadAxis2D(int32_t ID, const Input::GamepadAxis2D& gamepadAxis) const -> Vec2;

		private:
			Window* m_pWindow = nullptr;
			bool m_bIsGameFocused = false;
			Vec2 m_ScreenSize = Vec2(1.0f);
			Vec2 m_CursorPos = Vec2(0.0f);
			Vec2 m_CursorDelta = Vec2(0.0f);
		};
	}
}
