#pragma once

#include <arg_pch.hpp>

#include "imgui/imgui.h"

#include "Content/Content.hpp"
#include "Content/Import/TextureImporter.hpp"
#include "FileDialog/FileOpenDialog.hpp"

namespace Arg
{
	namespace Editor
	{
		namespace GUI
		{
			class ContentBrowser
			{
			public:

				void Initialize(
					const std::shared_ptr<Content::Content>& content,
					const std::shared_ptr<Content::ResourceCache>& resourceCache
				)
				{
					openedFolder = content->GetRootFolder();
					folderOpenTexture = resourceCache->CreateHandle<Content::TextureResource>("_Engine\\Icons\\folder_open");
					folderClosedTexture = resourceCache->CreateHandle<Content::TextureResource>("_Engine\\Icons\\folder_closed");
				}

				void Open()
				{
					bIsOpened = true;
				}

				void Draw(
					const std::shared_ptr<Content::Content>& content,
					std::shared_ptr<Content::ResourceCache>& resourceCache
				)
				{
					if (!bIsOpened)
					{
						return;
					}

					ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
					const bool isOpen = ImGui::Begin("Content Browser", &bIsOpened);
					ImGui::PopStyleVar();
					if (isOpen)
					{
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
								const auto rootFolder = content->GetRootFolder();
								const bool isOpen = ImGui::CollapsingHeader(rootFolder->GetName().c_str(),
									ImGuiTreeNodeFlags_DefaultOpen
									| ImGuiTreeNodeFlags_OpenOnArrow
									| ImGuiTreeNodeFlags_OpenOnDoubleClick
									| ImGuiTreeNodeFlags_FramePadding
									| ImGuiTreeNodeFlags_AllowOverlap
								);

								const float columnWidth = ImGui::GetColumnWidth();
								ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x + columnWidth - 25.0f, ImGui::GetCursorPos().y - 26.0f));
								if (ImGui::Button("+##CreateFolder", ImVec2(20.0f, 20.0f)))
								{
									content->CreateFolder(rootFolder);
								}

								if (isOpen)
								{
									DrawFolderTree(content, resourceCache, rootFolder, 0);
								}
							}

							if (ImGui::TableNextColumn())
							{
								const float columnWidth = ImGui::GetColumnWidth();
								const Vec3 folderColor = openedFolder->GetColor();
								ImGui::PushStyleColor(
									ImGuiCol_Header,
									ImVec4(folderColor.r, folderColor.g, folderColor.b, 0.5f)
								);
								const bool isOpen = ImGui::CollapsingHeader("##FolderHeader", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_AllowOverlap);
								ImGui::PopStyleColor();

								ImGui::SameLine(5.0f);
								const uint32_t imageID = folderOpenTexture.Get()->GetRendererID();
								ImGui::Image((void*)(intptr_t)imageID, ImVec2(25.0f, 25.0f), ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f), ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
								ImGui::SameLine(35.0f);
								ImGui::Text(openedFolder->GetName().c_str());

								const ImVec2 cursorPos = ImGui::GetCursorPos();
								ImGui::SetCursorPos(ImVec2(cursorPos.x + columnWidth - 350.0f, cursorPos.y - 29.0f));
								if (ImGui::Button("Import", ImVec2(100.0f, 24.0f)))
								{
									FileOpenDialog fileOpenDialog;
									std::filesystem::path path;
									const bool isSuccess = fileOpenDialog.GetFile(path);
									if (isSuccess)
									{
										if (path.has_extension() && path.extension() == ".png")
										{
											{
												const std::filesystem::path currentFolderPath = openedFolder->GetPath() / openedFolder->GetName();
												std::filesystem::path pathNoExtension = path;
												pathNoExtension.replace_extension("");
												const std::string fileName = pathNoExtension.filename().string();
												std::string resourceName = fileName;
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
												importer.ImportFile(path.string());
												importer.Save(resourceName, currentFolderPath);

												auto resource = content->CreateResource(
													resourceName,
													Content::ResourceType::ResourceTypeTexture,
													openedFolder
												);
											}
										}
									}
								}

								ImGui::SetCursorPos(ImVec2(cursorPos.x + columnWidth - 200.0f, cursorPos.y - 29.0f));
								static float zoom = 1.0f;
								ImGui::PushItemWidth(200.0f);
								ImGui::SliderFloat("##Zoom", &zoom, 0.0f, 1.0f);

								const ImVec2 itemSize(150.0f * (zoom * 0.3f + 0.7f), 150.0f * (zoom * 0.3f + 0.7f));
								const ImVec2 itemOffset(5.0f, 5.0f);

								const int32_t itemsInRow = columnWidth / (itemSize.x + itemOffset.x);

								if (ImGui::BeginTable(
									"Items",
									itemsInRow,
									ImGuiTableFlags_NoBordersInBody
								))
								{
									for (size_t i = 0; i < openedFolder->GetResourceCount(); i++)
									{
										ImGui::PushID(i);

										const auto& resource = openedFolder->GetResource(i);
										const auto& ID = resource->GetID();
										const auto resourceName = resource->GetName().c_str();

										if (ImGui::TableNextColumn())
										{
											auto cursorPos = ImGui::GetCursorPos();
											cursorPos.x += itemOffset.x;
											cursorPos.y += itemOffset.y;
											ImGui::SetCursorPos(cursorPos);
											ImGui::Button("##ItemButton", itemSize);

											if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Right))
											{
												ImGui::OpenPopup(ImGui::GetID("##ResourceContextMenu"));
											}

											if (ImGui::BeginDragDropSource())
											{
												ImGui::SetDragDropPayload("Resource", &ID, sizeof(GUID));
												ImGui::Text(resourceName);
												ImGui::EndDragDropSource();
											}

											static bool isRenameResource = false;
											static GUID renamedResource;
											if (ImGui::BeginPopupContextItem("##FolderContextMenu"))
											{
												if (ImGui::MenuItem("Rename"))
												{
													isRenameResource = true;
													renamedResource = ID;
												}

												ImGui::Separator();

												if (ImGui::MenuItem("Remove"))
												{
													content->RemoveResource(resource, openedFolder);
												}

												ImGui::EndPopup();
											}

											const ImVec2 imagePos = ImVec2(cursorPos.x + itemSize.x * 0.15f, cursorPos.y + itemSize.x * 0.10f);
											ImGui::SetCursorPos(imagePos);
											const auto imageHandle = resourceCache->CreateHandle<Content::TextureResource>(ID);
											const uint32_t imageID = imageHandle.IsValid() ? imageHandle.Get()->GetRendererID() : 0;
											ImGui::Image((void*)(intptr_t)imageID, ImVec2(itemSize.x * 0.70f, itemSize.y * 0.70f), ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f), ImVec4(1.0f, 1.0f, 1.0f, 1.0f));

											const auto textSize = ImGui::CalcTextSize(resourceName);
											const ImVec2 textPos = ImVec2(cursorPos.x + (itemSize.x * 0.5f) - (textSize.x * 0.5f), cursorPos.y + itemSize.x * 0.85f);
											ImGui::SetCursorPos(textPos);
											if (isRenameResource && renamedResource == ID)
											{
												ImGui::SetKeyboardFocusHere();
												char buffer[1024];
												strcpy_s(buffer, resourceName);
												ImGui::InputText("##FolderNewName", buffer, 1024, ImGuiInputTextFlags_CallbackAlways,
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
															Content::ResourceCache* resourceCache = (Content::ResourceCache*)data->UserData;
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
													resourceCache.get());
											}
											else
											{
												ImGui::Text(resourceName);
											}

										}

										ImGui::PopID();
									}
								}
								ImGui::EndTable();
							}
						}
						ImGui::EndTable();

					}
					ImGui::End();
				}

				void DrawFolderTree(
					const std::shared_ptr<Content::Content>& content,
					std::shared_ptr<Content::ResourceCache>& resourceCache,
					const std::shared_ptr<Content::ResourceFolder>& folder,
					const int32_t treeLevel
				)
				{
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
							ImVec4(folderColor.r, folderColor.g, folderColor.b, 0.5f)
						);
						const bool isOpen = ImGui::TreeNodeEx("##FolderNode", nodeFlags);
						if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
						{
							openedFolder = subfolder;
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
							const auto& resource = resourceCache->GetResource(droppedResourceID);
							content->MoveResource(resource, openedFolder, subfolder);
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
								content->CreateFolder(subfolder);
							}

							ImGui::Separator();

							if (ImGui::MenuItem("Remove"))
							{
								content->RemoveFolder(subfolder);
								openedFolder = folder;
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
								resourceCache->SaveResource(subfolder->GetID());
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

						ImGui::PopStyleColor();
						ImGui::SameLine(treeLevel * 20.0f + 30.0f);
						const uint32_t imageID = (isOpen && !isLeaf) ? folderOpenTexture.Get()->GetRendererID() : folderClosedTexture.Get()->GetRendererID();
						ImGui::Image((void*)(intptr_t)imageID, ImVec2(25.0f, 25.0f), ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f), ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
						ImGui::SameLine(treeLevel * 20.0f + 30.0f + 35.0f);
						static bool doRename = false;
						static std::string newFolderName;
						if (isRename && renamedFolder == subfolder->GetID())
						{
							ImGui::SetKeyboardFocusHere();
							char buffer[1024];
							strcpy_s(buffer, subfolder->GetName().c_str());
							GUID subfolderID = subfolder->GetID();
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
							content->RenameFolder(subfolder, newFolderName);
						}

						if (isOpen)
						{
							DrawFolderTree(content, resourceCache, subfolder, treeLevel + 1);

							ImGui::TreePop();
						}
						ImGui::PopID();
					}
				}

			private:
				Content::ResourceHandle<Content::TextureResource> folderOpenTexture;
				Content::ResourceHandle<Content::TextureResource> folderClosedTexture;
				std::shared_ptr<Content::ResourceFolder> openedFolder;
				bool bIsOpened = true;
			};
		}
	}
}
