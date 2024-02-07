#include <arg_pch.hpp>
#include "WorldSettingsPanel.hpp"

#include <imgui/ImGui.hpp>

#include "Editor.hpp"
#include "GUI/Elements/Properties.hpp"

void Arg::Editor::GUI::WorldSettingsPanel::OnInitialize(const EditorGUIContext& context)
{
}

void Arg::Editor::GUI::WorldSettingsPanel::OnDraw(const EditorGUIContext& context)
{
	Editor* pEditor = context.pEditor;
	auto& pGameEngine = pEditor->GetGameEngine();
	const bool isProjectOpended = pEditor->IsProjectOpened();
	auto& pResourceCache = isProjectOpended
		                       ? pEditor->GetProject()->GetResourceCache()
		                       : pEditor->GetResourceCache();
	auto& pContent = isProjectOpended
		                 ? pEditor->GetProject()->GetContent()
		                 : pEditor->GetContent();

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.0f, 10.0f));
	const bool isOpen = ImGui::Begin("World Settings", &GetIsOpened());
	ImGui::PopStyleVar();
	if (isOpen)
	{
		if (!pGameEngine->IsWorldLoaded())
		{
			ImGui::Text("No world open!");
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

		const bool isBackgroundHeaderOpen = ImGui::CollapsingHeader(
			"##BackgroundSettingsHeader",
			ImGuiTreeNodeFlags_DefaultOpen
			| ImGuiTreeNodeFlags_OpenOnArrow
			| ImGuiTreeNodeFlags_OpenOnDoubleClick
			| ImGuiTreeNodeFlags_FramePadding
			| ImGuiTreeNodeFlags_AllowOverlap
		);

		ImGui::SameLine(50.0f);
		ImGui::Text("Background");

		if (isBackgroundHeaderOpen)
		{
			if (ImGui::BeginTable(
				"##BackgroundSettingsTable",
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

					ImGui::Text("Type");

					ImGui::TableNextColumn();

					const char* backgroundTypes[] = {"Flat Color", "Skybox"};
					size_t currentType = pWorld->GetUsingSkybox() ? 1 : 0;
					ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - 165.0f);
					if (ImGui::BeginCombo("##BackgroundType", backgroundTypes[currentType]))
					{
						for (size_t i = 0; i < 2; i++)
						{
							const bool bIsSelected = currentType == i;
							if (ImGui::Selectable(backgroundTypes[i], bIsSelected))
							{
								currentType = i;
								pWorld->SetUsingSkybox(i == 1);
							}

							if (bIsSelected)
							{
								ImGui::SetItemDefaultFocus();
							}
						}

						ImGui::EndCombo();
					}

					switch (currentType)
					{
					case 0:
						{
							ImGui::TableNextColumn();
							ImGui::Dummy(ImVec2(100.0f, 0.0f));

							ImGui::Text("Color");

							ImGui::TableNextColumn();

							static Vec3 color = Vec3(1.0f);
							color = pWorld->GetBackgroundColor();
							ImGui::ColorEdit3(
								"##BackgroundColor",
								Math::ValuePtr(color),
								ImGuiColorEditFlags_Float
								| ImGuiColorEditFlags_HDR
							);
							if (color != pWorld->GetBackgroundColor())
							{
								pWorld->SetBackgroundColor(color);
							}

							break;
						}
					case 1:
						{
							for (size_t i = 0; i < 6; i++)
							{
								ImGui::PushID(static_cast<int32_t>(i));

								ImGui::TableNextColumn();
								ImGui::Dummy(ImVec2(100.0f, 0.0f));

								const std::string label = std::format("Texture {}", i);
								ImGui::Text(label.c_str());

								ImGui::TableNextColumn();

								const auto texture = pWorld->GetSkyboxTexture(i);
								ResourceHandleProperty(
									"##TextureHandle",
									Vec2(ImGui::GetWindowWidth() - 160.0f, 25.0f),
									texture.IsValid() ? texture.Get()->GetName().c_str() : nullptr,
									[&](GUID droppedResourceID)
									{
										const auto& resource = pResourceCache->GetResource(droppedResourceID);
										if (resource->GetType() == Content::ResourceType::ResourceTypeTexture)
										{
											pWorld->SetSkyboxTexture(
												i,
												pResourceCache
												->CreateHandle<Content::TextureResource>(
													droppedResourceID
												)
											);
										}
									},
									[&]
									{
										pWorld->SetSkyboxTexture(
											i,
											pResourceCache->CreateHandle<Content::TextureResource>(GUID::Empty)
										);
									}
								);

								ImGui::PopID();
							}

							break;
						}
					}
				}
			}

			ImGui::EndTable();
		}

		const bool isGameplayHeaderOpen = ImGui::CollapsingHeader(
			"##GameplaySettingsHeader",
			ImGuiTreeNodeFlags_DefaultOpen
			| ImGuiTreeNodeFlags_OpenOnArrow
			| ImGuiTreeNodeFlags_OpenOnDoubleClick
			| ImGuiTreeNodeFlags_FramePadding
			| ImGuiTreeNodeFlags_AllowOverlap
		);

		ImGui::SameLine(50.0f);
		ImGui::Text("Gameplay");

		if (isGameplayHeaderOpen)
		{
			if (ImGui::BeginTable(
				"##GameplaySettingsTable",
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

					ImGui::Text("Main Camera");

					ImGui::TableNextColumn();

					const auto mainCamera = pWorld->GetMainCamera();

					const char* mainCameraName = nullptr;
					std::string name;
					if (mainCamera.IsValid())
					{
						name = std::format(
							"{}:{}",
							mainCamera.Get().GetOwner()->GetName(),
							mainCamera.Get().VGetName()
						);
						mainCameraName = name.c_str();
					}

					ActorComponentHandleProperty(
						"##MainCameraHandle",
						Vec2(ImGui::GetWindowWidth() - 160.0f, 25.0f),
						mainCameraName,
						[&](GUID droppedActorID)
						{
							if (!pWorld->HasActor(droppedActorID))
							{
								return;
							}

							const auto& actor = pWorld->GetActor(droppedActorID);
							if (!actor.HasComponent(Gameplay::CameraComponent::COMPONENT_ID))
							{
								return;
							}

							pWorld->SetMainCamera(
								Gameplay::ActorComponentHandle<Gameplay::CameraComponent>(
									pWorld.get(),
									actor.GetID(),
									Gameplay::CameraComponent::COMPONENT_ID
								)
							);
						},
						[&]
						{
							pWorld->SetMainCamera(
								Gameplay::ActorComponentHandle<Gameplay::CameraComponent>()
							);
						}
					);
				}
			}

			ImGui::EndTable();
		}

		const bool isPhysicsHeaderOpen = ImGui::CollapsingHeader(
			"##PhysicsSettingsHeader",
			ImGuiTreeNodeFlags_DefaultOpen
			| ImGuiTreeNodeFlags_OpenOnArrow
			| ImGuiTreeNodeFlags_OpenOnDoubleClick
			| ImGuiTreeNodeFlags_FramePadding
			| ImGuiTreeNodeFlags_AllowOverlap
		);

		ImGui::SameLine(50.0f);
		ImGui::Text("Physics");

		if (isPhysicsHeaderOpen)
		{
			if (ImGui::BeginTable(
				"##PhysicsSettingsTable",
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

					ImGui::Text("Gravity");

					ImGui::TableNextColumn();

					Vec3 gravity = pWorld->GetGravity();
					ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - 165.0f);
					ImGui::InputFloat3("##Gravity", Math::ValuePtr(gravity));
					if (gravity != pWorld->GetGravity())
					{
						pWorld->SetGravity(gravity);
					}
				}
			}

			ImGui::EndTable();
		}
	}

	ImGui::End();
}
