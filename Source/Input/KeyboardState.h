#pragma once

#include <map>

#include "KeyCodes.h"

namespace Arg
{
	class KeyboardState
	{
	public:
		KeyboardState() = default;
		~KeyboardState() = default;

		bool IsKeyPressed(KeyCode keyCode, int mods = 0) const;
		bool IsKeyDown(KeyCode keyCode) const;
		bool IsKeyUp(KeyCode keyCode) const;
		bool IsKeyReleased(KeyCode keyCode) const;

		void OnKeyPressed(int key);
		void OnKeyReleased(int key);
		void SetMods(int mods);
		void Update();

	private:
		bool GetKeyDownState(int key) const;
		bool GetLastKeyDownState(int key) const;

	private:
		std::map<int, bool> m_KeyDownState;
		std::map<int, bool> m_LastKeyDownState;
		int m_Mods;
	};
}
