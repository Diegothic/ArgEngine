#include <arg_pch.hpp>
#include "ResourceDetailsPanel.hpp"

#include <imgui/ImGui.hpp>

#include "Editor.hpp"
#include "GUI/Elements/Properties.hpp"

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
	const bool isProjectOpened = pEditor->IsProjectOpened();
	auto& pResourceCache = isProjectOpened
		                       ? pEditor->GetProject()->GetResourceCache()
		                       : pEditor->GetResourceCache();
	auto& pContent = isProjectOpened
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

				const auto diffuseMap = pMaterial->GetDiffuseMap();
				ResourceHandleProperty(
					"##DiffuseMapHandle",
					Vec2(ImGui::GetWindowWidth() - 160.0f, 25.0f),
					diffuseMap.IsValid() ? diffuseMap.Get()->GetName().c_str() : nullptr,
					[&](GUID droppedResourceID)
					{
						const auto& resource = pResourceCache->GetResource(droppedResourceID);
						if (resource->GetType() == Content::ResourceType::ResourceTypeTexture)
						{
							pMaterial->SetDiffuseMap(
								pResourceCache
								->CreateHandle<Content::TextureResource>(
									droppedResourceID
								)
							);
						}
					},
					[&]
					{
						pMaterial->SetDiffuseMap(
							pResourceCache->CreateHandle<Content::TextureResource>(GUID::Empty)
						);
					}
				);
			}

			// Diffuse color
			{
				const auto diffuseMap = pMaterial->GetDiffuseMap();
				if (!diffuseMap.IsValid())
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
			}

			// Specular map
			{
				ImGui::TableNextColumn();
				ImGui::Dummy(ImVec2(100.0f, 0.0f));

				ImGui::Text("Specular Texture");

				ImGui::TableNextColumn();

				const auto specularMap = pMaterial->GetSpecularMap();
				ResourceHandleProperty(
					"##SpecularMapHandle",
					Vec2(ImGui::GetWindowWidth() - 160.0f, 25.0f),
					specularMap.IsValid() ? specularMap.Get()->GetName().c_str() : nullptr,
					[&](GUID droppedResourceID)
					{
						const auto& resource = pResourceCache->GetResource(droppedResourceID);
						if (resource->GetType() == Content::ResourceType::ResourceTypeTexture)
						{
							pMaterial->SetSpecularMap(
								pResourceCache
								->CreateHandle<Content::TextureResource>(
									droppedResourceID
								)
							);
						}
					},
					[&]
					{
						pMaterial->SetSpecularMap(
							pResourceCache->CreateHandle<Content::TextureResource>(GUID::Empty)
						);
						pMaterial->SetSpecular(0.5f);
					}
				);
			}

			// Specular
			{
				const auto specularMap = pMaterial->GetSpecularMap();
				if (!specularMap.IsValid())
				{
					ImGui::TableNextColumn();
					ImGui::Dummy(ImVec2(100.0f, 0.0f));

					ImGui::Text("Specular");

					ImGui::TableNextColumn();

					float specular = pMaterial->GetSpecular();
					ImGui::SliderFloat("##Specular", &specular, 0.0f, 1.0f);
					pMaterial->SetSpecular(specular);
				}
			}

			// Shininess
			{
				ImGui::TableNextColumn();
				ImGui::Dummy(ImVec2(100.0f, 0.0f));

				ImGui::Text("Shininess");

				ImGui::TableNextColumn();

				float shininess = pMaterial->GetShininess();
				ImGui::SliderFloat("##Shininess", &shininess, 0.0f, 1.0f);
				pMaterial->SetShininess(shininess);
			}

			// Shininess
			{
				ImGui::TableNextColumn();
				ImGui::Dummy(ImVec2(100.0f, 0.0f));

				ImGui::Text("Reflectivity");

				ImGui::TableNextColumn();

				float reflectivity = pMaterial->GetReflectivity();
				ImGui::SliderFloat("##Reflectivity", &reflectivity, 0.0f, 1.0f);
				pMaterial->SetReflectivity(reflectivity);
			}
		}
		ImGui::EndTable();
	}
}
