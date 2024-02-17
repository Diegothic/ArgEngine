#include <arg_pch.hpp>
#include "ContentBrowserPanel.hpp"

#include <imgui/ImGui.hpp>

#include "Editor.hpp"
#include "Content/ResourceCache.hpp"
#include "Content/Import/SkeletalAnimationImporter.hpp"
#include "Content/Import/SkeletalModelImporter.hpp"
#include "Content/Import/SkeletonImporter.hpp"
#include "Content/Import/SoundImporter.hpp"
#include "Content/Import/TextureImporter.hpp"
#include "Content/Import/StaticModelImporter.hpp"
#include "Dialog/FileDialog/FileOpenDialog.hpp"
#include "Dialog/MessageBox/MessageBoxDialog.hpp"

void Arg::Editor::GUI::ContentBrowserPanel::OnInitialize(const EditorGUIContext& context)
{
	Editor* pEditor = context.pEditor;
	auto& pResourceCache = pEditor->GetResourceCache();

	if (pEditor->IsProjectOpened())
	{
		auto& project = pEditor->GetProject();
		project->GetContent();
		m_pOpenedFolder = project->GetContent()->GetRootFolder();
	}
	else
	{
		m_pOpenedFolder = pEditor->GetContent()->GetRootFolder();
	}

	m_FolderOpenIcon = pResourceCache->CreateHandle<Content::TextureResource>(
		"_Editor\\Icons\\icon_folder_open"
	);
	m_FolderOpenIcon.AddRef();

	m_FolderClosedIcon = pResourceCache->CreateHandle<Content::TextureResource>(
		"_Editor\\Icons\\icon_folder_closed"
	);
	m_FolderClosedIcon.AddRef();

	m_FileIcon = pResourceCache->CreateHandle<Content::TextureResource>(
		"_Editor\\Icons\\icon_file"
	);
	m_FileIcon.AddRef();

	m_FileIconMap = pResourceCache->CreateHandle<Content::TextureResource>(
		"_Editor\\Icons\\icon_file_map"
	);
	m_FileIconMap.AddRef();

	m_FileIconTexture = pResourceCache->CreateHandle<Content::TextureResource>(
		"_Editor\\Icons\\icon_file_texture"
	);
	m_FileIconTexture.AddRef();

	m_FileIconStaticModel = pResourceCache->CreateHandle<Content::TextureResource>(
		"_Editor\\Icons\\icon_file_static_model"
	);
	m_FileIconStaticModel.AddRef();

	m_FileIconMaterial = pResourceCache->CreateHandle<Content::TextureResource>(
		"_Editor\\Icons\\icon_file_material"
	);
	m_FileIconMaterial.AddRef();

	m_FileIconSkeleton = pResourceCache->CreateHandle<Content::TextureResource>(
		"_Editor\\Icons\\icon_file_skeleton"
	);
	m_FileIconSkeleton.AddRef();

	m_FileIconSkeletalModel = pResourceCache->CreateHandle<Content::TextureResource>(
		"_Editor\\Icons\\icon_file_skeletal_model"
	);
	m_FileIconSkeletalModel.AddRef();

	m_FileIconSkeletalAnimation = pResourceCache->CreateHandle<Content::TextureResource>(
		"_Editor\\Icons\\icon_file_skeletal_animation"
	);
	m_FileIconSkeletalAnimation.AddRef();

	m_FileIconSound = pResourceCache->CreateHandle<Content::TextureResource>(
		"_Editor\\Icons\\icon_file_sound"
	);
	m_FileIconSound.AddRef();
}

void Arg::Editor::GUI::ContentBrowserPanel::OnDraw(const EditorGUIContext& context)
{
	Editor* pEditor = context.pEditor;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	const bool isOpen = ImGui::Begin("Content Browser", &GetIsOpened());
	ImGui::PopStyleVar();
	if (isOpen)
	{
		DrawBrowser(context);
	}

	ImGui::End();
}

void Arg::Editor::GUI::ContentBrowserPanel::DrawBrowser(
	const EditorGUIContext& context
)
{
	Editor* pEditor = context.pEditor;
	GameEngine* pGameEngine = pEditor->GetGameEngine().get();
	const bool isProjectOpended = pEditor->IsProjectOpened();
	auto& pResourceCache = isProjectOpended
		                       ? pEditor->GetProject()->GetResourceCache()
		                       : pEditor->GetResourceCache();
	auto& pContent = isProjectOpended
		                 ? pEditor->GetProject()->GetContent()
		                 : pEditor->GetContent();

	const auto& folderOpenIconTexture = m_FolderOpenIcon.Get()->GetTexture();
	const auto& fileIconTexture = m_FileIcon.Get()->GetTexture();

	if (ImGui::BeginTable(
			"Content",
			2,
			ImGuiTableFlags_BordersInnerV
			| ImGuiTableFlags_Resizable,
			ImVec2(0.0f, ImGui::GetWindowSize().y - ImGui::GetFrameHeight()))
	)
	{
		if (ImGui::TableNextColumn())
		{
			const auto rootFolder = pContent->GetRootFolder();
			const bool isOpen = ImGui::CollapsingHeader(
				rootFolder->GetName().c_str(),
				ImGuiTreeNodeFlags_DefaultOpen
				| ImGuiTreeNodeFlags_OpenOnArrow
				| ImGuiTreeNodeFlags_OpenOnDoubleClick
				| ImGuiTreeNodeFlags_FramePadding
				| ImGuiTreeNodeFlags_AllowOverlap
			);

			const float columnWidth = ImGui::GetColumnWidth();
			ImGui::SameLine(columnWidth - 110.0f);
			if (ImGui::Button("New Folder", ImVec2(100.0f, 24.0f)))
			{
				pContent->CreateFolder(rootFolder);
			}

			if (isOpen)
			{
				if (ImGui::BeginChild("##ContentFolders"))
				{
					DrawFolderTree(context, rootFolder, 0);
				}

				ImGui::EndChild();
			}
		}

		if (ImGui::TableNextColumn())
		{
			if (m_pOpenedFolder != nullptr && m_pOpenedFolder->GetID() != GUID::Empty)
			{
				const float columnWidth = ImGui::GetColumnWidth();
				const Vec3 folderColor = m_pOpenedFolder->GetColor();
				ImGui::PushStyleColor(
					ImGuiCol_Header,
					ImVec4(folderColor.r, folderColor.g, folderColor.b, 0.5f)
				);
				ImGui::PushStyleColor(
					ImGuiCol_HeaderHovered,
					ImVec4(folderColor.r, folderColor.g, folderColor.b, 0.5f)
				);
				ImGui::PushStyleColor(
					ImGuiCol_HeaderActive,
					ImVec4(folderColor.r, folderColor.g, folderColor.b, 0.5f)
				);
				const bool isOpen = ImGui::CollapsingHeader("##FolderHeader",
				                                            ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Leaf |
				                                            ImGuiTreeNodeFlags_AllowOverlap);
				ImGui::PopStyleColor(3);

				ImGui::SameLine(5.0f);
				const uint32_t imageID = folderOpenIconTexture->GetRendererID();
				ImGui::Image((void*)(intptr_t)imageID, ImVec2(25.0f, 25.0f), ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f),
				             ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
				ImGui::SameLine(35.0f);
				ImGui::Text(m_pOpenedFolder->GetName().c_str());

				const ImVec2 cursorPos = ImGui::GetCursorPos();
				ImGui::SetCursorPos(ImVec2(cursorPos.x + columnWidth - 450.0f, cursorPos.y - 29.0f));
				if (ImGui::Button("+ Import", ImVec2(100.0f, 24.0f)))
				{
					ImGui::OpenPopup(ImGui::GetID("##ImportContextMenu"));
				}

				if (ImGui::BeginPopupContextItem("##ImportContextMenu"))
				{
					if (ImGui::MenuItem("Texture"))
					{
						std::filesystem::path path;
						const bool bIsSuccess = Dialog::FileOpenDialog::GetFile(path);
						if (bIsSuccess)
						{
							if (path.has_extension()
								&& (path.extension() == ".png"
									|| path.extension() == ".jpg"
									|| path.extension() == ".tga"
									|| path.extension() == ".bmp"
								)
							)
							{
								const auto currentFolderPath = m_pOpenedFolder->GetFullPath();
								auto pathNoExtension = path;
								pathNoExtension.replace_extension("");
								const auto fileName = pathNoExtension.filename().string();
								auto resourceName = fileName;
								if (std::filesystem::exists(currentFolderPath / resourceName))
								{
									for (size_t i = 0; i < 999; i++)
									{
										resourceName = std::format("{} ({})", fileName, i);
										if (!std::filesystem::exists(currentFolderPath / resourceName))
										{
											break;
										}
									}
								}

								Import::TextureImporter importer;
								bool bImported = importer.ImportFile(path.string());
								if (!bImported)
								{
									Dialog::MessageBoxDialog::ShowWarning("Failed to import file!");
								}
								else
								{
									importer.Save(resourceName, currentFolderPath);

									auto resource = pContent->CreateResource(
										resourceName,
										Content::ResourceType::ResourceTypeTexture,
										m_pOpenedFolder
									);

									if (pEditor->IsProjectOpened())
									{
										auto& project = pEditor->GetProject();
										project->Save();
									}
								}
							}
							else
							{
								Dialog::MessageBoxDialog::ShowWarning("Invalid file!");
							}
						}
					}

					ImGui::Separator();

					if (ImGui::MenuItem("Static Model"))
					{
						std::unique_ptr<Import::IResourceImporter> importer
							= std::make_unique<Import::StaticModelImporter>();
						ImportResource(
							context,
							importer.get(),
							Content::ResourceType::ResourceTypeStaticModel,
							{".fbx", ".gltf", ".glb"}
						);
					}

					ImGui::Separator();

					if (ImGui::MenuItem("Skeletal Model"))
					{
						auto importer = std::make_unique<Import::SkeletalModelImporter>();
						ImportResource(
							context,
							importer.get(),
							Content::ResourceType::ResourceTypeSkeletalModel,
							{".fbx", ".gltf", ".glb"}
						);
					}

					if (ImGui::MenuItem("Skeleton"))
					{
						auto importer = std::make_unique<Import::SkeletonImporter>();
						ImportResource(
							context,
							importer.get(),
							Content::ResourceType::ResourceTypeSkeleton,
							{".fbx", ".gltf", ".glb"}
						);
					}

					if (ImGui::MenuItem("Skeletal Animations"))
					{
						auto animationImporter = std::make_unique<Import::SkeletalAnimationImporter>();
						ImportAnimations(
							context,
							animationImporter.get(),
							Content::ResourceType::ResourceTypeSkeletalAnimation,
							{".fbx", ".gltf", ".glb"}
						);
					}

					ImGui::Separator();

					if (ImGui::MenuItem("Sound"))
					{
						auto soundImporter = std::make_unique<Import::SoundImporter>();
						ImportResource(
							context,
							soundImporter.get(),
							Content::ResourceType::ResourceTypeSound,
							{".wav", ".ogg"}
						);
					}

					ImGui::EndPopup();
				}

				ImGui::SetCursorPos(ImVec2(cursorPos.x + columnWidth - 340.0f, cursorPos.y - 29.0f));
				if (ImGui::Button("+ Create", ImVec2(100.0f, 24.0f)))
				{
					ImGui::OpenPopup(ImGui::GetID("##CreateContextMenu"));
				}

				if (ImGui::BeginPopupContextItem("##CreateContextMenu"))
				{
					if (ImGui::MenuItem("Map"))
					{
						auto resource = pContent->CreateResource(
							"New Map",
							Content::ResourceType::ResourceTypeWorld,
							m_pOpenedFolder
						);

						if (pEditor->IsProjectOpened())
						{
							auto& project = pEditor->GetProject();
							project->Save();
						}
					}

					ImGui::Separator();

					if (ImGui::MenuItem("Material"))
					{
						auto resource = pContent->CreateResource(
							"New Material",
							Content::ResourceType::ResourceTypeMaterial,
							m_pOpenedFolder
						);

						if (pEditor->IsProjectOpened())
						{
							auto& project = pEditor->GetProject();
							project->Save();
						}
					}

					ImGui::EndPopup();
				}

				ImGui::SetCursorPos(ImVec2(cursorPos.x + columnWidth - 200.0f, cursorPos.y - 29.0f));
				static float zoom = 1.0f;
				ImGui::PushItemWidth(200.0f);
				ImGui::SliderFloat("##Zoom", &zoom, 0.0f, 1.0f);

				const ImVec2 itemSize(150.0f * (zoom * 0.3f + 0.7f), 190.0f * (zoom * 0.3f + 0.7f));
				const ImVec2 itemOffset(5.0f, 5.0f);

				const int32_t itemsInRow = static_cast<int32_t>(columnWidth / (itemSize.x + itemOffset.x));

				if (ImGui::BeginChild("##ResourceItems"))
				{
					if (ImGui::BeginTable(
						"Items",
						itemsInRow,
						ImGuiTableFlags_NoBordersInBody
					))
					{
						for (size_t i = 0; i < m_pOpenedFolder->GetResourceCount(); i++)
						{
							ImGui::PushID(static_cast<int32_t>(i));

							const auto& resource = m_pOpenedFolder->GetResource(i);
							const auto& ID = resource->GetID();
							const auto resourceName = resource->GetName().c_str();

							const bool bIsSelected = pEditor->HasSelectedResource()
								&& pEditor->GetSelectedResource() == resource;

							if (ImGui::TableNextColumn())
							{
								auto cursorPos = ImGui::GetCursorPos();
								cursorPos.x += itemOffset.x;
								cursorPos.y += itemOffset.y;
								ImGui::SetCursorPos(cursorPos);

								if (bIsSelected)
								{
									ImGui::PushStyleColor(
										ImGuiCol_Button,
										ImVec4(0.27f, 0.84f, 0.93f, 0.3f)
									);
									ImGui::PushStyleColor(
										ImGuiCol_ButtonHovered,
										ImVec4(0.37f, 0.94f, 1.0f, 0.3f)
									);
									ImGui::PushStyleColor(
										ImGuiCol_ButtonActive,
										ImVec4(0.17f, 0.74f, 0.83f, 0.3f)
									);
								}
								ImGui::Button("##ItemButton", itemSize);
								if (bIsSelected)
								{
									ImGui::PopStyleColor(3);
								}

								if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Right))
								{
									ImGui::OpenPopup(ImGui::GetID("##ResourceContextMenu"));
								}

								if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
								{
									pEditor->SelectResource(ID);
								}

								if (ImGui::BeginDragDropSource())
								{
									ImGui::SetDragDropPayload("Resource", &ID, sizeof(GUID));
									ImGui::Text(resourceName);
									ImGui::EndDragDropSource();
								}

								static bool isRenameResource = false;
								static GUID renamedResource;
								if (ImGui::BeginPopupContextItem("##ResourceContextMenu"))
								{
									if (resource->GetType() == Content::ResourceType::ResourceTypeWorld)
									{
										if (ImGui::MenuItem("Open"))
										{
											pEditor->DeselectActor();
											pEditor->DeselectResource();
											pGameEngine->LoadWorld(resource->GetID());
											pEditor->GetCamera()->Reset();
										}

										ImGui::Separator();
									}

									if (ImGui::MenuItem("Rename"))
									{
										isRenameResource = true;
										renamedResource = ID;
									}

									ImGui::Separator();

									if (ImGui::MenuItem("Remove"))
									{
										pEditor->DeselectResource();
										pContent->RemoveResource(resource, m_pOpenedFolder);
									}

									ImGui::EndPopup();
								}

								const ImVec2 imagePos = ImVec2(
									cursorPos.x + (itemSize.x * 0.15f),
									cursorPos.y + (itemSize.x * 0.15f)
								);
								ImGui::SetCursorPos(imagePos);
								uint32_t imageID = fileIconTexture->GetRendererID();
								switch (resource->GetType())
								{
								case Content::ResourceType::ResourceTypeWorld:
									{
										const auto& resourceIconTexture = m_FileIconMap.Get()->GetTexture();
										imageID = resourceIconTexture->GetRendererID();
										break;
									}
								case Content::ResourceType::ResourceTypeTexture:
									{
										const auto& resourceIconTexture = m_FileIconTexture.Get()->GetTexture();
										imageID = resourceIconTexture->GetRendererID();
										break;
									}
								case Content::ResourceType::ResourceTypeStaticModel:
									{
										const auto& resourceIconTexture = m_FileIconStaticModel.Get()->GetTexture();
										imageID = resourceIconTexture->GetRendererID();
										break;
									}
								case Content::ResourceType::ResourceTypeMaterial:
									{
										const auto& resourceIconTexture = m_FileIconMaterial.Get()->GetTexture();
										imageID = resourceIconTexture->GetRendererID();
										break;
									}
								case Content::ResourceType::ResourceTypeSkeleton:
									{
										const auto& resourceIconTexture = m_FileIconSkeleton.Get()->GetTexture();
										imageID = resourceIconTexture->GetRendererID();
										break;
									}
								case Content::ResourceType::ResourceTypeSkeletalModel:
									{
										const auto& resourceIconTexture = m_FileIconSkeletalModel.Get()->GetTexture();
										imageID = resourceIconTexture->GetRendererID();
										break;
									}
								case Content::ResourceType::ResourceTypeSkeletalAnimation:
									{
										const auto& resourceIconTexture = m_FileIconSkeletalAnimation.Get()->
											GetTexture();
										imageID = resourceIconTexture->GetRendererID();
										break;
									}
								case Content::ResourceType::ResourceTypeSound:
									{
										const auto& resourceIconTexture = m_FileIconSound.Get()->GetTexture();
										imageID = resourceIconTexture->GetRendererID();
										break;
									}
								}

								ImGui::Image(
									(void*)(intptr_t)imageID,
									ImVec2(itemSize.x * 0.7f, itemSize.x * 0.7f),
									ImVec2(0.0f, 1.0f),
									ImVec2(1.0f, 0.0f),
									ImVec4(1.0f, 1.0f, 1.0f, 1.0f)
								);

								const auto textSize = ImGui::CalcTextSize(resourceName, nullptr, false, itemSize.x - (itemSize.x * 0.1f));
								const ImVec2 textPos = ImVec2(
									cursorPos.x + (itemSize.x * 0.5f) - (textSize.x * 0.5f),
									cursorPos.y + (itemSize.x * 0.85f)
								);

								ImGui::SetCursorPos(textPos);
								ImGui::PushTextWrapPos(cursorPos.x + itemSize.x - (itemSize.x * 0.05f));
								if (isRenameResource && renamedResource == ID)
								{
									ImGui::SetKeyboardFocusHere();
									char buffer[1024];
									strcpy_s(buffer, resourceName);
									ImGui::InputText(
										"##ResourceNewName",
										buffer,
										1024,
										ImGuiInputTextFlags_CallbackAlways,
										[](ImGuiInputTextCallbackData* data) -> int32_t
										{
											if (data->BufTextLen < 1)
											{
												return 0;
											}

											if (ImGui::IsKeyDown(ImGuiKey_Escape))
											{
												isRenameResource = false;
												return 1;
											}

											if (ImGui::IsKeyDown(ImGuiKey_Enter))
											{
												Content::ResourceCache* resourceCache = (Content::ResourceCache*)
													data->UserData;
												const auto& resource = resourceCache->GetResource(renamedResource);
												const std::string newName(data->Buf);
												if (!std::filesystem::exists(resource->GetPath() / newName))
												{
													resourceCache->RenameResource(renamedResource, newName);
												}
												isRenameResource = false;
												return 1;
											}

											return 0;
										},
										pResourceCache.get());
								}
								else
								{
									ImGui::Text(resourceName);
								}

								ImGui::PopTextWrapPos();
							}

							ImGui::PopID();
						}
					}

					ImGui::EndTable();
				}
				ImGui::EndChild();
			}
			else
			{
				ImGui::Text("No folder opened");
			}
		}
	}

	ImGui::EndTable();
}

void Arg::Editor::GUI::ContentBrowserPanel::DrawFolderTree(
	const EditorGUIContext& context,
	const std::shared_ptr<Content::ResourceFolder>& folder,
	const int32_t treeLevel
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

	const auto& folderOpenTexture = m_FolderOpenIcon.Get()->GetTexture();
	const auto& folderClosedTexture = m_FolderClosedIcon.Get()->GetTexture();

	for (auto i = 0; i < folder->GetSubfolderCount(); i++)
	{
		ImGui::PushID(i);
		const auto subfolder = folder->GetSubfolder(i);
		ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow
			| ImGuiTreeNodeFlags_OpenOnDoubleClick
			| ImGuiTreeNodeFlags_FramePadding
			| ImGuiTreeNodeFlags_Framed;
		const bool isLeaf = subfolder->GetSubfolderCount() < 1;
		if (isLeaf)
		{
			nodeFlags |= ImGuiTreeNodeFlags_Leaf;
		}

		const Vec3 folderColor = subfolder->GetColor();
		ImGui::PushStyleColor(
			ImGuiCol_Header,
			ImVec4(0.12f, 0.12f, 0.12f, 0.5f)
		);
		ImGui::PushStyleColor(
			ImGuiCol_HeaderHovered,
			ImVec4(0.32f, 0.32f, 0.32f, 0.5f)
		);
		const bool isOpen = ImGui::TreeNodeEx("##FolderNode", nodeFlags);
		ImGui::PopStyleColor(2);

		if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
		{
			m_pOpenedFolder = subfolder;
		}

		if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Right))
		{
			ImGui::OpenPopup(ImGui::GetID("##FolderContextMenu"));
		}

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
			pContent->MoveResource(resource, m_pOpenedFolder, subfolder);
		}

		static bool isRename = false;
		static GUID renamedFolder;
		static bool isEditColor = false;
		if (ImGui::BeginPopupContextItem("##FolderContextMenu"))
		{
			if (ImGui::MenuItem("Rename"))
			{
				isRename = true;
				renamedFolder = subfolder->GetID();
			}

			if (ImGui::MenuItem("Change color"))
			{
				isEditColor = true;
			}

			ImGui::Separator();

			if (ImGui::MenuItem("New folder"))
			{
				pContent->CreateFolder(subfolder);
			}

			ImGui::Separator();

			if (ImGui::MenuItem("Remove"))
			{
				pContent->RemoveFolder(subfolder);
				m_pOpenedFolder = folder;
			}

			ImGui::EndPopup();
		}

		static Vec3 color;

		if (ImGui::BeginPopup("##ColorPopup"))
		{
			ImGui::ColorPicker3("##FolderColor", Math::ValuePtr(color));
			if (ImGui::IsKeyDown(ImGuiKey_Escape))
			{
				ImGui::CloseCurrentPopup();
			}

			if (ImGui::IsKeyDown(ImGuiKey_Enter))
			{
				subfolder->ChangeColor(color);
				pResourceCache->SaveResource(subfolder->GetID());
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}

		if (isEditColor)
		{
			ImGui::OpenPopup(ImGui::GetID("##ColorPopup"));
			isEditColor = false;
			color = subfolder->GetColor();
		}

		ImGui::SameLine(treeLevel * 20.0f + 30.0f);
		const uint32_t imageID = (isOpen && !isLeaf)
			                         ? folderOpenTexture->GetRendererID()
			                         : folderClosedTexture->GetRendererID();
		ImGui::Image((void*)(intptr_t)imageID, ImVec2(25.0f, 25.0f), ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f),
		             ImVec4(folderColor.r, folderColor.g, folderColor.b, 1.0f));
		ImGui::SameLine(treeLevel * 20.0f + 30.0f + 35.0f);
		static bool doRename = false;
		static std::string newFolderName;
		if (isRename && renamedFolder == subfolder->GetID())
		{
			ImGui::SetKeyboardFocusHere();
			char buffer[1024];
			strcpy_s(buffer, subfolder->GetName().c_str());
			ImGui::InputText("##FolderNewName", buffer, 1024, ImGuiInputTextFlags_CallbackAlways,
			                 [](ImGuiInputTextCallbackData* data) -> int32_t
			                 {
				                 if (data->BufTextLen < 1)
				                 {
					                 return 0;
				                 }

				                 if (ImGui::IsKeyDown(ImGuiKey_Escape))
				                 {
					                 isRename = false;
					                 return 1;
				                 }

				                 if (ImGui::IsKeyDown(ImGuiKey_Enter))
				                 {
					                 newFolderName = data->Buf;
					                 doRename = true;
					                 isRename = false;
					                 return 1;
				                 }

				                 return 0;
			                 }
			);
		}
		else
		{
			ImGui::Text(subfolder->GetName().c_str());
		}

		if (doRename)
		{
			doRename = false;
			pContent->RenameFolder(subfolder, newFolderName);
		}

		if (isOpen)
		{
			DrawFolderTree(context, subfolder, treeLevel + 1);

			ImGui::TreePop();
		}
		ImGui::PopID();
	}
}

void Arg::Editor::GUI::ContentBrowserPanel::ImportResource(
	const EditorGUIContext& context,
	Import::IResourceImporter* pImporter,
	const Content::ResourceType& resourceType,
	const std::vector<std::string>& validExtensions
) const
{
	Editor* pEditor = context.pEditor;
	const bool isProjectOpened = pEditor->IsProjectOpened();
	const auto& pContent = isProjectOpened
		                       ? pEditor->GetProject()->GetContent()
		                       : pEditor->GetContent();

	std::filesystem::path path;
	const bool isSuccess = Dialog::FileOpenDialog::GetFile(path);
	if (!isSuccess
		|| !path.has_extension()
		|| std::ranges::find(validExtensions,
		                     path.extension()
		) == validExtensions.end())
	{
		Dialog::MessageBoxDialog::ShowWarning("Invalid file!");
		return;
	}

	const auto currentFolderPath = m_pOpenedFolder->GetFullPath();
	auto pathNoExtension = path;
	pathNoExtension.replace_extension("");
	const auto fileName = pathNoExtension.filename().string();
	auto resourceName = fileName;
	if (std::filesystem::exists(currentFolderPath / resourceName))
	{
		for (size_t i = 0; i < 999; i++)
		{
			resourceName = std::format("{} ({})", fileName, i);
			if (!std::filesystem::exists(currentFolderPath / resourceName))
			{
				break;
			}
		}
	}

	if (std::filesystem::exists(currentFolderPath / resourceName))
	{
		Dialog::MessageBoxDialog::ShowWarning("Items limit exceeded!");
		return;
	}

	const bool bImported = pImporter->ImportFile(path.string());
	if (!bImported)
	{
		Dialog::MessageBoxDialog::ShowWarning("Failed to import file!");
		return;
	}

	pImporter->Save(resourceName, currentFolderPath);

	auto resource = pContent->CreateResource(
		resourceName,
		resourceType,
		m_pOpenedFolder
	);

	if (pEditor->IsProjectOpened())
	{
		const auto& project = pEditor->GetProject();
		project->Save();
	}
}

void Arg::Editor::GUI::ContentBrowserPanel::ImportAnimations(
	const EditorGUIContext& context,
	Import::SkeletalAnimationImporter* pImporter,
	const Content::ResourceType& resourceType,
	const std::vector<std::string>& validExtensions
) const
{
	Editor* pEditor = context.pEditor;
	const bool isProjectOpened = pEditor->IsProjectOpened();
	const auto& pContent = isProjectOpened
		                       ? pEditor->GetProject()->GetContent()
		                       : pEditor->GetContent();

	std::filesystem::path path;
	const bool isSuccess = Dialog::FileOpenDialog::GetFile(path);
	if (!isSuccess
		|| !path.has_extension()
		|| std::ranges::find(validExtensions,
		                     path.extension()
		) == validExtensions.end())
	{
		Dialog::MessageBoxDialog::ShowWarning("Invalid file!");
		return;
	}


	const auto currentFolderPath = m_pOpenedFolder->GetFullPath();
	auto pathNoExtension = path;
	pathNoExtension.replace_extension("");
	const auto fileName = pathNoExtension.filename().string();

	pImporter->ScanFile(path.string());
	for (size_t i = 0; i < pImporter->GetAnimationCount(); i++)
	{
		pImporter->SetIndex(i);
		const std::string& animationName = pImporter->GetAnimationName(i);

		auto resourceName = std::format("{}_{}", fileName, animationName);
		if (std::filesystem::exists(currentFolderPath / resourceName))
		{
			for (size_t i = 0; i < 999; i++)
			{
				resourceName = std::format("{}_{} ({})", fileName, animationName, i);
				if (!std::filesystem::exists(currentFolderPath / resourceName))
				{
					break;
				}
			}
		}

		if (std::filesystem::exists(currentFolderPath / resourceName))
		{
			Dialog::MessageBoxDialog::ShowWarning("Items limit exceeded!");
			return;
		}

		const bool bImported = pImporter->ImportFile(path.string());
		if (!bImported)
		{
			Dialog::MessageBoxDialog::ShowWarning("Failed to import file!");
			return;
		}

		pImporter->Save(resourceName, currentFolderPath);

		auto resource = pContent->CreateResource(
			resourceName,
			resourceType,
			m_pOpenedFolder
		);
	}

	if (pEditor->IsProjectOpened())
	{
		const auto& project = pEditor->GetProject();
		project->Save();
	}
}
