#include "SceneHierarchyWidget.h"

#include "Editor/Inspector.h"
#include "Gameplay/Scene/Scene.h"
#include "Gameplay/Scene/GameObject/GameObject.h"


Arg::SceneHierarchyWidget::SceneHierarchyWidget(Vec2 position, Vec2 size, Scene* scene, Inspector* inspector)
	: EditorWidget(position, size),
	m_pScene(scene),
	m_pInspector(inspector)
{

}

void Arg::SceneHierarchyWidget::VOnDraw()
{
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

void Arg::SceneHierarchyWidget::SelectGameObject(uint64_t ID) const
{
	m_pInspector->SetInspectedItem(ID);
	m_pScene->SetSelectedGameObject(m_pInspector->GetInspectedItemID());
}

void Arg::SceneHierarchyWidget::DrawTransformWithChildren(GameObject* gameObject)
{
	ImGui::PushID(static_cast<int>(gameObject->GetID()));
	ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow
		| ImGuiTreeNodeFlags_OpenOnDoubleClick
		| ImGuiTreeNodeFlags_FramePadding;
	if (gameObject->GetChildrenCount() < 1)
	{
		nodeFlags |= ImGuiTreeNodeFlags_Leaf;
	}

	if (gameObject->GetID() == m_pInspector->GetInspectedItemID())
	{
		nodeFlags |= ImGuiTreeNodeFlags_Selected;
	}

	const bool isOpen = ImGui::TreeNodeEx(gameObject->GetName().c_str(), nodeFlags);
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

void Arg::SceneHierarchyWidget::OnItemDropped(uint64_t ID, uint64_t targetID) const
{
	if (ID == targetID)
	{
		return;
	}

	SelectGameObject(targetID);
	m_pScene->ChangeParentGameObject(ID, targetID);
}