#pragma once

#include <arg_pch.hpp>

#include "KeyboardState.hpp"
#include "MouseButtons.hpp"
#include "MouseState.hpp"
#include "GamepadState.hpp"

namespace Arg
{
	namespace Input
	{
		class Input
		{
		public:
			Input() = default;
			~Input() = default;

			void Initialize();
			void PullEvents();

			auto GetKeyboardState() const-> const KeyboardState&;
			auto GetMouseState() const-> const MouseState&;
			auto GetGamepadState(int id) const-> const GamepadState&;

			void OnKeyPressed(int key, int mods);
			void OnKeyReleased(int key);
			void OnCursorModeChanged(CursorMode mode);
			void OnMouseButtonPressed(int button, int mods);
			void OnMouseButtonReleased(int button);
			void OnMousePositionChanged(Vec2 position);
			void OnMouseScrollChanged(double scroll);
			void OnGamepadConnected(int id);
			void OnGamepadDisconnected(int id);

		private:
			void PrePullEvents();
			void PostPullEvents();

		private:
			KeyboardState m_KeyboardState;
			MouseState m_MouseState;
			std::unordered_map<int, GamepadState> m_GamepadStateForID;
		};
	}
}
