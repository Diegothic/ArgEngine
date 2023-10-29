#include "TransformWidget.h"

#include "Gameplay/Scene/GameObject/GameObject.h"
#include "Gameplay/Scene/GameObject/Transform.h"

Arg::TransformWidget::TransformWidget(GameObject* gameObject)
	: EditorWidget(Vec2(0.0f), Vec2(1.0f)),
	m_pGameObject(gameObject)
{

}

void Arg::TransformWidget::VOnDraw()
{
	const Vec2 size = GetSize();

	if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
	{
		const Transform& transform = m_pGameObject->GetTransform();
		const Vec3& position = transform.GetLocalPosition();
		const Vec3& rotation = transform.GetLocalRotationEuler();
		const Vec3& scale = transform.GetLocalScale();

		Vec3 newPosition = position;
		ImGui::Text("Position");
		ImGui::SameLine();
		ImGui::SetCursorPosX(size.x - (size.x * 0.36f) - 110.0f);
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);

		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
		ImGui::Text("X");
		ImGui::PopStyleColor();
		ImGui::SameLine();
		ImGui::SetNextItemWidth(size.x * 0.12f);
		ImGui::DragFloat("##PosX", &newPosition[0], 0.1f);

		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
		ImGui::Text("Y");
		ImGui::PopStyleColor();
		ImGui::SameLine();
		ImGui::SetNextItemWidth(size.x * 0.12f);
		ImGui::DragFloat("##PosY", &newPosition[1], 0.1f);

		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 0, 255, 255));
		ImGui::Text("Z");
		ImGui::PopStyleColor();
		ImGui::SameLine();
		ImGui::SetNextItemWidth(size.x * 0.12f);
		ImGui::DragFloat("##PosZ", &newPosition[2], 0.1f);

		ImGui::SameLine();
		if (ImGui::Button("R##Pos"))
		{
			newPosition = Vec3(0.0f);
		}

		for (int i = 0; i < 3; i++)
		{
			if (newPosition[i] == position[i])
			{
				continue;
			}

			m_pGameObject->SetLocalPosition(newPosition);
		}

		Vec3 newRotation = rotation;
		ImGui::Text("Rotation");
		ImGui::SameLine();
		ImGui::SetCursorPosX(size.x - (size.x * 0.36f) - 110.0f);
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);

		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
		ImGui::Text("X");
		ImGui::PopStyleColor();
		ImGui::SameLine();
		ImGui::SetNextItemWidth(size.x * 0.12f);
		ImGui::DragFloat("##RotX", &newRotation[0]);

		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
		ImGui::Text("Y");
		ImGui::PopStyleColor();
		ImGui::SameLine();
		ImGui::SetNextItemWidth(size.x * 0.12f);
		ImGui::DragFloat("##RotY", &newRotation[1]);

		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 0, 255, 255));
		ImGui::Text("Z");
		ImGui::PopStyleColor();
		ImGui::SameLine();
		ImGui::SetNextItemWidth(size.x * 0.12f);
		ImGui::DragFloat("##RotZ", &newRotation[2]);

		ImGui::SameLine();
		if (ImGui::Button("R##Rot"))
		{
			newRotation = Vec3(0.0f);
		}

		for (int i = 0; i < 3; i++)
		{
			if (newRotation[i] == rotation[i])
			{
				continue;
			}

			m_pGameObject->SetLocalRotation(newRotation);
		}


		Vec3 newScale = scale;
		ImGui::Text("Scale");


		ImGui::SameLine();
		static bool scaleProportionally = false;
		ImGui::Checkbox("##Prop", &scaleProportionally);

		ImGui::SameLine();
		ImGui::SetCursorPosX(size.x - (size.x * 0.36f) - 110.0f);
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);

		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
		ImGui::Text("X");
		ImGui::PopStyleColor();
		ImGui::SameLine();
		ImGui::SetNextItemWidth(size.x * 0.12f);
		ImGui::DragFloat("##ScaX", &newScale[0], 0.1f);

		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
		ImGui::Text("Y");
		ImGui::PopStyleColor();
		ImGui::SameLine();
		ImGui::SetNextItemWidth(size.x * 0.12f);
		ImGui::DragFloat("##ScaY", &newScale[1], 0.1f);

		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 0, 255, 255));
		ImGui::Text("Z");
		ImGui::PopStyleColor();
		ImGui::SameLine();
		ImGui::SetNextItemWidth(size.x * 0.12f);
		ImGui::DragFloat("##ScaZ", &newScale[2], 0.1f);

		ImGui::SameLine();
		if (ImGui::Button("R##Sca"))
		{
			scaleProportionally = false;
			newScale = Vec3(1.0f);
		}

		for (int i = 0; i < 3; i++)
		{
			if (newScale[i] == scale[i])
			{
				continue;
			}

			if (scaleProportionally)
			{
				float proportion = 1.0f;
				int proportionIndex = 0;
				for (int j = 0; j < 3; j++)
				{
					if (newScale[j] != scale[j])
					{
						proportionIndex = j;
						proportion = newScale[j] / (scale[j] == 0.0f ? 1.0f : scale[j]);
						break;
					}
				}

				for (int j = 0; j < 3; j++)
				{
					newScale[j] = (scale[j] == 0.0f ? newScale[proportionIndex] : scale[j] * proportion);
				}
			}

			m_pGameObject->SetLocalScale(newScale);
		}

	}

}
