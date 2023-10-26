#pragma once

#include <imgui/imgui.h>

#include "Inpector.h"
#include "Editor/EditorWidget.h"
#include "Editor/Scene/Component/TransformWidget.h"
#include "Editor/Scene/Component/ComponentWidget.h"
#include "Gameplay/Scene/Scene.h"

namespace Arg
{
	class InspectorWidget : public EditorWidget
	{
	public:
		InspectorWidget(Vec2 position, Vec2 size, Inspector* inspector, Scene* scene)
			: EditorWidget(position, size),
			m_pInspector(inspector),
			m_pScene(scene)
		{
		}

		virtual void Draw() override
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

		virtual void OnResized(Vec2 newPosition, Vec2 newSize) override
		{

		}

	private:
		void DrawGameObjectInspector(GameObject* gameObject) const
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
				const Rc<ComponentWidget> componentWidget = gameObject->GetComponent(i)->CreateWidget();
				componentWidget->Resize(Vec2(0.0f), size);
				componentWidget->Draw();

				ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
			}

			ImGui::PopID();
		}

	private:
		Inspector* m_pInspector = nullptr;
		Scene* m_pScene = nullptr;
	};
}
