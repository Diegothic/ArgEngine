#include <arg_pch.hpp>
#include "EditorGUIPanel.hpp"

#include "EditorGUI.hpp"

void Arg::Editor::GUI::EditorGUIPanel::Initialize(const EditorGUIContext& context)
{
	OnInitialize(context);
}

void Arg::Editor::GUI::EditorGUIPanel::Open()
{
	m_bIsOpened = true;
}

auto Arg::Editor::GUI::EditorGUIPanel::IsOpened() const -> const bool&
{
	return m_bIsOpened;
}

void Arg::Editor::GUI::EditorGUIPanel::Draw(const EditorGUIContext& context)
{
	if (!m_bIsOpened)
	{
		return;
	}

	OnDraw(context);
}

auto Arg::Editor::GUI::EditorGUIPanel::GetIsOpened() -> bool&
{
	return m_bIsOpened;
}
