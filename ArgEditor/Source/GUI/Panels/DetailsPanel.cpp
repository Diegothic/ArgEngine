#include <arg_pch.hpp>
#include "DetailsPanel.hpp"

#include <imgui/ImGui.hpp>

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

	const bool bIsTransformOpen = ImGui::CollapsingHeader(
		"Transform",
		ImGuiTreeNodeFlags_DefaultOpen
	);
	if (bIsTransformOpen)
	{
		const Vec3 position = actor->GetLocalPosition();
		const Vec3 rotation = actor->GetLocalRotationEuler();
		const Vec3 scale = actor->GetLocalScale();

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
				newPosition = Gameplay::Transform::POSITION_DEFAULT;
			}

			if (newPosition != actor->GetLocalPosition())
			{
				actor->SetLocalPosition(newPosition);
			}

			ImGui::TableNextColumn();

			Vec3 newRotation = Math::degrees(rotation);
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
				newRotation = Math::degrees(Gameplay::Transform::ROTATION_EULER_DEFAULT);
			}

			if (newRotation != Math::degrees(rotation))
			{
				actor->SetLocalRotationEuler(Math::radians(newRotation));
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
				newScale = Gameplay::Transform::SCALE_DEFAULT;
			}

			for (int i = 0; i < 3; i++)
			{
				newScale[i] = Math::max(newScale[i], 0.0f);
			}

			const Vec3 currentScale = actor->GetLocalScale();
			if (newScale != currentScale)
			{
				if (scaleProportionally)
				{
					for (int i = 0; i < 3; i++)
					{
						if (newScale[i] != currentScale[i]
							&& currentScale[i] > 0.0f)
						{
							const float diffPercent = newScale[i] / currentScale[i];
							for (int j = 0; j < 3; j++)
							{
								if (j == i)
								{
									continue;
								}

								newScale[j] = currentScale[j] * diffPercent;
							}

							break;
						}
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
		const bool bComponentHeaderOpen = ImGui::CollapsingHeader(
			componentName.c_str(),
			ImGuiTreeNodeFlags_DefaultOpen
		);

		if (ImGui::BeginDragDropSource())
		{
			static GUID actorComponentSource[2];
			actorComponentSource[0] = actor->GetID();
			actorComponentSource[1] = component->VGetID();

			ImGui::SetDragDropPayload("ActorComponent", &actorComponentSource, 2 * sizeof(GUID));
			ImGui::Text(component->VGetName().c_str());
			ImGui::EndDragDropSource();
		}

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
			else if (component->VGetID() == Gameplay::CameraComponent::COMPONENT_ID)
			{
				auto actorComponent = dynamic_pointer_cast<Gameplay::CameraComponent>(component);
				DrawActorComponentProperties(context, actor, actorComponent);
			}
			else if (component->VGetID() == Gameplay::PhysicsBodyComponent::COMPONENT_ID)
			{
				auto actorComponent = dynamic_pointer_cast<Gameplay::PhysicsBodyComponent>(component);
				DrawActorComponentProperties(context, actor, actorComponent);
			}
			else if (component->VGetID() == Gameplay::SoundPlayerComponent::COMPONENT_ID)
			{
				auto actorComponent = dynamic_pointer_cast<Gameplay::SoundPlayerComponent>(component);
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
		"##SkeletalModelComponentTable",
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

			ImGui::Text("Skeletal");
			ImGui::Text("Model");

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

			ImGui::Text("Cast");
			ImGui::Text("Shadows");

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

			ImGui::Text("Receive");
			ImGui::Text("Shadows");

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

			ImGui::Text("Current");
			ImGui::Text("Animation");

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

			ImGui::Text("Play");
			ImGui::Text("on Start");

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
		"##PointLightComponentTable",
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
		"##SpotLightComponentTable",
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
	std::shared_ptr<Gameplay::CameraComponent>& pComponent
)
{
	Editor* pEditor = context.pEditor;
	const bool isProjectOpen = pEditor->IsProjectOpened();
	auto& pResourceCache = isProjectOpen
		                       ? pEditor->GetProject()->GetResourceCache()
		                       : pEditor->GetResourceCache();
	auto& pContent = isProjectOpen
		                 ? pEditor->GetProject()->GetContent()
		                 : pEditor->GetContent();

	if (ImGui::BeginTable(
		"##CameraComponentTable",
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

			ImGui::Text("Mode");

			ImGui::TableNextColumn();

			const char* backgroundTypes[] = {"Perspective", "Orthographic"};
			int32_t currentType = static_cast<int32_t>(pComponent->GetMode());
			ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - 165.0f);
			if (ImGui::BeginCombo("##Mode", backgroundTypes[currentType]))
			{
				for (int32_t i = 0; i < 2; i++)
				{
					const bool bIsSelected = currentType == i;
					if (ImGui::Selectable(backgroundTypes[i], bIsSelected))
					{
						currentType = i;
						pComponent->SetMode(static_cast<Gameplay::CameraMode>(i));
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

					ImGui::Text("FOV Angle");

					ImGui::TableNextColumn();

					float angle = pComponent->GetFOVAngle();
					ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - 165.0f);
					ImGui::InputFloat("##FOVAngle", &angle);
					if (angle != pComponent->GetFOVAngle())
					{
						pComponent->SetFOVAngle(angle);
					}
					break;
				}
			case 1:
				{
					ImGui::TableNextColumn();
					ImGui::Dummy(ImVec2(100.0f, 0.0f));

					ImGui::Text("Size");

					ImGui::TableNextColumn();

					float size = pComponent->GetSize();
					ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - 165.0f);
					ImGui::InputFloat("##Size", &size);
					if (size != pComponent->GetSize())
					{
						pComponent->SetSize(size);
					}
					break;
				}
			}
		}

		ImGui::EndTable();
	}
}

void Arg::Editor::GUI::DetailsPanel::DrawActorComponentProperties(
	const EditorGUIContext& context,
	Gameplay::Actor* pActor,
	std::shared_ptr<Gameplay::PhysicsBodyComponent>& pComponent
)
{
	Editor* pEditor = context.pEditor;
	const bool isProjectOpen = pEditor->IsProjectOpened();
	auto& pResourceCache = isProjectOpen
		                       ? pEditor->GetProject()->GetResourceCache()
		                       : pEditor->GetResourceCache();
	auto& pContent = isProjectOpen
		                 ? pEditor->GetProject()->GetContent()
		                 : pEditor->GetContent();

	if (ImGui::BeginTable(
		"##PhysicsBodyComponentTable",
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

			ImGui::Text("Dynamic");

			ImGui::TableNextColumn();

			bool isDynamic = pComponent->GetIsDynamic();
			ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - 165.0f);
			ImGui::Checkbox("##IsDynamic", &isDynamic);
			if (isDynamic != pComponent->GetIsDynamic())
			{
				pComponent->SetIsDynamic(isDynamic);
			}
		}

		{
			ImGui::TableNextColumn();
			ImGui::Dummy(ImVec2(100.0f, 0.0f));

			ImGui::Text("Mass");

			ImGui::TableNextColumn();

			float mass = pComponent->GetMass();
			ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - 165.0f);
			ImGui::InputFloat("##Mass", &mass);
			if (mass != pComponent->GetMass())
			{
				pComponent->SetMass(mass);
			}
		}

		{
			ImGui::TableNextColumn();
			ImGui::Dummy(ImVec2(100.0f, 0.0f));

			ImGui::Text("Damping");

			ImGui::TableNextColumn();

			float damping = pComponent->GetDamping();
			ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - 165.0f);
			ImGui::InputFloat("##Damping", &damping);
			if (damping != pComponent->GetDamping())
			{
				pComponent->SetDamping(damping);
			}
		}

		{
			ImGui::TableNextColumn();
			ImGui::Dummy(ImVec2(100.0f, 0.0f));

			ImGui::Text("Angular");
			ImGui::Text("Damping");

			ImGui::TableNextColumn();

			float damping = pComponent->GetAngularDamping();
			ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - 165.0f);
			ImGui::InputFloat("##ADamping", &damping);
			if (damping != pComponent->GetAngularDamping())
			{
				pComponent->SetAngularDamping(damping);
			}
		}

		{
			ImGui::TableNextColumn();
			ImGui::Dummy(ImVec2(100.0f, 0.0f));

			ImGui::Text("Bounciness");

			ImGui::TableNextColumn();

			float bounciness = pComponent->GetBounciness();
			ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - 165.0f);
			ImGui::DragFloat("##Bounciness", &bounciness, 0.1f, 0.0f);
			if (bounciness != pComponent->GetBounciness())
			{
				pComponent->SetBounciness(bounciness);
			}
		}

		{
			ImGui::TableNextColumn();
			ImGui::Dummy(ImVec2(100.0f, 0.0f));

			ImGui::Text("Lock");
			ImGui::Text("Movement");

			ImGui::TableNextColumn();

			{
				bool bLock = pComponent->GetMovementLockX();
				ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - 165.0f);
				ImGui::Text("X");
				ImGui::SameLine();
				ImGui::Checkbox("##LockMovementX", &bLock);
				if (bLock != pComponent->GetMovementLockX())
				{
					pComponent->SetMovementLockX(bLock);
				}
			}

			{
				bool bLock = pComponent->GetMovementLockY();
				ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - 165.0f);
				ImGui::Text("Y");
				ImGui::SameLine();
				ImGui::Checkbox("##LockMovementY", &bLock);
				if (bLock != pComponent->GetMovementLockY())
				{
					pComponent->SetMovementLockY(bLock);
				}
			}

			{
				bool bLock = pComponent->GetMovementLockZ();
				ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - 165.0f);
				ImGui::Text("Z");
				ImGui::SameLine();
				ImGui::Checkbox("##LockMovementZ", &bLock);
				if (bLock != pComponent->GetMovementLockZ())
				{
					pComponent->SetMovementLockZ(bLock);
				}
			}
		}

		{
			ImGui::TableNextColumn();
			ImGui::Dummy(ImVec2(100.0f, 0.0f));

			ImGui::Text("Lock");
			ImGui::Text("Rotation");

			ImGui::TableNextColumn();

			{
				bool bLock = pComponent->GetRotationLockX();
				ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - 165.0f);
				ImGui::Text("X");
				ImGui::SameLine();
				ImGui::Checkbox("##LockRotationX", &bLock);
				if (bLock != pComponent->GetRotationLockX())
				{
					pComponent->SetRotationLockX(bLock);
				}
			}

			{
				bool bLock = pComponent->GetRotationLockY();
				ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - 165.0f);
				ImGui::Text("Y");
				ImGui::SameLine();
				ImGui::Checkbox("##LockRotationY", &bLock);
				if (bLock != pComponent->GetRotationLockY())
				{
					pComponent->SetRotationLockY(bLock);
				}
			}

			{
				bool bLock = pComponent->GetRotationLockZ();
				ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - 165.0f);
				ImGui::Text("Z");
				ImGui::SameLine();
				ImGui::Checkbox("##LockRotationZ", &bLock);
				if (bLock != pComponent->GetRotationLockZ())
				{
					pComponent->SetRotationLockZ(bLock);
				}
			}
		}

		{
			ImGui::TableNextColumn();
			ImGui::Dummy(ImVec2(100.0f, 0.0f));

			ImGui::Text("Shape");

			ImGui::TableNextColumn();

			const char* shapeTypes[] = {"Box", "Sphere", "Capsule"};
			int32_t currentType = static_cast<int32_t>(pComponent->GetPhysicsShape());
			ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - 165.0f);
			if (ImGui::BeginCombo("##Shape", shapeTypes[currentType]))
			{
				for (int32_t i = 0; i < 3; i++)
				{
					const bool bIsSelected = currentType == i;
					if (ImGui::Selectable(shapeTypes[i], bIsSelected))
					{
						currentType = i;
						pComponent->SetPhysicsShape(static_cast<Physics::PhysicsBodyShape>(i));
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

					ImGui::Text("Size");

					ImGui::TableNextColumn();

					Vec3 size = pComponent->GetSize();
					ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - 165.0f);
					ImGui::InputFloat3("##Size", Math::ValuePtr(size));
					if (size != pComponent->GetSize())
					{
						pComponent->SetSize(size);
					}
					break;
				}
			case 1:
				{
					ImGui::TableNextColumn();
					ImGui::Dummy(ImVec2(100.0f, 0.0f));

					ImGui::Text("Radius");

					ImGui::TableNextColumn();

					Vec3 size = pComponent->GetSize();
					ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - 165.0f);
					ImGui::InputFloat("##Size", &size.x);
					if (size.x != pComponent->GetSize().x)
					{
						pComponent->SetSize(size);
					}
					break;
				}
			case 2:
				{
					{
						ImGui::TableNextColumn();
						ImGui::Dummy(ImVec2(100.0f, 0.0f));

						ImGui::Text("Radius");

						ImGui::TableNextColumn();

						float radius = pComponent->GetSize().x;
						ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - 165.0f);
						ImGui::InputFloat("##Size", &radius);
						if (radius != pComponent->GetSize().x)
						{
							Vec3 newSize = pComponent->GetSize();
							newSize.x = radius;
							pComponent->SetSize(newSize);
						}
					}
					{
						ImGui::TableNextColumn();
						ImGui::Dummy(ImVec2(100.0f, 0.0f));

						ImGui::Text("Height");

						ImGui::TableNextColumn();

						float height = pComponent->GetSize().z;
						ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - 165.0f);
						ImGui::InputFloat("##Height", &height);
						if (height != pComponent->GetSize().z)
						{
							Vec3 newSize = pComponent->GetSize();
							newSize.z = height;
							pComponent->SetSize(newSize);
						}
					}
					break;
				}
			}
		}

		ImGui::EndTable();
	}
}

void Arg::Editor::GUI::DetailsPanel::DrawActorComponentProperties(
	const EditorGUIContext& context,
	Gameplay::Actor* pActor,
	std::shared_ptr<Gameplay::SoundPlayerComponent>& pComponent
)
{
	Editor* pEditor = context.pEditor;
	const bool isProjectOpen = pEditor->IsProjectOpened();
	auto& pResourceCache = isProjectOpen
		                       ? pEditor->GetProject()->GetResourceCache()
		                       : pEditor->GetResourceCache();
	auto& pContent = isProjectOpen
		                 ? pEditor->GetProject()->GetContent()
		                 : pEditor->GetContent();

	if (ImGui::BeginTable(
		"##SoundPlayerComponentTable",
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

			ImGui::Text("Sound");

			ImGui::TableNextColumn();

			const auto& sound = pComponent->GetSound();
			ResourceHandleProperty(
				"##SoundHandle",
				Vec2(ImGui::GetWindowWidth() - 165.0f, 25.0f),
				sound.IsValid() ? sound.Get()->GetName().c_str() : nullptr,
				[&](GUID droppedResourceID)
				{
					const auto& resource = pResourceCache->GetResource(droppedResourceID);
					if (resource->GetType() == Content::ResourceType::ResourceTypeSound)
					{
						pComponent->SetSound(pResourceCache->CreateHandle<Content::SoundResource>(
							droppedResourceID
						));
					}
				},
				[&]
				{
					pComponent->SetSound(pResourceCache->CreateHandle<Content::SoundResource>(
						GUID::Empty
					));
				}
			);
		}

		{
			ImGui::TableNextColumn();
			ImGui::Dummy(ImVec2(100.0f, 0.0f));

			ImGui::Text("Play");
			ImGui::Text("on Start");

			ImGui::TableNextColumn();

			bool bPlayOnStart = pComponent->GetPlayOnStart();
			ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - 165.0f);
			ImGui::Checkbox("##PlayOnStart", &bPlayOnStart);
			if (bPlayOnStart != pComponent->GetPlayOnStart())
			{
				pComponent->SetPlayOnStart(bPlayOnStart);
			}
		}

		{
			ImGui::TableNextColumn();
			ImGui::Dummy(ImVec2(100.0f, 0.0f));

			ImGui::Text("Volume");

			ImGui::TableNextColumn();

			float volume = pComponent->GetVolume();
			ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - 165.0f);
			ImGui::InputFloat("##Volume", &volume);
			if (volume != pComponent->GetVolume())
			{
				pComponent->SetVolume(volume);
			}
		}

		{
			ImGui::TableNextColumn();
			ImGui::Dummy(ImVec2(100.0f, 0.0f));

			ImGui::Text("Volume");
			ImGui::Text("Variance");

			ImGui::TableNextColumn();

			float volumeVariance = pComponent->GetVolumeVariance();
			ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - 165.0f);
			ImGui::InputFloat("##VolumeVariance", &volumeVariance);
			if (volumeVariance != pComponent->GetVolumeVariance())
			{
				pComponent->SetVolumeVariance(volumeVariance);
			}
		}

		{
			ImGui::TableNextColumn();
			ImGui::Dummy(ImVec2(100.0f, 0.0f));

			ImGui::Text("Pitch");

			ImGui::TableNextColumn();

			float pitch = pComponent->GetPitch();
			ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - 165.0f);
			ImGui::InputFloat("##Pitch", &pitch);
			if (pitch != pComponent->GetPitch())
			{
				pComponent->SetPitch(pitch);
			}
		}

		{
			ImGui::TableNextColumn();
			ImGui::Dummy(ImVec2(100.0f, 0.0f));

			ImGui::Text("Pitch");
			ImGui::Text("Variance");

			ImGui::TableNextColumn();

			float pitchVariance = pComponent->GetPitchVariance();
			ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - 165.0f);
			ImGui::InputFloat("##PitchVariance", &pitchVariance);
			if (pitchVariance != pComponent->GetPitchVariance())
			{
				pComponent->SetPitchVariance(pitchVariance);
			}
		}

		{
			ImGui::TableNextColumn();
			ImGui::Dummy(ImVec2(100.0f, 0.0f));

			ImGui::Text("Loop");

			ImGui::TableNextColumn();

			bool isLooping = pComponent->GetIsLooping();
			ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - 165.0f);
			ImGui::Checkbox("##IsLooping", &isLooping);
			if (isLooping != pComponent->GetIsLooping())
			{
				pComponent->SetIsLooping(isLooping);
			}
		}

		{
			ImGui::TableNextColumn();
			ImGui::Dummy(ImVec2(100.0f, 0.0f));

			ImGui::Text("Is 3D");

			ImGui::TableNextColumn();

			bool is3D = pComponent->GetIs3D();
			ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - 165.0f);
			ImGui::Checkbox("##Is3D", &is3D);
			if (is3D != pComponent->GetIs3D())
			{
				pComponent->SetIs3D(is3D);
			}
		}

		if (pComponent->GetIs3D())
		{
			ImGui::TableNextColumn();
			ImGui::Dummy(ImVec2(100.0f, 0.0f));

			ImGui::Text("Distance");

			ImGui::TableNextColumn();

			float distance = pComponent->GetDistance();
			ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - 165.0f);
			ImGui::InputFloat("##Distance", &distance);
			if (distance != pComponent->GetDistance())
			{
				pComponent->SetDistance(distance);
			}
		}

		ImGui::EndTable();
	}
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
