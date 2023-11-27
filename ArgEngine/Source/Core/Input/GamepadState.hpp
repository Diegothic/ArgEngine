#pragma once

#include <arg_pch.hpp>

#include "Core/Math/Math.hpp"

#include "GamepadConstants.hpp"
#include "GamepadButtons.hpp"
#include "GamepadAxes.hpp"

namespace Arg
{
	namespace Input
	{
		class GamepadState
		{
		public:
			GamepadState() = default;
			GamepadState(int id);
			~GamepadState() = default;

			auto GetID() const -> int { return m_ID; }

			auto IsConnected() const -> bool { return m_bIsConnected; }
			auto IsButtonPressed(GamepadButton button) const -> bool;
			auto IsButtonDown(GamepadButton button) const -> bool;
			auto IsButtonUp(GamepadButton button) const -> bool;
			auto IsButtonReleased(GamepadButton button) const -> bool;

			auto GetAxis(GamepadAxis axis) const -> float;
			auto GetAxisDelta(GamepadAxis axis) const -> float;
			auto GetAxis2D(GamepadAxis2D axis) const->Vec2;
			auto GetAxis2DDelta(GamepadAxis2D axis) const->Vec2;

			void OnGamepadConnected() { m_bIsConnected = true; }
			void OnGamepadDisconnected() { m_bIsConnected = false; }
			void OnButtonPressed(int button);
			void OnButtonReleased(int button);
			void OnAxisChanged(int axis, float value);
			void Update();

		private:
			auto GetButtonDownState(int button) const -> bool;
			auto GetLastButtonDownState(int button) const -> bool;
			auto GetAxisState(int axis) const -> float;
			auto GetLastAxisState(int axis) const -> float;

		private:
			int m_ID = 0;
			bool m_bIsConnected = false;
			std::unordered_map<int, bool> m_ButtonDownState;
			std::unordered_map<int, bool> m_LastButtonDownState;
			std::unordered_map<int, float> m_AxisState;
			std::unordered_map<int, float> m_LastAxisState;
		};
	}
}
