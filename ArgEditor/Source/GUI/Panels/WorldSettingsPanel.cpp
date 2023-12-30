#include <arg_pch.hpp>
#include "WorldSettingsPanel.hpp"

#include <imgui/imgui.h>

#include "Editor.hpp"

void Arg::Editor::GUI::WorldSettingsPanel::OnInitialize(const EditorGUIContext& context)
{
}

void Arg::Editor::GUI::WorldSettingsPanel::OnDraw(const EditorGUIContext& context)
{
	Editor* pEditor = context.pEditor;
	auto& pGameEngine = pEditor->GetGameEngine();

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.0f, 10.0f));
	const bool isOpen = ImGui::Begin("World Settings", &GetIsOpened());
	ImGui::PopStyleVar();
	if (isOpen)
	{
		if (!pGameEngine->IsWorldLoaded())
		{
			ImGui::Text("No world opened!");
			ImGui::End();
			return;
		}

		auto& pWorld = pGameEngine->GetLoadedWorld();

		const bool isSunlightHeaderOpen = ImGui::CollapsingHeader(
			"##SunlightSettingsHeader",
			ImGuiTreeNodeFlags_DefaultOpen
			| ImGuiTreeNodeFlags_OpenOnArrow
			| ImGuiTreeNodeFlags_OpenOnDoubleClick
			| ImGuiTreeNodeFlags_FramePadding
			| ImGuiTreeNodeFlags_AllowOverlap
		);

		ImGui::SameLine(50.0f);
		ImGui::Text("Sunlight");

		if (isSunlightHeaderOpen)
		{
			if (ImGui::BeginTable(
				"##SunlightSettingsTable",
				2,
				ImGuiTableFlags_BordersInnerV
				| ImGuiTableFlags_BordersOuter
				| ImGuiTableFlags_NoSavedSettings
				| ImGuiTableFlags_SizingFixedFit
			))
			{
				{
					ImGui::TableNextColumn();
					ImGui::Dummy(ImVec2(100.0f, 0.0f));

					ImGui::Text("Color");

					ImGui::TableNextColumn();

					Vec3 color = pWorld->GetSunlightColor();
					ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - 165.0f * 1.0f);
					ImGui::ColorEdit3("##SunlightColor", Math::ValuePtr(color));
					if (color != pWorld->GetSunlightColor())
					{
						pWorld->SetSunlightColor(color);
					}
				}

				{
					ImGui::TableNextColumn();
					ImGui::Dummy(ImVec2(100.0f, 0.0f));

					ImGui::Text("Direction");

					ImGui::TableNextColumn();

					Vec3 direction = pWorld->GetSunlightDirection();
					ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - 165.0f * 1.0f);
					ImGui::InputFloat3("##SunlightDirection", Math::ValuePtr(direction));
					if (direction != pWorld->GetSunlightDirection())
					{
						pWorld->SetSunlightDirection(direction);
					}
				}

				{
					ImGui::TableNextColumn();
					ImGui::Dummy(ImVec2(100.0f, 0.0f));

					ImGui::Text("Intensity");

					ImGui::TableNextColumn();

					float intensity = pWorld->GetSunlightIntensity();
					ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - 165.0f * 1.0f);
					ImGui::InputFloat("##SunlightIntensity", &intensity);
					if (intensity != pWorld->GetSunlightIntensity())
					{
						pWorld->SetSunlightIntensity(intensity);
					}
				}

				{
					ImGui::TableNextColumn();
					ImGui::Dummy(ImVec2(100.0f, 0.0f));

					ImGui::Text("Cast Shadows");

					ImGui::TableNextColumn();

					bool bCastShadows = pWorld->GetSunlightCastsShadows();
					ImGui::Checkbox("##SunlightCastShadows", &bCastShadows);
					if (bCastShadows != pWorld->GetSunlightCastsShadows())
					{
						pWorld->SetSunlightCastsShadows(bCastShadows);
					}
				}
			}

			ImGui::EndTable();
		}
	}

	ImGui::End();
}
