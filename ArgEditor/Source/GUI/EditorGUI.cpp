#include <arg_pch.hpp>
#include "EditorGUI.hpp"

#include <imgui/ImGui.hpp>

#include "Editor.hpp"
#include "Dialog/FileDialog/FileOpenDialog.hpp"
#include "Dialog/MessageBox/MessageBoxDialog.hpp"

Arg::Editor::GUI::EditorGUI::EditorGUI(const EditorGUISpec& spec)
	: m_ConfigFile(spec.ConfigFile.string())
{
}

void Arg::Editor::GUI::EditorGUI::Initialize(
	const EditorGUIContext& context,
	const void* pWindowHandle
)
{
	ARG_CONSOLE_LOG("Initializing ImGui");
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.IniFilename = m_ConfigFile.c_str();

	StyleColors();

	ImGui_ImplGlfw_InitForOpenGL((GLFWwindow*)pWindowHandle, true);
	ImGui_ImplOpenGL3_Init("#version 150");

	m_ContentBrowser.Initialize(context);
	m_ProjectSettings.Initialize(context);
	m_WorldOutliner.Initialize(context);
	m_Details.Initialize(context);
	m_ResourceDetails.Initialize(context);
	m_WorldSettings.Initialize(context);

	m_ContentBrowser.Open();
	m_ProjectSettings.Open();
	m_WorldOutliner.Open();
	m_Details.Open();
	m_ResourceDetails.Open();
	m_WorldSettings.Open();
}

void Arg::Editor::GUI::EditorGUI::CleanUp()
{
	ImGui_ImplGlfw_Shutdown();
	ImGui_ImplOpenGL3_Shutdown();
	ImGui::DestroyContext();
}

void Arg::Editor::GUI::EditorGUI::SetFont(
	const std::filesystem::path& fontFile,
	const float fontSize
)
{
	ImGuiIO& io = ImGui::GetIO();

	ImFontConfig fontConfig;
	fontConfig.SizePixels = fontSize;
	fontConfig.OversampleH = fontConfig.OversampleV = 3;
	fontConfig.PixelSnapH = true;

	const auto font = fontFile.string();
	io.Fonts->AddFontFromFileTTF(font.c_str(), fontSize, &fontConfig);
}

void Arg::Editor::GUI::EditorGUI::StyleColors()
{
	ImGuiStyle& style = ImGui::GetStyle();
	ImVec4* colors = style.Colors;

	style.WindowRounding = 0.0f;
	style.TabRounding = 0.0f;
	style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	style.TabBorderSize = 0.0f;
	style.WindowMenuButtonPosition = ImGuiDir_None;

	colors[ImGuiCol_Text] = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.12f, 0.12f, 0.12f, 1.0f);
	colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
	colors[ImGuiCol_Border] = ImVec4(0.35f, 0.35f, 0.35f, 0.50f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.08f, 0.08f, 0.08f, 1.0f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.06f, 0.06f, 0.06f, 1.0f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.06f, 0.06f, 0.06f, 1.0f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.08f, 0.08f, 0.08f, 1.0f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.44f, 0.27f, 0.81f, 1.00f);
	colors[ImGuiCol_CheckMark] = ImVec4(0.44f, 0.27f, 0.81f, 1.00f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.44f, 0.27f, 0.81f, 1.00f);
	colors[ImGuiCol_Button] = ImVec4(0.08f, 0.08f, 0.08f, 1.00f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.20f, 0.20f, 0.20f, 1.0f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.01f, 0.01f, 0.01f, 1.00f);
	colors[ImGuiCol_Header] = ImVec4(0.08f, 0.08f, 0.08f, 1.00f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.20f, 0.20f, 0.20f, 1.0f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.01f, 0.01f, 0.01f, 1.00f);
	colors[ImGuiCol_Separator] = colors[ImGuiCol_Border];
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.25f, 0.25f, 0.25f, 0.40f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.44f, 0.27f, 0.81f, 0.40f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.34f, 0.17f, 0.71f, 0.40f);
	colors[ImGuiCol_Tab] = ImVec4(0.20f, 0.20f, 0.20f, 1.0f);
	colors[ImGuiCol_TabHovered] = ImVec4(0.44f, 0.27f, 0.81f, 1.00f);
	colors[ImGuiCol_TabActive] = ImVec4(0.34f, 0.17f, 0.71f, 1.00f);
	colors[ImGuiCol_TabUnfocused] = ImVec4(0.20f, 0.20f, 0.20f, 1.0f);
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.34f, 0.17f, 0.71f, 1.00f);
	colors[ImGuiCol_DockingPreview] = ImVec4(0.44f, 0.27f, 0.81f, 0.60f);
	colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
	colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	colors[ImGuiCol_TableHeaderBg] = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
	colors[ImGuiCol_TableBorderStrong] = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);
	colors[ImGuiCol_TableBorderLight] = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
	colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(0.44f, 0.27f, 0.81f, 0.30f);
	colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
	colors[ImGuiCol_NavHighlight] = ImVec4(0.44f, 0.27f, 0.81f, 1.00f);
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
	colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
	colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
}

void Arg::Editor::GUI::EditorGUI::OnGUI(const EditorGUIContext& context)
{
	Editor* pEditor = context.pEditor;

	std::shared_ptr<Window>& pWindow = pEditor->GetWindow();
	const Vec2i viewportSize = Vec2i(pWindow->GetWidth(), pWindow->GetHeight());

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	ImGuizmo::BeginFrame();

	if (!pEditor->IsProjectOpened())
	{
		ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
		ImGui::SetNextWindowSize(ImVec2(
			static_cast<float>(viewportSize.x),
			static_cast<float>(viewportSize.y)
		));
		const bool isWelcomeOpen = ImGui::Begin(
			"Welcome",
			nullptr,
			ImGuiWindowFlags_NoResize
			| ImGuiWindowFlags_NoMove
			| ImGuiWindowFlags_NoCollapse
			| ImGuiWindowFlags_NoBringToFrontOnFocus
			| ImGuiWindowFlags_NoTitleBar
			| ImGuiWindowFlags_NoSavedSettings
		);

		if (isWelcomeOpen)
		{
			ImVec2 center = ImVec2(pWindow->GetWidth() / 2.0f, pWindow->GetHeight() / 2.0f);

			std::string text("Open or Create a Project to continue");
			ImVec2 textSize = ImGui::CalcTextSize(text.c_str());
			ImGui::SetCursorPos(ImVec2(center.x - (textSize.x * 0.5f), center.y - 150.0f));
			ImGui::Text(text.c_str());

			ImGui::SetCursorPos(ImVec2(center.x - 225.0f, center.y - 100.0f));
			if (ImGui::Button("Create Project", ImVec2(200.0f, 200.0f)))
			{
				std::filesystem::path path;
				const bool isSuccess = Dialog::FileOpenDialog::GetDirectory(path);
				if (isSuccess
					&& std::filesystem::is_directory(path)
					&& std::filesystem::is_empty(path))
				{
					pEditor->CreateProject(path);

					std::string directoryName = path.filename().string();
					std::filesystem::path settingsFile = path / directoryName;
					settingsFile.replace_extension(".aproj");
					pEditor->OpenProject(settingsFile);
				}
				else if (!std::filesystem::is_empty(path))
				{
					Dialog::MessageBoxDialog::ShowError(
						"Directory is not empty!",
						"Failed to create a project!"
					);
				}
				else
				{
					Dialog::MessageBoxDialog::ShowError(
						"Invalid project directory.",
						"Failed to create a project!"
					);
				}
			}

			ImGui::SetCursorPos(ImVec2(center.x + 25.0f, center.y - 100.0f));
			if (ImGui::Button("Open Project", ImVec2(200.0f, 200.0f)))
			{
				std::filesystem::path path;
				const bool isSuccess = Dialog::FileOpenDialog::GetFile(path);
				if (isSuccess
					&& path.has_extension()
					&& path.extension() == PROJECT_FILE_EXTENSION)
				{
					pEditor->DeselectActor();
					pEditor->OpenProject(path);
					m_ContentBrowser.Initialize(context);
				}
				else
				{
					Dialog::MessageBoxDialog::ShowError(
						"Invalid project file.",
						"Failed to open project!"
					);
				}
			}
		}

		ImGui::End();
	}
	else
	{
		auto& pProject = pEditor->GetProject();
		auto& pGameEngine = pEditor->GetGameEngine();

		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("Project"))
			{
				if (ImGui::MenuItem("Open"))
				{
					std::filesystem::path path;
					const bool isSuccess = Dialog::FileOpenDialog::GetFile(path);
					if (isSuccess
						&& path.has_extension()
						&& path.extension() == PROJECT_FILE_EXTENSION)
					{
						pEditor->DeselectActor();
						pEditor->OpenProject(path);
						m_ContentBrowser.Initialize(context);
					}
					else
					{
						Dialog::MessageBoxDialog::ShowError(
							"Invalid project file.",
							"Failed to open project!"
						);
					}
				}

				if (ImGui::MenuItem("Create"))
				{
					std::filesystem::path path;
					const bool isSuccess = Dialog::FileOpenDialog::GetDirectory(path);
					if (isSuccess
						&& std::filesystem::is_directory(path)
						&& std::filesystem::is_empty(path))
					{
						pEditor->CreateProject(path);

						std::string directoryName = path.filename().string();
						std::filesystem::path settingsFile = path / directoryName;
						settingsFile.replace_extension(".aproj");
						pEditor->OpenProject(settingsFile);
					}
					else if (!std::filesystem::is_empty(path))
					{
						Dialog::MessageBoxDialog::ShowError(
							"Directory is not empty!",
							"Failed to create a project!"
						);
					}
					else
					{
						Dialog::MessageBoxDialog::ShowError(
							"Invalid project directory.",
							"Failed to create a project!"
						);
					}
				}

				ImGui::Separator();

				if (ImGui::MenuItem("Save"))
				{
					if (!pGameEngine->IsPlaying())
					{
						pProject->Save();
					}
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Window"))
			{
				if (ImGui::MenuItem("Content Browser"))
				{
					m_ContentBrowser.Open();
				}

				if (ImGui::MenuItem("Project Settings"))
				{
					m_ProjectSettings.Open();
				}

				if (ImGui::MenuItem("World Outliner"))
				{
					m_WorldOutliner.Open();
				}

				if (ImGui::MenuItem("World Settings"))
				{
					m_WorldSettings.Open();
				}

				if (ImGui::MenuItem("Details"))
				{
					m_Details.Open();
				}

				if (ImGui::MenuItem("Resource"))
				{
					m_ResourceDetails.Open();
				}

				ImGui::EndMenu();
			}

			const float frameTime = pGameEngine->GetGameTime().GetAvgFrameTime();
			const float fps = 1.0f / frameTime;

			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 100.0f);
			const std::string fpsText = std::format("FPS: {:.3f}", fps);
			ImGui::Text(fpsText.c_str());

			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 250.0f);
			const std::string frameTimeText = std::format("Frame Time: {:.5f}", frameTime);
			ImGui::Text(frameTimeText.c_str());

			ImGui::EndMainMenuBar();
		}

		{
			ImGui::SetNextWindowPos(ImVec2(0.0f, ImGui::GetFrameHeight()));
			ImGui::SetNextWindowSize(ImVec2(
				static_cast<float>(viewportSize.x),
				50.0f
			));
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
			ImGui::Begin(
				"ToolBar",
				nullptr,
				ImGuiWindowFlags_NoResize
				| ImGuiWindowFlags_NoMove
				| ImGuiWindowFlags_NoCollapse
				| ImGuiWindowFlags_NoBringToFrontOnFocus
				| ImGuiWindowFlags_NoTitleBar
				| ImGuiWindowFlags_NoSavedSettings
			);
			ImGui::PopStyleVar();

			{
				ImGui::SetCursorPos(ImVec2(5.0f, 5.0f));
				if (ImGui::Button("Save", ImVec2(40.0f, 40.0f)))
				{
					if (!pGameEngine->IsPlaying())
					{
						pProject->Save();
					}
				}

				ImGui::SetCursorPos(ImVec2(50.0f, 5.0f));
				if (ImGui::Button("ReloadScripts", ImVec2(40.0f, 40.0f)))
				{
					if (!pGameEngine->IsPlaying())
					{
						pEditor->ReloadScripts();
					}
				}

				ImGui::SetCursorPos(ImVec2(170.0f, 5.0f));
				if (ImGui::Button("Move", ImVec2(40.0f, 40.0f)))
				{
					pEditor->SetGuizmoOperation(EditorGizmoOperation::Translate);
				}

				ImGui::SetCursorPos(ImVec2(215.0f, 5.0f));
				if (ImGui::Button("Rotate", ImVec2(40.0f, 40.0f)))
				{
					pEditor->SetGuizmoOperation(EditorGizmoOperation::Rotate);
				}

				ImGui::SetCursorPos(ImVec2(260.0f, 5.0f));
				if (ImGui::Button("Scale", ImVec2(40.0f, 40.0f)))
				{
					pEditor->SetGuizmoOperation(EditorGizmoOperation::Scale);
					pEditor->SetGuizmoMode(EditorGizmoMode::Local);
				}

				ImGui::SetCursorPos(ImVec2(305.0f, 5.0f));
				if (pEditor->GetGuizmoMode() == EditorGizmoMode::Global)
				{
					if (ImGui::Button("Local", ImVec2(40.0f, 40.0f)))
					{
						pEditor->SetGuizmoMode(EditorGizmoMode::Local);
					}
				}
				else
				{
					if (ImGui::Button("Global", ImVec2(40.0f, 40.0f)))
					{
						if (pEditor->GetGuizmoOperation() != EditorGizmoOperation::Scale)
						{
							pEditor->SetGuizmoMode(EditorGizmoMode::Global);
						}
					}
				}

				ImGui::SetCursorPos(ImVec2((viewportSize.x * 0.5f) - 20.0f, 5.0f));
				if (!pGameEngine->IsPlaying())
				{
					if (ImGui::Button("Play", ImVec2(40.0f, 40.0f)))
					{
						pEditor->PlayGame();
					}
				}
				else
				{
					if (ImGui::Button("Pause", ImVec2(40.0f, 40.0f)))
					{
						pEditor->StopGame();
					}
				}
			}

			ImGui::End();
		}

		ImGui::SetNextWindowPos(ImVec2(0.0f, ImGui::GetFrameHeight() + 50.0f));
		ImGui::SetNextWindowSize(ImVec2(
			static_cast<float>(viewportSize.x),
			static_cast<float>(viewportSize.y) - ImGui::GetFrameHeight() - 50.0f
		));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		const bool isViewportOpen = ImGui::Begin(
			"Viewport",
			nullptr,
			ImGuiWindowFlags_NoResize
			| ImGuiWindowFlags_NoMove
			| ImGuiWindowFlags_NoCollapse
			| ImGuiWindowFlags_NoBringToFrontOnFocus
			| ImGuiWindowFlags_NoTitleBar
			| ImGuiWindowFlags_NoSavedSettings
		);
		ImGui::PopStyleVar();
		if (isViewportOpen)
		{
			ImGuiID dockSpace = ImGui::GetID("EditorDockspace");
			ImGui::DockSpace(dockSpace, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);

			{
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
				const bool isGameViewOpen = ImGui::Begin(
					"Game View",
					nullptr,
					ImGuiWindowFlags_NoCollapse
				);
				ImGui::PopStyleVar();

				if (pGameEngine->IsPlaying()
					&& ImGui::IsWindowHovered()
					&& ImGui::IsMouseClicked(ImGuiMouseButton_Left)
					&& !pEditor->IsGameFocused())
				{
					pEditor->SetGameFocused(true);
				}

				if (pEditor->IsGameFocused())
				{
					ImGui::SetWindowFocus("Game View");
					ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse;
				}
				else
				{
					ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
				}

				if (isGameViewOpen)
				{
					if (!pGameEngine->IsPlaying())
					{
						if (ImGui::IsWindowHovered(ImGuiFocusedFlags_RootAndChildWindows))
						{
							if (!pEditor->IsCameraActive())
							{
								pEditor->SetCameraActive(true);
							}
						}
						else
						{
							if (ImGui::IsMouseReleased(ImGuiMouseButton_Right)
								&& pEditor->IsCameraActive())
							{
								pEditor->SetCameraActive(false);
								pEditor->GetCamera()->Cancel();
							}
						}
					}

					const Vec2i windowViewportSize = Vec2i(
						ImGui::GetWindowSize().x,
						ImGui::GetWindowSize().y - ImGui::GetFrameHeight()
					);

					if (pEditor->GetEditorViewSize() != windowViewportSize)
					{
						pEditor->SetEditorViewSize(windowViewportSize);
					}

					ImGui::Image(
						(void*)(intptr_t)pEditor->GetEditorViewRendererID(),
						ImVec2(
							static_cast<float>(windowViewportSize.x),
							static_cast<float>(windowViewportSize.y)
						),
						ImVec2(0.0f, 1.0f),
						ImVec2(1.0f, 0.0f)
					);

					if (!pGameEngine->IsPlaying())
					{
						const auto& pCamera = pEditor->GetCamera();

						if (pEditor->HasSelectedActor())
						{
							Gameplay::Actor* pActor;
							pEditor->GetSelectedActor(pActor);
							Mat4 actorTransform = pActor->GetTransform();
							EditTransform(
								pEditor->GetGuizmoOperation(),
								pEditor->GetGuizmoMode(),
								pCamera->GetCamera().get(),
								actorTransform
							);
							Vec3 position, rotation, scale;
							Math::Decompose(actorTransform, position, rotation, scale);
							pActor->SetPosition(position);
							pActor->SetRotation(Math::degrees(rotation));
							pActor->SetScale(scale);
						}

						const Mat4 view = pCamera->GetView();
						const Mat4 proj = pCamera->GetProjection(1.0f);

						const auto& camera = pCamera->GetCamera();

						const Vec3 zPoint = camera->GetPosition() + camera->GetForwardVector() * 5.0f + Vec3(
							0.0f, 0.0f, 1.0f);
						const Vec3 yPoint = camera->GetPosition() + camera->GetForwardVector() * 5.0f + Vec3(
							0.0f, 1.0f, 0.0f);
						const Vec3 xPoint = camera->GetPosition() + camera->GetForwardVector() * 5.0f + Vec3(
							1.0f, 0.0f, 0.0f);

						const Vec4 zPointT = proj * view * Vec4(zPoint, 1.0f);
						const Vec2 zPointClip = Vec2(zPointT.x / zPointT.w, zPointT.y / zPointT.w);

						const Vec4 yPointT = proj * view * Vec4(yPoint, 1.0f);
						const Vec2 yPointClip = Vec2(yPointT.x / yPointT.w, yPointT.y / yPointT.w);

						const Vec4 xPointT = proj * view * Vec4(xPoint, 1.0f);
						const Vec2 xPointClip = Vec2(xPointT.x / xPointT.w, xPointT.y / xPointT.w);

						ImVec2 orientationCenter = ImVec2(
							ImGui::GetWindowPos().x + 40.0f,
							ImGui::GetWindowPos().y + ImGui::GetWindowHeight() - 40.0f
						);

						const ImVec2 xTextSize = ImGui::CalcTextSize("X");
						ImGui::SetCursorScreenPos(ImVec2(
							orientationCenter.x + 60.0f * xPointClip.x - xTextSize.x * 0.5f,
							orientationCenter.y - 60.0f * xPointClip.y - xTextSize.y * 0.5f
						));
						ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "X");

						const ImVec2 yTextSize = ImGui::CalcTextSize("Y");
						ImGui::SetCursorScreenPos(ImVec2(
							orientationCenter.x + 60.0f * yPointClip.x - yTextSize.x * 0.5f,
							orientationCenter.y - 60.0f * yPointClip.y - yTextSize.y * 0.5f
						));
						ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Y");

						const ImVec2 zTextSize = ImGui::CalcTextSize("Z");
						ImGui::SetCursorScreenPos(ImVec2(
							orientationCenter.x + 60.0f * zPointClip.x - zTextSize.x * 0.5f,
							orientationCenter.y - 60.0f * zPointClip.y - zTextSize.y * 0.5f
						));
						ImGui::TextColored(ImVec4(0.0f, 0.0f, 1.0f, 1.0f), "Z");

						ImGui::GetForegroundDrawList()->AddLine(
							orientationCenter,
							ImVec2(
								orientationCenter.x + 45.0f * xPointClip.x,
								orientationCenter.y - 45.0f * xPointClip.y
							),
							0xFF0000FF,
							2.0f
						);


						ImGui::GetForegroundDrawList()->AddLine(
							orientationCenter,
							ImVec2(
								orientationCenter.x + 45.0f * yPointClip.x,
								orientationCenter.y - 45.0f * yPointClip.y
							),
							0xFF00FF00,
							2.0f
						);

						ImGui::GetForegroundDrawList()->AddLine(
							orientationCenter,
							ImVec2(
								orientationCenter.x + 45.0f * zPointClip.x,
								orientationCenter.y - 45.0f * zPointClip.y
							),
							0xFFFF0000,
							2.0f
						);
					}
				}
				ImGui::End();
			}

			m_ContentBrowser.Draw(context);
			m_ProjectSettings.Draw(context);
			m_WorldOutliner.Draw(context);
			m_Details.Draw(context);
			m_ResourceDetails.Draw(context);
			m_WorldSettings.Draw(context);
		}

		ImGui::End();
	}

	ImGui::Render();
}

void Arg::Editor::GUI::EditorGUI::RenderDrawData()
{
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Arg::Editor::GUI::EditorGUI::EditTransform(
	const EditorGizmoOperation& operation,
	const EditorGizmoMode& mode,
	const Renderer::Camera* pCamera,
	Mat4& outTransform
)
{
	ImGuizmo::OPERATION currentOperation = ImGuizmo::TRANSLATE;
	switch (operation)
	{
	case EditorGizmoOperation::Translate:
		currentOperation = ImGuizmo::TRANSLATE;
		break;
	case EditorGizmoOperation::Rotate:
		currentOperation = ImGuizmo::ROTATE;
		break;
	case EditorGizmoOperation::Scale:
		currentOperation = ImGuizmo::SCALE;
		break;
	}
	ImGuizmo::MODE currentMode = ImGuizmo::WORLD;
	switch (mode)
	{
	case EditorGizmoMode::Global:
		currentMode = ImGuizmo::WORLD;
		break;
	case EditorGizmoMode::Local:
		currentMode = ImGuizmo::LOCAL;
		break;
	}

	const ImVec2 windowPos = ImGui::GetWindowPos();
	const ImVec2 windowSize = ImGui::GetWindowSize();
	ImGuizmo::SetRect(windowPos.x, windowPos.y, windowSize.x, windowSize.y);
	const Mat4 view = pCamera->GetView();
	const Mat4 projection = pCamera->VGetProjection(windowSize.x / windowSize.y);
	ImGuizmo::SetDrawlist(ImGui::GetWindowDrawList());
	ImGuizmo::Manipulate(
		Math::ValuePtr(view),
		Math::ValuePtr(projection),
		currentOperation,
		currentMode,
		Math::ValuePtr(outTransform),
		nullptr,
		nullptr
	);
}
