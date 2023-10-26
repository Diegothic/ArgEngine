#pragma once

#include <map>

#include "MouseButtons.h"
#include "CursorModes.h"
#include "Math/MathTypes.h"

namespace Arg
{
	class MouseState
	{
	public:
		MouseState();
		~MouseState() = default;

		bool IsButtonPressed(MouseButton button, int mods = 0) const;
		bool IsButtonDown(MouseButton button) const;
		bool IsButtonUp(MouseButton button) const;
		bool IsButtonReleased(MouseButton button) const;

		bool IsButtonPressed(int button, int mods = 0) const;
		bool IsButtonDown(int button) const;
		bool IsButtonUp(int button) const;
		bool IsButtonReleased(int button) const;

		Vec2 GetPosition() const;
		Vec2 GetPositionDelta() const;
		double GetScrollDelta() const;
		const CursorMode& GetCursorMode() const;

		void OnButtonPressed(int button);
		void OnButtonReleased(int button);
		void OnModsChanged(int mods);
		void OnPositionChanged(Vec2 position);
		void OnScrollChanged(double scroll);
		void OnCursorModeChanged(CursorMode cursorMode);
		void Update();

	private:
		bool GetButtonDownState(int button) const;
		bool GetLastButtonDownState(int button) const;

	private:
		Vec2 m_Position;
		Vec2 m_LastPosition;
		std::map<int, bool> m_ButtonDownState;
		std::map<int, bool> m_LastButtonDownState;
		double m_Scroll;
		double m_LastScroll;
		int m_Mods;
		CursorMode m_CursorMode;
	};
}
