#include <arg_pch.hpp>
#include "DetailsPanel.hpp"

#include <imgui/imgui.h>

#include "Editor.hpp"
#include "GUI/Elements/Properties.hpp"

void Arg::Editor::GUI::DetailsPanel::OnInitialize(const EditorGUIContext& context)
{
}

void Arg::Editor::GUI::DetailsPanel::OnDraw(const EditorGUIContext& context)
{
	Editor* pEditor = context.pEditor;
	auto& pContent = pEditor->GetContent();
	auto& pResourceCache = pEditor->GetResourceCache();
	auto& pGameEngine = pEditor->GetGameEngine();

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.0f, 10.0f));
	const bool isOpen = ImGui::Begin("Details", &GetIsOpened());
	ImGui::PopStyleVar();
	if (isOpen)
	{
		Gameplay::Actor* pSelectedActor = nullptr;
		if (pEditor->HasSelectedActor()
			&& pEditor->GetSelectedActor(pSelectedActor))
		{
			DrawActorDetails(context, pSelectedActor);
		}
	}
	ImGui::End();
}

void Arg::Editor::GUI::DetailsPanel::DrawActorDetails(
	const EditorGUIContext& context,
	Gameplay::Actor* actor
)
{
	Gameplay::GameWorld* pWorld = actor->GetWorld();
	Gameplay::ComponentRegistry* pComponentsRegistry = pWorld->GetComponentRegistry();
	Vec2 windowSize = Vec2(ImGui::GetWindowSize().x, ImGui::GetWindowSize().y);

	ImGui::PushID(static_cast<int>(actor->GetID()));

	const auto& actorName = actor->GetName();
	ImGui::Text(actorName.c_str());
	ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "\tID: %llu", actor->GetID());

	if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
	{
		const auto position = actor->GetLocalPosition();
		const auto rotation = actor->GetLocalRotation();
		const auto scale = actor->GetLocalScale();

		if (ImGui::BeginTable(
			"##TransformTable",
			2,
			ImGuiTableFlags_BordersInnerV
			| ImGuiTableFlags_BordersOuter
			| ImGuiTableFlags_NoSavedSettings
			| ImGuiTableFlags_SizingFixedFit
		))
		{
			ImGui::TableNextColumn();
			ImGui::Dummy(ImVec2(80.0f, 0.0f));

			Vec3 newPosition = position;
			ImGui::Text("Position");

			ImGui::TableNextColumn();

			ImGui::Dummy(ImVec2(10.0f, 0.0f));
			ImGui::SameLine();
			ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
			ImGui::Text("X");
			ImGui::PopStyleColor();
			ImGui::SameLine();
			ImGui::SetNextItemWidth(windowSize.x * 0.12f);
			ImGui::DragFloat("##PosX", &newPosition[0], 0.1f);

			ImGui::SameLine();
			ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
			ImGui::Text("Y");
			ImGui::PopStyleColor();
			ImGui::SameLine();
			ImGui::SetNextItemWidth(windowSize.x * 0.12f);
			ImGui::DragFloat("##PosY", &newPosition[1], 0.1f);

			ImGui::SameLine();
			ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 0, 255, 255));
			ImGui::Text("Z");
			ImGui::PopStyleColor();
			ImGui::SameLine();
			ImGui::SetNextItemWidth(windowSize.x * 0.12f);
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

				actor->SetLocalPosition(newPosition);
			}

			ImGui::TableNextColumn();

			Vec3 newRotation = rotation;
			ImGui::Text("Rotation");

			ImGui::TableNextColumn();

			ImGui::Dummy(ImVec2(10.0f, 0.0f));
			ImGui::SameLine();
			ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
			ImGui::Text("X");
			ImGui::PopStyleColor();
			ImGui::SameLine();
			ImGui::SetNextItemWidth(windowSize.x * 0.12f);
			ImGui::DragFloat("##RotX", &newRotation[0]);

			ImGui::SameLine();
			ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
			ImGui::Text("Y");
			ImGui::PopStyleColor();
			ImGui::SameLine();
			ImGui::SetNextItemWidth(windowSize.x * 0.12f);
			ImGui::DragFloat("##RotY", &newRotation[1]);

			ImGui::SameLine();
			ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 0, 255, 255));
			ImGui::Text("Z");
			ImGui::PopStyleColor();
			ImGui::SameLine();
			ImGui::SetNextItemWidth(windowSize.x * 0.12f);
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

				actor->SetLocalRotation(newRotation);
			}

			ImGui::TableNextColumn();

			Vec3 newScale = scale;
			ImGui::Text("Scale");
			ImGui::SameLine();
			static bool scaleProportionally = false;
			ImGui::Checkbox("##Prop", &scaleProportionally);

			ImGui::TableNextColumn();

			ImGui::Dummy(ImVec2(10.0f, 0.0f));
			ImGui::SameLine();
			ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
			ImGui::Text("X");
			ImGui::PopStyleColor();
			ImGui::SameLine();
			ImGui::SetNextItemWidth(windowSize.x * 0.12f);
			ImGui::DragFloat("##ScaX", &newScale[0], 0.1f);

			ImGui::SameLine();
			ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
			ImGui::Text("Y");
			ImGui::PopStyleColor();
			ImGui::SameLine();
			ImGui::SetNextItemWidth(windowSize.x * 0.12f);
			ImGui::DragFloat("##ScaY", &newScale[1], 0.1f);

			ImGui::SameLine();
			ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 0, 255, 255));
			ImGui::Text("Z");
			ImGui::PopStyleColor();
			ImGui::SameLine();
			ImGui::SetNextItemWidth(windowSize.x * 0.12f);
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

				actor->SetLocalScale(newScale);
			}
		}

		ImGui::EndTable();
	}

	GUID componentToRemove = GUID::Empty;
	for (size_t i = 0; i < actor->GetComponentCount(); i++)
	{
		ImGui::PushID(i);

		auto& component = actor->GetComponentByIndex(i);
		const auto& componentName = component->VGetName();
		const bool bComponentHeaderOpen =
			ImGui::CollapsingHeader(componentName.c_str(), ImGuiTreeNodeFlags_DefaultOpen);
		if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Right))
		{
			ImGui::OpenPopup(ImGui::GetID("##ComponentContextMenu"));
		}

		if (ImGui::BeginPopupContextItem("##ComponentContextMenu"))
		{
			if (ImGui::MenuItem("Remove"))
			{
				componentToRemove = component->VGetID();
			}

			ImGui::EndPopup();
		}

		if (bComponentHeaderOpen)
		{
			if (component->VGetID() == Gameplay::StaticModelComponent::COMPONENT_ID)
			{
				auto actorComponent = dynamic_pointer_cast<Gameplay::StaticModelComponent>(component);
				DrawActorComponentProperties(context, actor, actorComponent);
			}
			else if (component->VGetID() == Gameplay::SkeletalModelComponent::COMPONENT_ID)
			{
				auto actorComponent = dynamic_pointer_cast<Gameplay::SkeletalModelComponent>(component);
				DrawActorComponentProperties(context, actor, actorComponent);
			}
			else if (component->VGetID() == Gameplay::PointLightComponent::COMPONENT_ID)
			{
				auto actorComponent = dynamic_pointer_cast<Gameplay::PointLightComponent>(component);
				DrawActorComponentProperties(context, actor, actorComponent);
			}
			else if (component->VGetID() == Gameplay::SpotLightComponent::COMPONENT_ID)
			{
				auto actorComponent = dynamic_pointer_cast<Gameplay::SpotLightComponent>(component);
				DrawActorComponentProperties(context, actor, actorComponent);
			}
			else
			{
				auto scriptComponent = dynamic_pointer_cast<Script::ScriptComponent>(component);
				DrawActorComponentProperties(context, actor, scriptComponent);
			}
		}

		ImGui::PopID();
	}

	if (componentToRemove != GUID::Empty)
	{
		actor->RemoveComponent(componentToRemove);
		componentToRemove = GUID::Empty;
	}

	ImGui::Dummy(ImVec2(windowSize.x, 50.0f));
	if (ImGui::Button("+ Add Component", ImVec2(windowSize.x, 40.0f)))
	{
		ImGui::OpenPopup("##AddComponent");
	}

	if (ImGui::BeginPopup("##AddComponent"))
	{
		ImGui::PushStyleColor(ImGuiCol_Header, ImGui::GetStyle().Colors[ImGuiCol_HeaderHovered]);
		ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImGui::GetStyle().Colors[ImGuiCol_HeaderHovered]);
		const bool bIsHeaderOpen = ImGui::CollapsingHeader("Select a component...",
		                                                   ImGuiTreeNodeFlags_Leaf
		                                                   | ImGuiTreeNodeFlags_DefaultOpen
		                                                   | ImGuiTreeNodeFlags_NoTreePushOnOpen
		);
		ImGui::PopStyleColor(2);

		if (bIsHeaderOpen)
		{
			for (size_t i = 0; i < pComponentsRegistry->GetComponentCount(); i++)
			{
				const GUID componentID = pComponentsRegistry->GetComponentID(i);
				const char* componentName = pComponentsRegistry->GetComponentName(i).c_str();
				if (actor->HasComponent(componentID))
				{
					ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImGui::GetStyle().Colors[ImGuiCol_Header]);
					ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImGui::GetStyle().Colors[ImGuiCol_Header]);
					ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyle().Colors[ImGuiCol_TextDisabled]);

					ImGui::TreeNodeEx(componentName, ImGuiTreeNodeFlags_Leaf);

					ImGui::PopStyleColor(3);
				}
				else
				{
					ImGui::TreeNodeEx(componentName, ImGuiTreeNodeFlags_Leaf);

					if (ImGui::IsItemClicked())
					{
						const auto pActorComponent = pComponentsRegistry->CreateComponent(
							componentID
						);
						actor->AddComponent(pActorComponent);

						ImGui::CloseCurrentPopup();
					}
				}

				ImGui::TreePop();
			}
		}

		ImGui::EndPopup();
	}

	ImGui::PopID();
}

void Arg::Editor::GUI::DetailsPanel::DrawActorComponentProperties(
	const EditorGUIContext& context,
	Gameplay::Actor* pActor,
	std::shared_ptr<Gameplay::StaticModelComponent>& pComponent)
{
	Editor* pEditor = context.pEditor;
	const bool isProjectOpended = pEditor->IsProjectOpened();
	auto& pResourceCache = isProjectOpended
		                       ? pEditor->GetProject()->GetResourceCache()
		                       : pEditor->GetResourceCache();
	auto& pContent = isProjectOpended
		                 ? pEditor->GetProject()->GetContent()
		                 : pEditor->GetContent();

	if (ImGui::BeginTable(
		"##StaticModelComponentTable",
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

			ImGui::Text("Static Model");

			ImGui::TableNextColumn();

			const auto staticModel = pComponent->GetStaticModel();
			ResourceHandleProperty(
				"##StaticModelHandle",
				Vec2(ImGui::GetWindowWidth() - 165.0f, 25.0f),
				staticModel.IsValid() ? staticModel.Get()->GetName().c_str() : nullptr,
				[&](GUID droppedResourceID)
				{
					const auto& resource = pResourceCache->GetResource(droppedResourceID);
					if (resource->GetType() == Content::ResourceType::ResourceTypeStaticModel)
					{
						pComponent->SetStaticModel(pResourceCache->CreateHandle<Content::StaticModelResource>(
							droppedResourceID
						));
					}
				},
				[&]
				{
					pComponent->SetStaticModel(pResourceCache->CreateHandle<Content::StaticModelResource>(
						GUID::Empty
					));
				}
			);
		}

		{
			ImGui::TableNextColumn();
			ImGui::Dummy(ImVec2(100.0f, 0.0f));

			ImGui::Text("Materials");

			ImGui::TableNextColumn();

			for (size_t i = 0; i < pComponent->GetMaterialCount(); i++)
			{
				ImGui::PushID(i);

				ImGui::Text(std::to_string(i).c_str());
				ImGui::SameLine();

				const auto material = pComponent->GetMaterial(i);
				ResourceHandleProperty(
					"##MaterialHandle",
					Vec2(ImGui::GetWindowWidth() - 180.0f, 25.0f),
					material.IsValid() ? material.Get()->GetName().c_str() : nullptr,
					[&](GUID droppedResourceID)
					{
						const auto& resource = pResourceCache->GetResource(droppedResourceID);
						if (resource->GetType() == Content::ResourceType::ResourceTypeMaterial)
						{
							pComponent->SetMaterial(
								i,
								pResourceCache->CreateHandle<Content::MaterialResource>(
									droppedResourceID
								));
						}
					},
					[&]
					{
						pComponent->SetMaterial(
							i,
							pResourceCache->CreateHandle<Content::MaterialResource>(GUID::Empty)
						);
					}
				);

				ImGui::PopID();
			}
		}

		{
			ImGui::TableNextColumn();
			ImGui::Dummy(ImVec2(100.0f, 0.0f));

			ImGui::Text("Cast Shadows");

			ImGui::TableNextColumn();

			bool bCastShadows = pComponent->GetCastShadows();
			ImGui::Checkbox("##CastShadows", &bCastShadows);
			if (bCastShadows != pComponent->GetCastShadows())
			{
				pComponent->SetCastShadows(bCastShadows);
			}
		}

		{
			ImGui::TableNextColumn();
			ImGui::Dummy(ImVec2(100.0f, 0.0f));

			ImGui::Text("Receive Shadows");

			ImGui::TableNextColumn();

			bool bReceiveShadows = pComponent->GetReceiveShadows();
			ImGui::Checkbox("##ReceiveShadows", &bReceiveShadows);
			if (bReceiveShadows != pComponent->GetReceiveShadows())
			{
				pComponent->SetReceiveShadows(bReceiveShadows);
			}
		}
	}

	ImGui::EndTable();
}

void Arg::Editor::GUI::DetailsPanel::DrawActorComponentProperties(
	const EditorGUIContext& context,
	Gameplay::Actor* pActor,
	std::shared_ptr<Gameplay::SkeletalModelComponent>& pComponent
)
{
	Editor* pEditor = context.pEditor;
	const bool isProjectOpended = pEditor->IsProjectOpened();
	auto& pResourceCache = isProjectOpended
		                       ? pEditor->GetProject()->GetResourceCache()
		                       : pEditor->GetResourceCache();
	auto& pContent = isProjectOpended
		                 ? pEditor->GetProject()->GetContent()
		                 : pEditor->GetContent();

	if (ImGui::BeginTable(
		"##SkeletonComponentTable",
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

			ImGui::Text("Skeleton");

			ImGui::TableNextColumn();

			const auto skeleton = pComponent->GetSkeleton();
			ResourceHandleProperty(
				"##SkeletonHandle",
				Vec2(ImGui::GetWindowWidth() - 165.0f, 25.0f),
				skeleton.IsValid() ? skeleton.Get()->GetName().c_str() : nullptr,
				[&](GUID droppedResourceID)
				{
					const auto& resource = pResourceCache->GetResource(droppedResourceID);
					if (resource->GetType() == Content::ResourceType::ResourceTypeSkeleton)
					{
						pComponent->SetSkeleton(pResourceCache->CreateHandle<Content::SkeletonResource>(
							droppedResourceID
						));
					}
				},
				[&]
				{
					pComponent->SetSkeleton(pResourceCache->CreateHandle<Content::SkeletonResource>(
						GUID::Empty
					));
				}
			);
		}

		{
			ImGui::TableNextColumn();
			ImGui::Dummy(ImVec2(100.0f, 0.0f));

			ImGui::Text("Skeletal Model");

			ImGui::TableNextColumn();

			const auto model = pComponent->GetModel();
			ResourceHandleProperty(
				"##SkeletalModelHandle",
				Vec2(ImGui::GetWindowWidth() - 165.0f, 25.0f),
				model.IsValid() ? model.Get()->GetName().c_str() : nullptr,
				[&](GUID droppedResourceID)
				{
					const auto& resource = pResourceCache->GetResource(droppedResourceID);
					if (resource->GetType() == Content::ResourceType::ResourceTypeSkeletalModel)
					{
						pComponent->SetModel(pResourceCache->CreateHandle<Content::SkeletalModelResource>(
							droppedResourceID
						));
					}
				},
				[&]
				{
					pComponent->SetModel(pResourceCache->CreateHandle<Content::SkeletalModelResource>(
						GUID::Empty
					));
				}
			);
		}

		{
			ImGui::TableNextColumn();
			ImGui::Dummy(ImVec2(100.0f, 0.0f));

			ImGui::Text("Materials");

			ImGui::TableNextColumn();

			for (size_t i = 0; i < pComponent->GetMaterialCount(); i++)
			{
				ImGui::PushID(i);

				ImGui::Text(std::to_string(i).c_str());
				ImGui::SameLine();

				const auto material = pComponent->GetMaterial(i);
				ResourceHandleProperty(
					"##MaterialHandle",
					Vec2(ImGui::GetWindowWidth() - 180.0f, 25.0f),
					material.IsValid() ? material.Get()->GetName().c_str() : nullptr,
					[&](GUID droppedResourceID)
					{
						const auto& resource = pResourceCache->GetResource(droppedResourceID);
						if (resource->GetType() == Content::ResourceType::ResourceTypeMaterial)
						{
							pComponent->SetMaterial(
								i,
								pResourceCache->CreateHandle<Content::MaterialResource>(
									droppedResourceID
								));
						}
					},
					[&]
					{
						pComponent->SetMaterial(
							i,
							pResourceCache->CreateHandle<Content::MaterialResource>(GUID::Empty)
						);
					}
				);

				ImGui::PopID();
			}
		}

		{
			ImGui::TableNextColumn();
			ImGui::Dummy(ImVec2(100.0f, 0.0f));

			ImGui::Text("Cast Shadows");

			ImGui::TableNextColumn();

			bool bCastShadows = pComponent->GetCastShadows();
			ImGui::Checkbox("##CastShadows", &bCastShadows);
			if (bCastShadows != pComponent->GetCastShadows())
			{
				pComponent->SetCastShadows(bCastShadows);
			}
		}

		{
			ImGui::TableNextColumn();
			ImGui::Dummy(ImVec2(100.0f, 0.0f));

			ImGui::Text("Receive Shadows");

			ImGui::TableNextColumn();

			bool bReceiveShadows = pComponent->GetReceiveShadows();
			ImGui::Checkbox("##ReceiveShadows", &bReceiveShadows);
			if (bReceiveShadows != pComponent->GetReceiveShadows())
			{
				pComponent->SetReceiveShadows(bReceiveShadows);
			}
		}

		{
			ImGui::TableNextColumn();
			ImGui::Dummy(ImVec2(100.0f, 0.0f));

			ImGui::Text("Current Animation");

			ImGui::TableNextColumn();

			const auto animation = pComponent->GetCurrentAnimation();
			ResourceHandleProperty(
				"##SkeletalAnimationHandle",
				Vec2(ImGui::GetWindowWidth() - 165.0f, 25.0f),
				animation.IsValid() ? animation.Get()->GetName().c_str() : nullptr,
				[&](GUID droppedResourceID)
				{
					const auto& resource = pResourceCache->GetResource(droppedResourceID);
					if (resource->GetType() == Content::ResourceType::ResourceTypeSkeletalAnimation)
					{
						pComponent->SetCurrentAnimation(
							pResourceCache->CreateHandle<Content::SkeletalAnimationResource>(
								droppedResourceID
							));
					}
				},
				[&]
				{
					pComponent->SetCurrentAnimation(pResourceCache->CreateHandle<Content::SkeletalAnimationResource>(
						GUID::Empty
					));
				}
			);
		}

		{
			ImGui::TableNextColumn();
			ImGui::Dummy(ImVec2(100.0f, 0.0f));

			ImGui::Text("Play on Start");

			ImGui::TableNextColumn();

			bool bPlayOnStart = pComponent->GetPlayOnStart();
			ImGui::Checkbox("##PlayOnStart", &bPlayOnStart);
			if (bPlayOnStart != pComponent->GetPlayOnStart())
			{
				pComponent->SetPlayOnStart(bPlayOnStart);
			}
		}

		{
			ImGui::TableNextColumn();
			ImGui::Dummy(ImVec2(100.0f, 0.0f));

			ImGui::Text("Loop");

			ImGui::TableNextColumn();

			bool bLooping = pComponent->GetLooping();
			ImGui::Checkbox("##Loop", &bLooping);
			if (bLooping != pComponent->GetLooping())
			{
				pComponent->SetLooping(bLooping);
			}
		}
	}

	ImGui::EndTable();
}

void Arg::Editor::GUI::DetailsPanel::DrawActorComponentProperties(
	const EditorGUIContext& context,
	Gameplay::Actor* pActor,
	std::shared_ptr<Gameplay::PointLightComponent>& pComponent)
{
	Editor* pEditor = context.pEditor;
	const bool isProjectOpended = pEditor->IsProjectOpened();
	auto& pResourceCache = isProjectOpended
		                       ? pEditor->GetProject()->GetResourceCache()
		                       : pEditor->GetResourceCache();
	auto& pContent = isProjectOpended
		                 ? pEditor->GetProject()->GetContent()
		                 : pEditor->GetContent();

	if (ImGui::BeginTable(
		"##StaticModelComponentTable",
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

			Vec3 color = pComponent->GetLightColor();
			ImGui::ColorEdit3("##LightColor", Math::ValuePtr(color));
			if (color != pComponent->GetLightColor())
			{
				pComponent->SetLightColor(color);
			}
		}

		{
			ImGui::TableNextColumn();
			ImGui::Dummy(ImVec2(100.0f, 0.0f));

			ImGui::Text("Range");

			ImGui::TableNextColumn();

			float range = pComponent->GetLightRange();
			ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - 165.0f);
			ImGui::SliderFloat("##LightRange", &range, 0.0f, 1.0f);
			if (range != pComponent->GetLightRange())
			{
				pComponent->SetLightRange(range);
			}
		}

		{
			ImGui::TableNextColumn();
			ImGui::Dummy(ImVec2(100.0f, 0.0f));

			ImGui::Text("Intensity");

			ImGui::TableNextColumn();

			float intensity = pComponent->GetLightIntensity();
			ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - 165.0f);
			ImGui::DragFloat("##LightIntensity", &intensity, 0.1f);
			if (intensity != pComponent->GetLightIntensity())
			{
				pComponent->SetLightIntensity(intensity);
			}
		}
	}

	ImGui::EndTable();
}

void Arg::Editor::GUI::DetailsPanel::DrawActorComponentProperties(
	const EditorGUIContext& context,
	Gameplay::Actor* pActor,
	std::shared_ptr<Gameplay::SpotLightComponent>& pComponent
)
{
	Editor* pEditor = context.pEditor;
	const bool isProjectOpended = pEditor->IsProjectOpened();
	auto& pResourceCache = isProjectOpended
		                       ? pEditor->GetProject()->GetResourceCache()
		                       : pEditor->GetResourceCache();
	auto& pContent = isProjectOpended
		                 ? pEditor->GetProject()->GetContent()
		                 : pEditor->GetContent();

	if (ImGui::BeginTable(
		"##StaticModelComponentTable",
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

			Vec3 color = pComponent->GetLightColor();
			ImGui::ColorEdit3("##LightColor", Math::ValuePtr(color));
			if (color != pComponent->GetLightColor())
			{
				pComponent->SetLightColor(color);
			}
		}

		{
			ImGui::TableNextColumn();
			ImGui::Dummy(ImVec2(100.0f, 0.0f));

			ImGui::Text("Range");

			ImGui::TableNextColumn();

			float range = pComponent->GetLightRange();
			ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - 165.0f);
			ImGui::SliderFloat("##LightRange", &range, 0.0f, 1.0f);
			if (range != pComponent->GetLightRange())
			{
				pComponent->SetLightRange(range);
			}
		}

		{
			ImGui::TableNextColumn();
			ImGui::Dummy(ImVec2(100.0f, 0.0f));

			ImGui::Text("Intensity");

			ImGui::TableNextColumn();

			float intensity = pComponent->GetLightIntensity();
			ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - 165.0f);
			ImGui::DragFloat("##LightIntensity", &intensity, 0.1f);
			if (intensity != pComponent->GetLightIntensity())
			{
				pComponent->SetLightIntensity(intensity);
			}
		}

		{
			ImGui::TableNextColumn();
			ImGui::Dummy(ImVec2(100.0f, 0.0f));

			ImGui::Text("Inner Cone");

			ImGui::TableNextColumn();

			float angle = pComponent->GetInnerConeAngle();
			ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - 165.0f);
			ImGui::SliderFloat("##InnerConeAngle", &angle, 0.0f, 180.0f);
			if (angle != pComponent->GetInnerConeAngle())
			{
				pComponent->SetInnerConeAngle(angle);
			}
		}

		{
			ImGui::TableNextColumn();
			ImGui::Dummy(ImVec2(100.0f, 0.0f));

			ImGui::Text("Outer Cone");

			ImGui::TableNextColumn();

			float angle = pComponent->GetOuterConeAngle();
			ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - 165.0f);
			ImGui::SliderFloat("##OuterConeAngle", &angle, 0.0f, 180.0f);
			if (angle != pComponent->GetOuterConeAngle())
			{
				pComponent->SetOuterConeAngle(angle);
			}
		}
	}

	ImGui::EndTable();
}

void Arg::Editor::GUI::DetailsPanel::DrawActorComponentProperties(
	const EditorGUIContext& context,
	Gameplay::Actor* pActor,
	std::shared_ptr<Script::ScriptComponent>& pComponent
)
{
	Editor* pEditor = context.pEditor;
	const bool isProjectOpended = pEditor->IsProjectOpened();
	auto& pResourceCache = isProjectOpended
		                       ? pEditor->GetProject()->GetResourceCache()
		                       : pEditor->GetResourceCache();
	auto& pContent = isProjectOpended
		                 ? pEditor->GetProject()->GetContent()
		                 : pEditor->GetContent();

	if (ImGui::BeginTable(
		"##StaticModelComponentTable",
		2,
		ImGuiTableFlags_BordersInnerV
		| ImGuiTableFlags_BordersOuter
		| ImGuiTableFlags_NoSavedSettings
		| ImGuiTableFlags_SizingFixedFit
	))
	{
		for (size_t i = 0; i < pComponent->GetFieldsCount(); i++)
		{
			const auto field = pComponent->GetField(i);
			switch (field.Type)
			{
			case Script::ScriptComponent::Float:
				{
					float fieldValue;
					pComponent->GetFieldValue(field.Name, fieldValue);
					DrawScriptComponentProperty(
						context,
						pActor,
						pComponent,
						field.Name,
						fieldValue
					);
					break;
				}
			}
		}
	}
	ImGui::EndTable();
}

void Arg::Editor::GUI::DetailsPanel::DrawScriptComponentProperty(
	const EditorGUIContext& context,
	Gameplay::Actor* pActor,
	std::shared_ptr<Script::ScriptComponent>& pComponent,
	const std::string& propertyName,
	const float& propertyValue
)
{
	ImGui::TableNextColumn();
	ImGui::Dummy(ImVec2(100.0f, 0.0f));

	ImGui::Text(propertyName.c_str());

	ImGui::TableNextColumn();

	float inputValue = propertyValue;
	ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - 165.0f * 0.12f);
	ImGui::InputFloat("##FloatProperty", &inputValue);
	if (inputValue != propertyValue)
	{
		pComponent->SetFieldValue(propertyName, inputValue);
	}

	ImGui::TableNextColumn();
}
