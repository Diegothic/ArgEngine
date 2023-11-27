#include <arg_pch.hpp>
#include "KeyboardState.hpp"

auto Arg::Input::KeyboardState::IsKeyPressed(KeyCode keyCode, int mods) const -> bool
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

auto Arg::Input::KeyboardState::IsKeyDown(KeyCode keyCode) const -> bool
{
	return GetKeyDownState(keyCode);
}

auto Arg::Input::KeyboardState::IsKeyUp(KeyCode keyCode) const -> bool
{
	return !GetKeyDownState(keyCode);
}

auto Arg::Input::KeyboardState::IsKeyReleased(KeyCode keyCode) const -> bool
{
	return !GetKeyDownState(keyCode) && GetLastKeyDownState(keyCode);
}

void Arg::Input::KeyboardState::OnKeyPressed(int key)
{
	m_KeyDownState[key] = true;
}

void Arg::Input::KeyboardState::OnKeyReleased(int key)
{
	m_KeyDownState[key] = false;
}

void Arg::Input::KeyboardState::OnModsChanged(int mods)
{
	m_Mods = mods;
}

void Arg::Input::KeyboardState::Update()
{
	m_LastKeyDownState = m_KeyDownState;
}

auto Arg::Input::KeyboardState::GetKeyDownState(int key) const -> bool
{
	if (!m_KeyDownState.contains(key))
	{
		return false;
	}

	return m_KeyDownState.at(key);
}

auto Arg::Input::KeyboardState::GetLastKeyDownState(int key) const -> bool
{
	if (!m_LastKeyDownState.contains(key))
	{
		return false;
	}

	return m_LastKeyDownState.at(key);
}
