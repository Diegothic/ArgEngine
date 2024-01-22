#include <arg_pch.hpp>
#include "ProjectSettingsPanel.hpp"

#include <imgui/ImGui.hpp>

#include "Editor.hpp"
#include "GUI/Elements/Properties.hpp"

void Arg::Editor::GUI::ProjectSettingsPanel::OnInitialize(
	const EditorGUIContext& context
)
{
}

void Arg::Editor::GUI::ProjectSettingsPanel::OnDraw(
	const EditorGUIContext& context
)
{
	Editor* pEditor = context.pEditor;
	auto& pGameEngine = pEditor->GetGameEngine();
	const bool isProjectOpened = pEditor->IsProjectOpened();
	auto& pResourceCache = isProjectOpened
		                       ? pEditor->GetProject()->GetResourceCache()
		                       : pEditor->GetResourceCache();
	auto& pContent = isProjectOpened
		                 ? pEditor->GetProject()->GetContent()
		                 : pEditor->GetContent();

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.0f, 10.0f));
	const bool isOpen = ImGui::Begin("Project Settings", &GetIsOpened());
	ImGui::PopStyleVar();
	if (isOpen)
	{
		if (!pEditor->IsProjectOpened())
		{
			ImGui::Text("No project open!");
			ImGui::End();
			return;
		}

		Project* pProject = pEditor->GetProject().get();

		const bool isGeneralHeaderOpen = ImGui::CollapsingHeader(
			"##GeneralSettingsHeader",
			ImGuiTreeNodeFlags_DefaultOpen
			| ImGuiTreeNodeFlags_OpenOnArrow
			| ImGuiTreeNodeFlags_OpenOnDoubleClick
			| ImGuiTreeNodeFlags_FramePadding
			| ImGuiTreeNodeFlags_AllowOverlap
		);

		ImGui::SameLine(50.0f);
		ImGui::Text("General");

		if (isGeneralHeaderOpen)
		{
			if (ImGui::BeginTable(
				"##GeneralSettingsTable",
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

					ImGui::Text("Name");

					ImGui::TableNextColumn();

					static bool doRename = false;
					static std::string newProjectName;
					static char buffer[1024];
					strcpy_s(buffer, pProject->GetName().c_str());
					ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - 165.0f);
					ImGui::InputText(
						"##ProjectNewName",
						buffer,
						1024,
						ImGuiInputTextFlags_CallbackAlways,
						[](ImGuiInputTextCallbackData* data) -> int32_t
						{
							if (data->BufTextLen < 1)
							{
								return 0;
							}

							if (ImGui::IsKeyDown(ImGuiKey_Enter))
							{
								newProjectName = data->Buf;
								doRename = true;
								return 1;
							}

							return 0;
						}
					);

					if (doRename)
					{
						doRename = false;
						pProject->SetName(newProjectName);
					}
				}
			}

			ImGui::EndTable();
		}

		const bool isMapsHeaderOpen = ImGui::CollapsingHeader(
			"##MapsSettingsHeader",
			ImGuiTreeNodeFlags_DefaultOpen
			| ImGuiTreeNodeFlags_OpenOnArrow
			| ImGuiTreeNodeFlags_OpenOnDoubleClick
			| ImGuiTreeNodeFlags_FramePadding
			| ImGuiTreeNodeFlags_AllowOverlap
		);

		ImGui::SameLine(50.0f);
		ImGui::Text("Default Maps");

		if (isMapsHeaderOpen)
		{
			if (ImGui::BeginTable(
				"##MapsSettingsTable",
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

					ImGui::Text("Editor Map");

					ImGui::TableNextColumn();

					const std::string editorMap = pProject->GetEditorMap();
					const auto mapHandle = pResourceCache->CreateHandle<Content::WorldResource>(editorMap);
					ResourceHandleProperty(
						"##EditorMapHandle",
						Vec2(ImGui::GetWindowWidth() - 160.0f, 25.0f),
						mapHandle.IsValid() ? mapHandle.Get()->GetName().c_str() : nullptr,
						[&](GUID droppedResourceID)
						{
							const auto& resource = pResourceCache->GetResource(droppedResourceID);
							if (resource->GetType() == Content::ResourceType::ResourceTypeWorld)
							{
								const std::string newMap = std::filesystem::relative(
									resource->GetFullPath(),
									pContent->GetRootFolder()->GetFullPath()
								).replace_extension("").string();

								pProject->SetEditorMap(
									newMap
								);
							}
						},
						[&]
						{
							pProject->SetEditorMap("");
						}
					);
				}

				{
					ImGui::TableNextColumn();
					ImGui::Dummy(ImVec2(100.0f, 0.0f));

					ImGui::Text("Game Map");

					ImGui::TableNextColumn();

					const std::string gameMap = pProject->GetGameMap();
					const auto mapHandle = pResourceCache->CreateHandle<Content::WorldResource>(gameMap);
					ResourceHandleProperty(
						"##GameMapHandle",
						Vec2(ImGui::GetWindowWidth() - 160.0f, 25.0f),
						mapHandle.IsValid() ? mapHandle.Get()->GetName().c_str() : nullptr,
						[&](GUID droppedResourceID)
						{
							const auto& resource = pResourceCache->GetResource(droppedResourceID);
							if (resource->GetType() == Content::ResourceType::ResourceTypeWorld)
							{
								const std::string newMap = std::filesystem::relative(
									resource->GetFullPath(),
									pContent->GetRootFolder()->GetFullPath()
								).replace_extension("").string();

								pProject->SetGameMap(
									newMap
								);
							}
						},
						[&]
						{
							pProject->SetGameMap("");
						}
					);
				}
			}

			ImGui::EndTable();
		}
	}

	ImGui::End();
}
