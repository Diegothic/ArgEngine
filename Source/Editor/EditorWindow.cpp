#include "EditorWindow.h"

#include <iostream>
#include <glad/glad.h>

#include "Gameplay/Scene/Scene.h"
#include "GUI/Content/ContentBrowserWidget.h"
#include "GUI/Inspector/InspectorWidget.h"
#include "GUI/Scene/SceneHierarchyWidget.h"
#include "GUI/Scene/SceneManagementWidget.h"

Arg::EditorWindow::EditorWindow(const WindowSpec& spec)
	: Arg::Window(spec)
{
}

void Arg::EditorWindow::VOnCreate()
{
	Window::VOnCreate();

	m_Scene = NewBox<Scene>();
	m_Inspector = NewBox<Inspector>();

	m_LeftPanelWidget = NewBox<SceneManagementWidget>(Vec2(0.0f), Vec2(1.0f), m_Scene.get(), m_Inspector.get());
	m_RightPanelWidget = NewBox<InspectorWidget>(Vec2(0.0f), Vec2(1.0f), m_Inspector.get(), m_Scene.get());
	m_CenterPanelWidget = NewBox<SceneHierarchyWidget>(Vec2(0.0f), Vec2(1.0f), m_Scene.get(), m_Inspector.get());
	m_ContentBrowserWidget = NewBox<ContentBrowserWidget>(Vec2(0.0f), Vec2(1.0f), m_Scene.get()->GetContent());
	m_ConsoleLogWidget = NewBox<ContentBrowserWidget>(Vec2(0.0f), Vec2(1.0f), m_Scene.get()->GetContent());

	glGenFramebuffers(1, &m_FrameBufferID);
	glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBufferID);

	glGenTextures(1, &m_FrameBufferTextureID);
	glBindTexture(GL_TEXTURE_2D, m_FrameBufferTextureID);

	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_RGB,
		1,
		1,
		0,
		GL_RGB,
		GL_UNSIGNED_BYTE,
		nullptr
	);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_FrameBufferTextureID, 0);

	glGenTextures(1, &m_FrameBufferDepthTextureID);
	glBindTexture(GL_TEXTURE_2D, m_FrameBufferDepthTextureID);
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_DEPTH24_STENCIL8,
		1,
		1,
		0,
		GL_DEPTH_STENCIL,
		GL_UNSIGNED_INT_24_8,
		nullptr
	);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_FrameBufferDepthTextureID, 0);


	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Arg::EditorWindow::VOnStart()
{
	Window::VOnStart();

	m_Scene->Start();
}

void Arg::EditorWindow::VOnUpdate(
	WindowInput* input,
	double deltaTime
)
{
	Window::VOnUpdate(input, deltaTime);

	m_Scene->ClearGarbage();
}

void Arg::EditorWindow::VOnRender(Renderer* renderer)
{
	Window::VOnRender(renderer);

	glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBufferID);

	const FrameParams frameParams{
		.Size = m_CenterPanelWidget->GetSize(),
	};
	renderer->BeginFrame(frameParams);

	m_Scene->Render(renderer);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Arg::EditorWindow::VOnGUI()
{
	Window::VOnGUI();
	const float windowWidth = static_cast<float>(GetWidth());
	const float windowHeight = static_cast<float>(GetHeight());

	const ImGuiStyle& currentStyle = ImGui::GetStyle();
	ImGui::PushStyleColor(ImGuiCol_Border, currentStyle.Colors[ImGuiCol_MenuBarBg]);
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Open..."))
			{

			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Edit"))
		{
			if (ImGui::MenuItem("Settings"))
			{

			}

			ImGui::EndMenu();
		}

		ImGui::SameLine(windowWidth - 300.0f);
		ImGui::Separator();
		ImGui::SameLine();
		ImGui::Text("Editor");
		ImGui::SameLine();
		ImGui::Separator();
		ImGui::SameLine();
		ImGui::Text("Frame Time: %.3f", GetFrameTimeAvg());
		ImGui::SameLine();
		ImGui::Text("FPS: %.0f", GetFPSAvg());

		ImGui::EndMainMenuBar();
	}

	if (ImGui::BeginViewportSideBar("##SecondaryMenuBar",
		ImGui::GetMainViewport(),
		ImGuiDir_Up,
		ImGui::GetFrameHeight(),
		ImGuiWindowFlags_NoScrollbar
		| ImGuiWindowFlags_NoSavedSettings
		| ImGuiWindowFlags_MenuBar)
		) {
		if (ImGui::BeginMenuBar()) {
			ImGui::Text("Info");
			ImGui::EndMenuBar();
		}
		ImGui::End();
	}

	if (ImGui::BeginViewportSideBar("##MainStatusBar",
		ImGui::GetMainViewport(),
		ImGuiDir_Down,
		ImGui::GetFrameHeight(),
		ImGuiWindowFlags_NoScrollbar
		| ImGuiWindowFlags_NoSavedSettings
		| ImGuiWindowFlags_MenuBar)
		) {
		if (ImGui::BeginMenuBar()) {
			ImGui::Text("Status: ");
			ImGui::EndMenuBar();
		}
		ImGui::End();
	}

	ImGui::PopStyleColor();

	m_LeftPanelWidget->Draw();
	m_RightPanelWidget->Draw();

	const float workspaceWidth = windowWidth;
	const float workspaceHeight = windowHeight - 3.0f * ImGui::GetFrameHeight();
	const float workspaceTop = 2.0f * ImGui::GetFrameHeight();

	ImGui::SetNextWindowPos(ImVec2(
		0.0f,
		workspaceTop + workspaceHeight * 0.65f
	));
	ImGui::SetNextWindowSize(ImVec2(
		workspaceWidth - workspaceWidth * 0.2f + 1.0f,
		workspaceHeight - workspaceHeight * 0.65f + 1.0f
	));
	ImGui::Begin("##Bottom",
		nullptr,
		ImGuiWindowFlags_NoMove
		| ImGuiWindowFlags_NoResize
		| ImGuiWindowFlags_NoCollapse
		| ImGuiWindowFlags_NoTitleBar
	);

	if (ImGui::BeginTabBar("BottomTabs"))
	{
		if (ImGui::BeginTabItem("Content Browser"))
		{
			m_ContentBrowserWidget->Draw();
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Console log"))
		{
			m_ConsoleLogWidget->Draw();
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	ImGui::End();

	ImGui::SetNextWindowPos(ImVec2(
		workspaceWidth * 0.2f,
		workspaceTop
	));
	const ImVec2 gameWindowSize(
		workspaceWidth - 2.0f * workspaceWidth * 0.2f + 1.0f,
		workspaceHeight * 0.65f + 1.0f
	);
	ImGui::SetNextWindowSize(gameWindowSize);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.4f, 0.4f, 0.4f, 1.0f));
	ImGui::Begin("Game",
		nullptr,
		ImGuiWindowFlags_NoMove
		| ImGuiWindowFlags_NoResize
		| ImGuiWindowFlags_NoCollapse
		| ImGuiWindowFlags_NoTitleBar
		| ImGuiWindowFlags_NoScrollbar
		| ImGuiWindowFlags_NoScrollWithMouse
	);

	//const ImVec2 gameWindowWorkspace(
	//	gameWindowSize.x - 10.0f,
	//	gameWindowSize.y - 10.0f
	//);

	const ImVec2 textureSize(
		m_CenterPanelWidget->GetSize().x,
		m_CenterPanelWidget->GetSize().y
	);
	//if (gameWindowWorkspace.x < textureSize.x)
	//{
	//	const float ratio = textureSize.x / gameWindowWorkspace.x;
	//	textureSize.x /= ratio;
	//	textureSize.y /= ratio;
	//}

	//if (gameWindowWorkspace.y < textureSize.y)
	//{
	//	const float ratio = textureSize.y / gameWindowWorkspace.y;
	//	textureSize.x /= ratio;
	//	textureSize.y /= ratio;
	//}

	//const ImVec2 margins(
	//	gameWindowSize.x - textureSize.x,
	//	gameWindowSize.y - textureSize.y
	//);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	//ImGui::SetCursorPos(ImVec2(margins.x * 0.5f, margins.y * 0.5f));
	ImGui::SetCursorPos(ImVec2(0.0f, 0.0f));
	ImGui::BeginChild("GameView", textureSize, true);
	ImGui::Image((void*)(intptr_t)m_FrameBufferTextureID, textureSize);
	ImGui::EndChild();
	ImGui::PopStyleVar();

	ImGui::End();
	ImGui::PopStyleColor();
	ImGui::PopStyleVar();
}

void Arg::EditorWindow::VOnDestroy()
{
	Window::VOnDestroy();

	glDeleteFramebuffers(1, &m_FrameBufferID);
}

void Arg::EditorWindow::VOnResized()
{
	Window::VOnResized();

	const float windowWidth = static_cast<float>(GetWidth());
	const float windowHeight = static_cast<float>(GetHeight());
	const float menuBarHeight = 24.0f;

	const float workspaceWidth = windowWidth;
	const float workspaceHeight = windowHeight - 3.0f * menuBarHeight;
	const float workspaceTop = 2.0f * menuBarHeight;

	const Vec2 leftPanelPosition(
		0.0f,
		workspaceTop
	);
	const Vec2 leftPanelSize(
		workspaceWidth * 0.2f + 1.0f,
		workspaceHeight * 0.65f + 1.0f
	);
	const Vec2 rightPanelPosition(
		workspaceWidth * 0.8f,
		workspaceTop
	);
	const Vec2 rightPanelSize(
		windowWidth * 0.2f,
		workspaceHeight
	);
	const Vec2 centerPanelPosition(
		workspaceWidth * 0.2f,
		workspaceTop
	);
	const Vec2 centerPanelSize(
		workspaceWidth * 0.6f + 1.0f,
		workspaceHeight * 0.65f + 1.0f
	);
	const Vec2 bottomPanelPosition(
		0.0f,
		workspaceTop + workspaceHeight * 0.65f
	);
	const Vec2 bottomPanelSize(
		workspaceWidth * 0.8f + 1.0f,
		workspaceHeight * 0.35f + 1.0f
	);

	m_LeftPanelWidget->Resize(leftPanelPosition, leftPanelSize);
	m_RightPanelWidget->Resize(rightPanelPosition, rightPanelSize);
	m_CenterPanelWidget->Resize(centerPanelPosition, centerPanelSize);
	m_ContentBrowserWidget->Resize(bottomPanelPosition, bottomPanelSize);
	m_ConsoleLogWidget->Resize(bottomPanelPosition, bottomPanelSize);

	glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBufferID);
	glBindTexture(GL_TEXTURE_2D, m_FrameBufferTextureID);
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_RGB,
		static_cast<int>(centerPanelSize.x),
		static_cast<int>(centerPanelSize.y),
		0,
		GL_RGB,
		GL_UNSIGNED_BYTE,
		nullptr
	);

	glBindTexture(GL_TEXTURE_2D, m_FrameBufferDepthTextureID);
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_DEPTH24_STENCIL8,
		static_cast<int>(centerPanelSize.x),
		static_cast<int>(centerPanelSize.y),
		0,
		GL_DEPTH_STENCIL,
		GL_UNSIGNED_INT_24_8,
		nullptr
	);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
