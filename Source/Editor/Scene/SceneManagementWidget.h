#pragma once

#include <imgui/imgui.h>

#include "Editor/EditorWidget.h"
#include "SceneHierarchyWidget.h"
#include "SceneCreateObjectWidget.h"
#include "Editor/Inspector/Inpector.h"
#include "Gameplay/Scene/Scene.h"

namespace Arg
{
	class SceneManagementWidget : public EditorWidget
	{
	public:
		SceneManagementWidget(Vec2 position, Vec2 size, Scene* scene, Inspector* inspector)
			: EditorWidget(position, size),
			m_pScene(scene),
			m_pInspector(inspector)
		{
			m_HierarchyWidget = NewBox<SceneHierarchyWidget>(position, size, m_pScene, m_pInspector);
			m_CreateObjectWidget = NewBox<SceneCreateObjectWidget>(position, size, m_pScene);
		}

		virtual void Draw() override
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

		virtual void OnResized(Vec2 newPosition, Vec2 newSize) override
		{
			m_HierarchyWidget->Resize(newPosition, newSize);
			m_CreateObjectWidget->Resize(newPosition, newSize);
		}

	private:
		Scene* m_pScene = nullptr;
		Inspector* m_pInspector = nullptr;
		Box<EditorWidget> m_HierarchyWidget = nullptr;
		Box<EditorWidget> m_CreateObjectWidget = nullptr;
	};
}
