#pragma once

#include <arg_pch.hpp>

#include "KeyCodes.hpp"

namespace Arg
{
	namespace Input
	{
		class KeyboardState
		{
		public:
			KeyboardState() = default;
			~KeyboardState() = default;

			auto IsKeyPressed(KeyCode keyCode, int mods = 0) const -> bool;
			auto IsKeyDown(KeyCode keyCode) const -> bool;
			auto IsKeyUp(KeyCode keyCode) const -> bool;
			auto IsKeyReleased(KeyCode keyCode) const -> bool;

			void OnKeyPressed(int key);
			void OnKeyReleased(int key);
			void OnModsChanged(int mods);
			void Update();

		private:
			auto GetKeyDownState(int key) const -> bool;
			auto GetLastKeyDownState(int key) const -> bool;

		private:
			std::unordered_map<int, bool> m_KeyDownState;
			std::unordered_map<int, bool> m_LastKeyDownState;
			int m_Mods = 0;
		};
	}
}
