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


	for (size_t i = 0; i < actor->GetComponentCount(); i++)
	{
		auto& component = actor->GetComponentByIndex(i);
		const auto& componentName = component->VGetName();
		if (ImGui::CollapsingHeader(componentName.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
		{
			if (component->VGetID() == Gameplay::StaticModelComponent::COMPONENT_ID)
			{
				auto staticModelComponent = dynamic_pointer_cast<Gameplay::StaticModelComponent>(component);
				DrawActorComponentProperties(context, actor, staticModelComponent);
			}
		}
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
			if (actor->HasComponent(Gameplay::StaticModelComponent::COMPONENT_ID))
			{
				ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImGui::GetStyle().Colors[ImGuiCol_Header]);
				ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImGui::GetStyle().Colors[ImGuiCol_Header]);
				ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyle().Colors[ImGuiCol_TextDisabled]);

				ImGui::TreeNodeEx("Static Model", ImGuiTreeNodeFlags_Leaf);

				ImGui::PopStyleColor(3);
			}
			else
			{
				ImGui::TreeNodeEx("Static Model", ImGuiTreeNodeFlags_Leaf);

				if (ImGui::IsItemClicked())
				{
					const auto pActorComponent = pComponentsRegistry->CreateComponent(
						Gameplay::StaticModelComponent::COMPONENT_ID
					);
					actor->AddComponent(pActorComponent);

					ImGui::CloseCurrentPopup();
				}
			}

			ImGui::TreePop();
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
		ImGui::TableNextColumn();
		ImGui::Dummy(ImVec2(100.0f, 0.0f));

		ImGui::Text("Static Model");

		ImGui::TableNextColumn();

		auto cursorPos = ImGui::GetCursorPos();
		ImGui::Button(
			"##StaticModelHandle",
			ImVec2(ImGui::GetWindowWidth() - 100.0f, 25.0f)
		);

		{
			static bool droppedResource = false;
			static GUID droppedResourceID;
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Resource"))
				{
					IM_ASSERT(payload->DataSize == sizeof(GUID));
					droppedResourceID = *((GUID*)payload->Data);
					droppedResource = true;
				}
				ImGui::EndDragDropTarget();
			}

			if (droppedResource)
			{
				droppedResource = false;
				const auto& resource = pResourceCache->GetResource(droppedResourceID);
				if (resource->GetType() == Content::ResourceType::ResourceTypeStaticModel)
				{
					auto newStaticModel = pResourceCache->CreateHandle<Content::StaticModelResource>(droppedResourceID);
					pComponent->SetStaticModel(newStaticModel);
				}
			}
		}

		cursorPos.x += 10.0f;
		cursorPos.y += 5.0f;
		ImGui::SetCursorPos(cursorPos);

		const auto staticModel = pComponent->GetStaticModel();
		if (staticModel.IsValid())
		{
			const auto& staticModelName = staticModel.Get()->GetName();
			ImGui::Text(staticModelName.c_str());
		}
		else
		{
			ImGui::TextDisabled("Null reference");
		}


		ImGui::TableNextColumn();

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
				Vec2(ImGui::GetWindowWidth() - 120.0f, 25.0f),
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

		// ImGui::Text("0");
		// ImGui::SameLine();
		// ImGui::Button(
		// 	"##Material",
		// 	ImVec2(ImGui::GetWindowWidth() - 100.0f - ImGui::CalcTextSize("1").x, 25.0f)
		// );
		//
		// {
		// 	static bool droppedResource = false;
		// 	static GUID droppedResourceID;
		// 	if (ImGui::BeginDragDropTarget())
		// 	{
		// 		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Resource"))
		// 		{
		// 			IM_ASSERT(payload->DataSize == sizeof(GUID));
		// 			droppedResourceID = *((GUID*)payload->Data);
		// 			droppedResource = true;
		// 		}
		// 		ImGui::EndDragDropTarget();
		// 	}
		//
		// 	if (droppedResource)
		// 	{
		// 		droppedResource = false;
		// 		const auto& resource = pResourceCache->GetResource(droppedResourceID);
		// 		if (resource->GetType() == Content::ResourceType::ResourceTypeMaterial)
		// 		{
		// 			auto newMaterial = pResourceCache->CreateHandle<Content::MaterialResource>(droppedResourceID);
		// 			pComponent->SetMaterial(0, newMaterial);
		// 		}
		// 	}
		// }
	}
	ImGui::EndTable();
}
