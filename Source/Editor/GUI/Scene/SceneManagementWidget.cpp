#include "SceneManagementWidget.h"

#include "SceneCreateObjectWidget.h"
#include "SceneHierarchyWidget.h"


Arg::SceneManagementWidget::SceneManagementWidget(Vec2 position, Vec2 size, Scene* scene, Inspector* inspector) : EditorWidget(position, size),
m_pScene(scene),
m_pInspector(inspector)
{
	m_HierarchyWidget = NewBox<SceneHierarchyWidget>(position, size, m_pScene, m_pInspector);
	m_CreateObjectWidget = NewBox<SceneCreateObjectWidget>(position, size, m_pScene);
}

void Arg::SceneManagementWidget::VOnDraw()
{
	const Vec2 position = GetPosition();
	const Vec2 size = GetSize();

	ImGui::SetNextWindowPos(ImVec2(position.x, position.y));
	ImGui::SetNextWindowSize(ImVec2(size.x, size.y));
	ImGui::Begin("SceneManagement",
		nullptr,
		ImGuiWindowFlags_NoMove
		| ImGuiWindowFlags_NoResize
		| ImGuiWindowFlags_NoCollapse
		| ImGuiWindowFlags_NoTitleBar
	);

	if (ImGui::BeginTabBar("Tabs"))
	{
		if (ImGui::BeginTabItem("Scene Hierarchy"))
		{
			m_HierarchyWidget->Draw();
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Create Object"))
		{
			m_CreateObjectWidget->Draw();
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	ImGui::End();
}

void Arg::SceneManagementWidget::VOnResized(Vec2 newPosition, Vec2 newSize)
{
	EditorWidget::VOnResized(newPosition, newSize);

	m_HierarchyWidget->Resize(newPosition, newSize);
	m_CreateObjectWidget->Resize(newPosition, newSize);
}
