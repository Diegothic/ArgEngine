#pragma once

#include "imgui/imgui.h"

#include "Arg/Debug.h"
#include "Arg/Gameplay.h"

namespace Arg
{
	class SceneHierarchyWidget
	{
	public:
		SceneHierarchyWidget(Scene* scene)
			: m_pScene(scene)
		{

		}

		void OnGUI()
		{
			m_CanSelectObject = true;
			if (ImGui::CollapsingHeader("Scene", ImGuiTreeNodeFlags_DefaultOpen))
			{
				const GameObject* sceneRoot = m_pScene->GetRootObject();
				for (int i = 0; i < sceneRoot->GetChildrenCount(); i++)
				{
					DrawTransformWithChildren(sceneRoot->GetChild(i));
				}
			}

			if (ImGui::IsWindowHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
			{
				if (const ImGuiPayload* payload = ImGui::GetDragDropPayload())
				{
					if (payload->IsDataType("GameObject"))
					{
						GameObject* payload_data = (GameObject*)payload->Data;
						OnItemDropped(payload_data->GetID());
					}
				}
			}
		}

		uint64_t GetSelectedGameObjectID() const { return m_SelectedGameObjectID; }

	private:
		void SelectGameObject(uint64_t ID)
		{
			m_SelectedGameObjectID = ID;
			m_pScene->SetSelectedGameObject(m_SelectedGameObjectID);
		}

		void DrawTransformWithChildren(GameObject* gameObject)
		{
			ImGui::PushID(static_cast<int>(gameObject->GetID()));
			ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow
				| ImGuiTreeNodeFlags_OpenOnDoubleClick
				| ImGuiTreeNodeFlags_FramePadding;
			if (gameObject->GetChildrenCount() < 1)
			{
				nodeFlags |= ImGuiTreeNodeFlags_Leaf;
			}

			if (gameObject->GetID() == m_SelectedGameObjectID)
			{
				nodeFlags |= ImGuiTreeNodeFlags_Selected;
			}

			bool isOpen = ImGui::TreeNodeEx(gameObject->GetName().c_str(), nodeFlags);
			if (ImGui::IsItemClicked())
			{
				SelectGameObject(gameObject->GetID());
			}

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GameObject"))
				{
					IM_ASSERT(payload->DataSize == sizeof(GameObject));
					GameObject* payload_data = (GameObject*)payload->Data;
					OnItemDropped(payload_data->GetID(), gameObject->GetID());
				}
				ImGui::EndDragDropTarget();
			}

			if (ImGui::BeginDragDropSource())
			{
				ImGui::SetDragDropPayload("GameObject", gameObject, sizeof(GameObject));
				ImGui::Text(gameObject->GetName().c_str());
				ImGui::EndDragDropSource();
			}

			if (isOpen)
			{
				for (size_t i = 0; i < gameObject->GetChildrenCount(); i++)
				{
					DrawTransformWithChildren(gameObject->GetChild(i));
				}
				ImGui::TreePop();
			}

			ImGui::PopID();
		}

		void OnItemDropped(uint64_t ID, uint64_t targetID = 0)
		{
			if (ID == targetID)
			{
				return;
			}

			SelectGameObject(targetID);
			m_pScene->ChangeParentGameObject(ID, targetID);
		}

	private:
		Scene* m_pScene;
		uint64_t m_SelectedGameObjectID = 0;
		bool m_CanSelectObject = true;
	};
}
