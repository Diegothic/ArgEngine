#pragma once

#include <arg_pch.hpp>

#include "Core/Math/Math.hpp"

#include "MouseButtons.hpp"
#include "CursorModes.hpp"

namespace Arg
{
	namespace Input
	{
		class MouseState
		{
		public:
			MouseState() = default;
			~MouseState() = default;

			auto IsButtonPressed(MouseButton button, int mods = 0) const -> bool;
			auto IsButtonDown(MouseButton button) const -> bool;
			auto IsButtonUp(MouseButton button) const -> bool;
			auto IsButtonReleased(MouseButton button) const -> bool;

			auto IsButtonPressed(int button, int mods = 0) const -> bool;
			auto IsButtonDown(int button) const -> bool;
			auto IsButtonUp(int button) const -> bool;
			auto IsButtonReleased(int button) const -> bool;

			auto GetPosition() const->Vec2;
			auto GetPositionDelta() const->Vec2;
			auto GetScrollDelta() const -> double;
			auto GetCursorMode() const -> const CursorMode&;

			void OnButtonPressed(int button);
			void OnButtonReleased(int button);
			void OnModsChanged(int mods);
			void OnPositionChanged(Vec2 position);
			void OnScrollChanged(double scroll);
			void OnCursorModeChanged(CursorMode cursorMode);
			void Update();

		private:
			auto GetButtonDownState(int button) const -> bool;
			auto GetLastButtonDownState(int button) const -> bool;

		private:
			std::unordered_map<int, bool> m_ButtonDownState;
			std::unordered_map<int, bool> m_LastButtonDownState;
			int m_Mods = 0;
			Vec2 m_Position = Vec2(0.0f);
			Vec2 m_LastPosition = Vec2(0.0f);
			double m_Scroll = 0.0;
			double m_LastScroll = 0.0;
			CursorMode m_CursorMode = CursorMode::Normal;
		};
	}
}
