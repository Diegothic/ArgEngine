#include "KeyboardState.h"

Arg::KeyboardState::KeyboardState()
	: m_Mods()
{
}

bool Arg::KeyboardState::IsKeyPressed(KeyCode keyCode, int mods) const
{
	if (mods == 0 && m_Mods > 0)
	{
		return false;
	}

	if (mods > 0 && mods != m_Mods)
	{
		return false;
	}

	return GetKeyDownState(keyCode) && !GetLastKeyDownState(keyCode);
}

bool Arg::KeyboardState::IsKeyDown(KeyCode keyCode) const
{
	return GetKeyDownState(keyCode);
}

bool Arg::KeyboardState::IsKeyUp(KeyCode keyCode) const
{
	return !GetKeyDownState(keyCode);
}

bool Arg::KeyboardState::IsKeyReleased(KeyCode keyCode) const
{
	return !GetKeyDownState(keyCode) && GetLastKeyDownState(keyCode);
}

void Arg::KeyboardState::OnKeyPressed(int key)
{
	m_KeyDownState[key] = true;
}

void Arg::KeyboardState::OnKeyReleased(int key)
{
	m_KeyDownState[key] = false;
}

void Arg::KeyboardState::OnModsChanged(int mods)
{
	m_Mods = mods;
}

void Arg::KeyboardState::Update()
{
	m_LastKeyDownState = m_KeyDownState;
}

bool Arg::KeyboardState::GetKeyDownState(int key) const
{
	if (!m_KeyDownState.contains(key))
	{
		return false;
	}

	return m_KeyDownState.at(key);
}

bool Arg::KeyboardState::GetLastKeyDownState(int key) const
{
	if (!m_LastKeyDownState.contains(key))
	{
		return false;
	}

	return m_LastKeyDownState.at(key);
}
