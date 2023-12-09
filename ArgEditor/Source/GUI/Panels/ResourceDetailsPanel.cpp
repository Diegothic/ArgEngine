#include <arg_pch.hpp>
#include "ResourceDetailsPanel.hpp"

#include <imgui/imgui.h>

#include "Editor.hpp"

void Arg::Editor::GUI::ResourceDetailsPanel::OnInitialize(
	const EditorGUIContext& context
)
{

}

void Arg::Editor::GUI::ResourceDetailsPanel::OnDraw(
	const EditorGUIContext& context
)
{
	Editor* pEditor = context.pEditor;
	auto& pContent = pEditor->GetContent();
	auto& pResourceCache = pEditor->GetResourceCache();
	auto& pGameEngine = pEditor->GetGameEngine();

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.0f, 10.0f));
	const bool isOpen = ImGui::Begin("Resource", &GetIsOpened());
	ImGui::PopStyleVar();
	if (isOpen)
	{
		if (pEditor->HasSelectedResource())
		{
			auto& pSelectedResource = pEditor->GetSelectedResource();
			DrawResourceDetails(context, pSelectedResource);
		}
	}
	ImGui::End();
}

void Arg::Editor::GUI::ResourceDetailsPanel::DrawResourceDetails(
	const EditorGUIContext& context,
	std::shared_ptr<Content::Resource>& pResource
)
{
	const Vec2 windowSize = Vec2(
		ImGui::GetWindowSize().x,
		ImGui::GetWindowSize().y
	);

	const auto resourceID = pResource->GetID();
	const auto& resourceName = pResource->GetName();

	ImGui::PushID(static_cast<int>(resourceID));

	ImGui::Text(resourceName.c_str());
	ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "\tID: %llu", resourceID);

	const auto resourceType = pResource->GetType();
	switch (pResource->GetType())
	{
	case Content::ResourceType::ResourceTypeMaterial:
	{
		auto materialHandle = pResource->GetResourceCache()
			->CreateHandle<Content::MaterialResource>(resourceID);
		if (!materialHandle.IsValid())
		{
			break;
		}

		auto material = materialHandle.Get()->GetMaterial();
		DrawGameResourceDetails(context, material);
		break;
	}
	}

	ImGui::PopID();
}

void Arg::Editor::GUI::ResourceDetailsPanel::DrawGameResourceDetails(
	const EditorGUIContext& context,
	std::shared_ptr<Renderer::Material>& pMaterial
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

	if (ImGui::CollapsingHeader("Material",
		ImGuiTreeNodeFlags_DefaultOpen)
		)
	{
		if (ImGui::BeginTable(
			"##MaterialTable",
			2,
			ImGuiTableFlags_BordersInnerV
			| ImGuiTableFlags_BordersOuter
			| ImGuiTableFlags_NoSavedSettings
			| ImGuiTableFlags_SizingFixedFit
		))
		{
			// Diffuse map
			{
				ImGui::TableNextColumn();
				ImGui::Dummy(ImVec2(100.0f, 0.0f));

				ImGui::Text("Diffuse Texture");

				ImGui::TableNextColumn();

				const auto cursorPos = ImGui::GetCursorPos();
				ImGui::Button(
					"##DiffuseMapHandle",
					ImVec2(ImGui::GetWindowWidth() - 160.0f, 25.0f)
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
						if (resource->GetType() == Content::ResourceType::ResourceTypeTexture)
						{
							const auto diffuseMap = pResourceCache
								->CreateHandle<Content::TextureResource>(
									droppedResourceID
								);
							pMaterial->SetDiffuseMap(diffuseMap);
						}
					}
				}

				ImGui::SetCursorPos(ImVec2(cursorPos.x + ImGui::GetWindowWidth() - 160.0f, cursorPos.y));
				if (ImGui::Button(
					"X",
					ImVec2(25.0f, 25.0f)
				))
				{
					const auto diffuseMap = pResourceCache->CreateHandle<Content::TextureResource>(GUID::Empty);
					pMaterial->SetDiffuseMap(diffuseMap);
				}

				ImGui::SetCursorPos(ImVec2(cursorPos.x + 10.0f, cursorPos.y + 5.0f));

				const auto diffuseMap = pMaterial->GetDiffuseMap();
				if (diffuseMap.IsValid())
				{
					const auto& diffuseMapName = diffuseMap.Get()->GetName();
					ImGui::Text(diffuseMapName.c_str());
				}
				else
				{
					ImGui::TextDisabled("Null reference");
				}
			}

			// Diffuse color
			{
				ImGui::TableNextColumn();
				ImGui::Dummy(ImVec2(100.0f, 0.0f));

				ImGui::Text("Diffuse");

				ImGui::TableNextColumn();

				static Vec4 diffuseColor = Vec4(1.0f);
				diffuseColor = pMaterial->GetDiffuseColor();
				ImGui::ColorEdit4(
					"##DiffuseColor",
					Math::ValuePtr(diffuseColor),
					ImGuiColorEditFlags_Float
					| ImGuiColorEditFlags_HDR
				);
				pMaterial->SetDiffuseColor(diffuseColor);
			}

			// Specular
			{
				ImGui::TableNextColumn();
				ImGui::Dummy(ImVec2(100.0f, 0.0f));

				ImGui::Text("Specular");

				ImGui::TableNextColumn();

				static float specular = 1.0f;
				specular = pMaterial->GetSpecular();
				ImGui::SliderFloat("##Specular", &specular, 0.0f, 1.0f);
				pMaterial->SetSpecular(specular);
			}

			// Specular
			//{
			//	ImGui::TableNextColumn();
			//	ImGui::Dummy(ImVec2(100.0f, 0.0f));

			//	ImGui::Text("Shininess");

			//	ImGui::TableNextColumn();

			//	static float shininess = 1.0f;
			//	shininess = pMaterial->GetSpecular();
			//	ImGui::DragFloat("#Specular", &specular, 0.1f, 0.0f, 1.0f);
			//	pMaterial->SetSpecular(specular);
			//}
		}
		ImGui::EndTable();
	}
}
