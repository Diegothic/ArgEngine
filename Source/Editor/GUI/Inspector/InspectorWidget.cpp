#include "InspectorWidget.h"

#include "Editor/Inspector.h"
#include "Editor/GUI/Scene/TransformWidget.h"
#include "Editor/GUI/Scene/Component/ComponentWidget.h"
#include "Gameplay/Scene/Scene.h"
#include "Gameplay/Scene/Component/Component.h"
#include "Gameplay/Scene/Component/StaticMeshComponent.h"
#include "Gameplay/Scene/GameObject/GameObject.h"

Arg::InspectorWidget::InspectorWidget(Vec2 position, Vec2 size, Inspector* inspector, Scene* scene)
	: EditorWidget(position, size),
	m_pInspector(inspector),
	m_pScene(scene)
{
}

void Arg::InspectorWidget::VOnDraw()
{
	const Vec2 position = GetPosition();
	const Vec2 size = GetSize();

	ImGui::SetNextWindowPos(ImVec2(position.x, position.y));
	ImGui::SetNextWindowSize(ImVec2(size.x, size.y));
	ImGui::Begin("Inspector",
		nullptr,
		ImGuiWindowFlags_NoMove
		| ImGuiWindowFlags_NoResize
		| ImGuiWindowFlags_NoCollapse
		| ImGuiWindowFlags_NoTitleBar
	);

	if (ImGui::BeginTabBar("Tabs"))
	{
		if (ImGui::BeginTabItem("Inspector"))
		{
			if (m_pInspector->GetInspectedItemID() != 0)
			{
				GameObject* selectedObject = m_pScene->FindGameObject(m_pInspector->GetInspectedItemID());
				if (selectedObject != nullptr)
				{
					DrawGameObjectInspector(selectedObject);
				}
			}
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	ImGui::End();
}

void Arg::InspectorWidget::VOnResized(Vec2 newPosition, Vec2 newSize)
{

}

void Arg::InspectorWidget::DrawGameObjectInspector(GameObject* gameObject) const
{
	const Vec2 size = GetSize();

	ImGui::PushID(static_cast<int>(gameObject->GetID()));
	char nameBuffer[512];
	strcpy_s(nameBuffer, gameObject->GetName().c_str());
	ImGui::InputText("##Name",
		nameBuffer,
		512,
		ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_CallbackAlways,
		[](ImGuiInputTextCallbackData* data) -> int
		{
			if (data->BufTextLen < 1)
			{
				return 0;
			}

			GameObject* gameObject = (GameObject*)data->UserData;
			const std::string newName(data->Buf);
			gameObject->SetName(newName);

			return 1;
		},
		gameObject
	);

	ImGui::SameLine();
	ImGui::SetCursorPosX(size.x - 50.0f);
	if (ImGui::Button("X"))
	{
		m_pScene->DestroyGameObject(gameObject->GetID());
	}

	ImGui::Text("ID: %llu", gameObject->GetID());

	TransformWidget transformWidget(gameObject);
	transformWidget.Resize(Vec2(0.0f), size);
	transformWidget.Draw();

	ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);

	for (size_t i = 0; i < gameObject->GetComponentCount(); i++)
	{
		Component* component = gameObject->GetComponent(i);
		ImGui::PushID(static_cast<int>(component->GetID()));
		const Rc<ComponentWidget> componentWidget = component->CreateWidget();
		componentWidget->Resize(Vec2(0.0f), size);
		componentWidget->Draw();

		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
		ImGui::PopID();
	}

	if (ImGui::Button("Add Component", ImVec2(size.x, 36.0f)))
	{
		ImGui::OpenPopup("Add Component Menu");
	}

	if (ImGui::BeginPopup("Add Component Menu"))
	{
		if (ImGui::TreeNode("Visual"))
		{
			if (ImGui::Button("Static Mesh"))
			{
				m_pScene->CreateComponent<StaticMeshComponent>(gameObject->GetID());
				ImGui::CloseCurrentPopup();
			}

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Physics"))
		{
			ImGui::TreePop();
		}

		ImGui::EndPopup();
	}


	ImGui::PopID();
}
