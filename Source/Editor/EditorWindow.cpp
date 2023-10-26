#include "EditorWindow.h"

#include <iostream>
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <glad/glad.h>

Arg::EditorWindow::EditorWindow(const WindowSpec& spec)
	: Arg::Window(spec)
{
}

void Arg::EditorWindow::VOnCreate()
{
	Window::VOnCreate();

	m_Scene = NewBox<Scene>();

	m_SceneHierarchyWidget = NewBox<SceneHierarchyWidget>(m_Scene.get());
}

void Arg::EditorWindow::VOnStart()
{
	Window::VOnStart();

	glGenFramebuffers(1, &m_FrameBufferID);
	glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBufferID);

	glGenTextures(1, &m_FrameBufferTextureID);
	glBindTexture(GL_TEXTURE_2D, m_FrameBufferTextureID);

	glTexImage2D(
		GL_TEXTURE_2D, 
		0, 
		GL_RGB, 
		1920, 
		1080, 
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
		1920,
		1080,
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
		.Size = Vec2i(1920, 1080),
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
		| ImGuiWindowFlags_NoTitleBar
	);

	if (ImGui::BeginTabBar("Tabs"))
	{
		if (ImGui::BeginTabItem("Scene Hierarchy"))
		{
			m_SceneHierarchyWidget->OnGUI();

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Create Object"))
		{
			if (ImGui::Button("Empty"))
			{
				m_Scene->CreateGameObject("New Object");
			}
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}


	ImGui::End();

	const ImVec2 inspectorPanelSize(
		windowWidth * 0.2f,
		workspaceHeight
	);
	ImGui::SetNextWindowPos(ImVec2(
		workspaceWidth - workspaceWidth * 0.2f,
		workspaceTop
	));
	ImGui::SetNextWindowSize(inspectorPanelSize);
	ImGui::Begin("Inspector",
		nullptr,
		ImGuiWindowFlags_NoMove
		| ImGuiWindowFlags_NoResize
		| ImGuiWindowFlags_NoCollapse
	);

	if (m_SceneHierarchyWidget->GetSelectedGameObjectID() != 0)
	{
		GameObject* selectedObject = m_Scene->FindGameObject(m_SceneHierarchyWidget->GetSelectedGameObjectID());
		if (selectedObject != nullptr)
		{
			ImGui::PushID(static_cast<int>(selectedObject->GetID()));
			char nameBuffer[512];
			strcpy_s(nameBuffer, selectedObject->GetName().c_str());
			ImGui::InputText("##Name",
				nameBuffer,
				512,
				ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_CallbackAlways,
				[](ImGuiInputTextCallbackData* data) -> int
				{
					if (data->BufTextLen < 1)
					{
						return 0;
					}

					GameObject* gameObject = (GameObject*)data->UserData;
					const std::string newName(data->Buf);
					gameObject->SetName(newName);

					return 1;
				},
				selectedObject
			);

			ImGui::SameLine();
			ImGui::SetCursorPosX(inspectorPanelSize.x - 50.0f);
			if (ImGui::Button("X"))
			{
				m_Scene->DestroyGameObject(selectedObject->GetID());
			}

			if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
			{
				const Transform& transform = selectedObject->GetTransform();
				const Vec3& position = transform.GetLocalPosition();
				const Vec3& rotation = transform.GetLocalRotationEuler();
				const Vec3& scale = transform.GetLocalScale();

				Vec3 newPosition = position;
				ImGui::Text("Position");
				ImGui::SameLine();
				ImGui::SetCursorPosX(inspectorPanelSize.x - (inspectorPanelSize.x * 0.36f) - 110.0f);
				ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);

				ImGui::SameLine();
				ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
				ImGui::Text("X");
				ImGui::PopStyleColor();
				ImGui::SameLine();
				ImGui::SetNextItemWidth(inspectorPanelSize.x * 0.12f);
				ImGui::DragFloat("##PosX", &newPosition[0], 0.1f);

				ImGui::SameLine();
				ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
				ImGui::Text("Y");
				ImGui::PopStyleColor();
				ImGui::SameLine();
				ImGui::SetNextItemWidth(inspectorPanelSize.x * 0.12f);
				ImGui::DragFloat("##PosY", &newPosition[1], 0.1f);

				ImGui::SameLine();
				ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 0, 255, 255));
				ImGui::Text("Z");
				ImGui::PopStyleColor();
				ImGui::SameLine();
				ImGui::SetNextItemWidth(inspectorPanelSize.x * 0.12f);
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

					selectedObject->SetLocalPosition(newPosition);
				}

				Vec3 newRotation = rotation;
				ImGui::Text("Rotation");
				ImGui::SameLine();
				ImGui::SetCursorPosX(inspectorPanelSize.x - (inspectorPanelSize.x * 0.36f) - 110.0f);
				ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);

				ImGui::SameLine();
				ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
				ImGui::Text("X");
				ImGui::PopStyleColor();
				ImGui::SameLine();
				ImGui::SetNextItemWidth(inspectorPanelSize.x * 0.12f);
				ImGui::DragFloat("##RotX", &newRotation[0]);

				ImGui::SameLine();
				ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
				ImGui::Text("Y");
				ImGui::PopStyleColor();
				ImGui::SameLine();
				ImGui::SetNextItemWidth(inspectorPanelSize.x * 0.12f);
				ImGui::DragFloat("##RotY", &newRotation[1]);

				ImGui::SameLine();
				ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 0, 255, 255));
				ImGui::Text("Z");
				ImGui::PopStyleColor();
				ImGui::SameLine();
				ImGui::SetNextItemWidth(inspectorPanelSize.x * 0.12f);
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

					selectedObject->SetLocalRotation(newRotation);
				}


				Vec3 newScale = scale;
				ImGui::Text("Scale");
				

				ImGui::SameLine();
				static bool scaleProportionally = false;
				ImGui::Checkbox("##Prop", &scaleProportionally);

				ImGui::SameLine();
				ImGui::SetCursorPosX(inspectorPanelSize.x - (inspectorPanelSize.x * 0.36f) - 110.0f);
				ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);

				ImGui::SameLine();
				ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
				ImGui::Text("X");
				ImGui::PopStyleColor();
				ImGui::SameLine();
				ImGui::SetNextItemWidth(inspectorPanelSize.x * 0.12f);
				ImGui::DragFloat("##ScaX", &newScale[0], 0.1f);

				ImGui::SameLine();
				ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
				ImGui::Text("Y");
				ImGui::PopStyleColor();
				ImGui::SameLine();
				ImGui::SetNextItemWidth(inspectorPanelSize.x * 0.12f);
				ImGui::DragFloat("##ScaY", &newScale[1], 0.1f);

				ImGui::SameLine();
				ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 0, 255, 255));
				ImGui::Text("Z");
				ImGui::PopStyleColor();
				ImGui::SameLine();
				ImGui::SetNextItemWidth(inspectorPanelSize.x * 0.12f);
				ImGui::DragFloat("##ScaZ", &newScale[2], 0.1f);

				ImGui::SameLine();
				if (ImGui::Button("R##Rot"))
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

					selectedObject->SetLocalScale(newScale);
				}

			}

			ImGui::PopID();
		}
	}

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
