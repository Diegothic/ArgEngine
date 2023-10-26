#pragma once

#include <imgui/imgui.h>

#include "Editor/EditorWidget.h"
#include "Gameplay/Scene/Scene.h"

namespace Arg
{
	class SceneCreateObjectWidget : public EditorWidget
	{
	public:
		SceneCreateObjectWidget(Vec2 position, Vec2 size, Scene* scene)
			: EditorWidget(position, size),
			m_pScene(scene)
		{

		}

		virtual void Draw() override
		{
			const float width = GetSize().x;
			if (ImGui::Button("Empty", ImVec2(width, 36.0f)))
			{
				m_pScene->CreateGameObject("New Object");
			}
		}

	private:
		Scene* m_pScene = nullptr;
	};
}
