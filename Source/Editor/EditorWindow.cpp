#include "EditorWindow.h"

#include <iostream>

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "glad/glad.h"

#include "Arg/Debug.h"

Arg::EditorWindow::EditorWindow(const WindowSpec& spec)
	: Arg::Window(spec)
{
}

void Arg::EditorWindow::VOnCreate()
{
	Window::VOnCreate();
}

void Arg::EditorWindow::VOnStart()
{
	Window::VOnStart();

	glGenFramebuffers(1, &m_FrameBufferID);
	glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBufferID);

	glGenTextures(1, &m_FrameBufferTextureID);
	glBindTexture(GL_TEXTURE_2D, m_FrameBufferTextureID);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1920, 1080, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_FrameBufferTextureID, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Arg::EditorWindow::VOnUpdate(
	Box<WindowInput>& input,
	double deltaTime
)
{
	Window::VOnUpdate(input, deltaTime);
}

void Arg::EditorWindow::VOnRender(Box<Renderer>& renderer)
{
	Window::VOnRender(renderer);

	glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBufferID);

	const FrameParams frameParams{
		.Size = Vec2i(1920, 1080),
	};
	renderer->BeginFrame(frameParams);

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

		ImGui::SameLine(windowWidth - 270.0f);
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

	const float workspaceWidth = windowWidth;
	const float workspaceHeight = windowHeight - 3.0f * ImGui::GetFrameHeight();
	const float workspaceTop = 2.0f * ImGui::GetFrameHeight();

	ImGui::SetNextWindowPos(ImVec2(
		0.0f,
		workspaceTop
	));
	ImGui::SetNextWindowSize(ImVec2(
		workspaceWidth * 0.2f + 1.0f,
		workspaceHeight * 0.65f + 1.0f
	));
	ImGui::Begin("Hierarchy",
		nullptr,
		ImGuiWindowFlags_NoMove
		| ImGuiWindowFlags_NoResize
		| ImGuiWindowFlags_NoCollapse
	);



	ImGui::End();

	ImGui::SetNextWindowPos(ImVec2(
		workspaceWidth - workspaceWidth * 0.2f,
		workspaceTop
	));
	ImGui::SetNextWindowSize(ImVec2(
		windowWidth * 0.2f,
		workspaceHeight
	));
	ImGui::Begin("Inspector",
		nullptr,
		ImGuiWindowFlags_NoMove
		| ImGuiWindowFlags_NoResize
		| ImGuiWindowFlags_NoCollapse
	);



	ImGui::End();

	ImGui::SetNextWindowPos(ImVec2(
		0.0f,
		workspaceTop + workspaceHeight * 0.65f
	));
	ImGui::SetNextWindowSize(ImVec2(
		workspaceWidth - workspaceWidth * 0.2f + 1.0f,
		workspaceHeight - workspaceHeight * 0.65f + 1.0f
	));
	ImGui::Begin("Console",
		nullptr,
		ImGuiWindowFlags_NoMove
		| ImGuiWindowFlags_NoResize
		| ImGuiWindowFlags_NoCollapse
	);



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
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.05f, 0.05f, 0.05f, 1.0f));
	ImGui::Begin("Game",
		nullptr,
		ImGuiWindowFlags_NoMove
		| ImGuiWindowFlags_NoResize
		| ImGuiWindowFlags_NoCollapse
		| ImGuiWindowFlags_NoTitleBar
		| ImGuiWindowFlags_NoScrollbar
		| ImGuiWindowFlags_NoScrollWithMouse
	);


	const ImVec2 gameWindowWorkspace(
		gameWindowSize.x - 10.0f,
		gameWindowSize.y - 10.0f
	);

	ImVec2 textureSize(1920, 1080);
	if (gameWindowWorkspace.x < textureSize.x)
	{
		const float ratio = textureSize.x / gameWindowWorkspace.x;
		textureSize.x /= ratio;
		textureSize.y /= ratio;
	}

	if (gameWindowWorkspace.y < textureSize.y)
	{
		const float ratio = textureSize.y / gameWindowWorkspace.y;
		textureSize.x /= ratio;
		textureSize.y /= ratio;
	}

	const ImVec2 margins(
		gameWindowSize.x - textureSize.x,
		gameWindowSize.y - textureSize.y
	);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::SetCursorPos(ImVec2(margins.x * 0.5f, margins.y * 0.5f));
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
}
