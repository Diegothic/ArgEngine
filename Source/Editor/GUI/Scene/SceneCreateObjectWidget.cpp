#include "SceneCreateObjectWidget.h"

#include "Gameplay/Scene/Scene.h"


Arg::SceneCreateObjectWidget::SceneCreateObjectWidget(Vec2 position, Vec2 size, Scene* scene)
	: EditorWidget(position, size),
	m_pScene(scene)
{

}

void Arg::SceneCreateObjectWidget::VOnDraw()
{
	const float width = GetSize().x;
	if (ImGui::Button("Empty", ImVec2(width, 36.0f)))
	{
		m_pScene->CreateGameObject("New Object");
	}
}
