#include <arg_pch.hpp>
#include "EditorGUI.hpp"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

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
	m_WorldOutliner.Initialize(context);
	m_Details.Initialize(context);
	m_ResourceDetails.Initialize(context);

	m_ContentBrowser.Open();
	m_WorldOutliner.Open();
	m_Details.Open();
	m_ResourceDetails.Open();
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
	auto& pProject = pEditor->GetProject();
	auto& pGameEngine = pEditor->GetGameEngine();
	std::shared_ptr<Window>& pWindow = pEditor->GetWindow();

	const Vec2i viewportSize = Vec2i(pWindow->GetWidth(), pWindow->GetHeight());

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

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

			if (ImGui::MenuItem("Save"))
			{
				pProject->Save();
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Window"))
		{
			if (ImGui::MenuItem("Editor View"))
			{

			}

			if (ImGui::MenuItem("Game View"))
			{

			}

			if (ImGui::MenuItem("Content Browser"))
			{
				m_ContentBrowser.Open();
			}

			if (ImGui::MenuItem("World Outliner"))
			{
				m_WorldOutliner.Open();
			}

			if (ImGui::MenuItem("Details"))
			{
				m_Details.Open();
			}

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	ImGui::SetNextWindowPos(ImVec2(0.0f, ImGui::GetFrameHeight()));
	ImGui::SetNextWindowSize(ImVec2(
		static_cast<float>(viewportSize.x),
		static_cast<float>(viewportSize.y) - ImGui::GetFrameHeight()
	));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	const bool isOpen = ImGui::Begin(
		"Viewport",
		nullptr,
		ImGuiWindowFlags_NoResize
		| ImGuiWindowFlags_NoMove
		| ImGuiWindowFlags_NoCollapse
		| ImGuiWindowFlags_NoBringToFrontOnFocus
		| ImGuiWindowFlags_NoTitleBar
	);
	ImGui::PopStyleVar();
	if (isOpen)
	{
		static const ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_None;
		ImGuiID dockSpace = ImGui::GetID("EditorDockspace");
		ImGui::DockSpace(dockSpace, ImVec2(0.0f, 0.0f), dockspaceFlags);

		{
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
			const bool isOpen = ImGui::Begin("Editor");
			ImGui::PopStyleVar();
			if (isOpen)
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

				const auto& pCamera = pEditor->GetCamera();

				const float aspectRatio = (float)windowViewportSize.x / windowViewportSize.y;
				const Mat4 view = pCamera->GetView();
				const Mat4 proj = pCamera->GetProjection(1.0f);

				const auto& camera = pCamera->GetCamera();

				const Vec3 zPoint = camera->GetPosition() + camera->GetForwardVector() * 5.0f + Vec3(0.0f, 0.0f, 1.0f);
				const Vec3 yPoint = camera->GetPosition() + camera->GetForwardVector() * 5.0f + Vec3(0.0f, 1.0f, 0.0f);
				const Vec3 xPoint = camera->GetPosition() + camera->GetForwardVector() * 5.0f + Vec3(1.0f, 0.0f, 0.0f);

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
			ImGui::End();
		}

		{
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
			const bool isOpen = ImGui::Begin("Game");
			ImGui::PopStyleVar();
			if (isOpen)
			{

			}
			ImGui::End();
		}

		m_ContentBrowser.Draw(context);
		m_WorldOutliner.Draw(context);
		m_Details.Draw(context);
		m_ResourceDetails.Draw(context);
	}

	ImGui::End();

	ImGui::Render();
}

void Arg::Editor::GUI::EditorGUI::RenderDrawData()
{
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
