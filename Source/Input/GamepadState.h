#pragma once

#include <map>

#include "Arg/Math.h"
#include "GamepadConstants.h"
#include "GamepadButtons.h"
#include "GamepadAxes.h"

namespace Arg
{
	class GamepadState
	{
	public:
		GamepadState(int id);
		~GamepadState() = default;

		int GetID() const { return m_ID; }

		bool IsButtonPressed(GamepadButton button) const;
		bool IsButtonDown(GamepadButton button) const;
		bool IsButtonUp(GamepadButton button) const;
		bool IsButtonReleased(GamepadButton button) const;

		float GetAxis(GamepadAxis axis) const;
		float GetAxisDelta(GamepadAxis axis) const;
		Vec2 GetAxis2D(GamepadAxis2D axis) const;
		Vec2 GetAxis2DDelta(GamepadAxis2D axis) const;

		void OnButtonPressed(int button);
		void OnButtonReleased(int button);
		void OnAxisChanged(int axis, float value);
		void Update();

	private:
		bool GetButtonDownState(int button) const;
		bool GetLastButtonDownState(int button) const;
		float GetAxisState(int axis) const;
		float GetLastAxisState(int axis) const;

	private:
		int m_ID;
		std::map<int, bool> m_ButtonDownState;
		std::map<int, bool> m_LastButtonDownState;
		std::map<int, float> m_AxisState;
		std::map<int, float> m_LastAxisState;
	};
}
