#include <arg_pch.hpp>
#include "EditorApplication.hpp"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include "Debug/Log.hpp"
#include "Renderer/Renderer.hpp"
#include "Content/Content.hpp"
#include "GUI/Window/ContentBrowser.hpp"
#include "Content/Import/TextureImporter.hpp"
#include "Renderer/Camera/Camera.hpp"
#include "Renderer/Camera/PerspectiveCamera.hpp"
#include "Renderer/RenderTarget.hpp"

auto Arg::Client_CreateApplication() -> std::unique_ptr<Arg::Application>
{
	ARG_CONSOLE_LOG("Creating Editor");
	std::unique_ptr<Application> app(new EditorApplication());
	return app;
}

auto Arg::EditorApplication::VOnStartUp() -> void
{
	const Arg::WindowSpec windowSpec{
		.Title = "ArgEditor",
		.Width = 1920,
		.Height = 1080,
		.Mode = WindowMode::Windowed,
		.bIsVSync = true,
	};
	m_pWindow = std::make_unique<Window>(windowSpec);
	m_pWindow->Create();

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.IniFilename = "UserConfig\\imgui.ini";

	//ImGui::StyleColorsDark();
	{
		ImGuiStyle& style = ImGui::GetStyle();
		ImVec4* colors = style.Colors;

		style.WindowRounding = 0.0f;
		style.TabRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		//style.FrameBorderSize = 0.0f;
		//style.ChildBorderSize = 0.0f;
		style.TabBorderSize = 0.0f;
		//style.WindowPadding = ImVec2(0.0f, 0.0f);
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
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.44f, 0.27f, 0.81f, 1.00f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.34f, 0.17f, 0.71f, 1.00f);
		colors[ImGuiCol_Header] = ImVec4(0.08f, 0.08f, 0.08f, 1.00f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(0.44f, 0.27f, 0.81f, 1.00f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.34f, 0.17f, 0.71f, 1.00f);
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
		colors[ImGuiCol_TableBorderStrong] = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);   // Prefer using Alpha=1.0 here
		colors[ImGuiCol_TableBorderLight] = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);   // Prefer using Alpha=1.0 here
		colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
		colors[ImGuiCol_TextSelectedBg] = ImVec4(0.44f, 0.27f, 0.81f, 0.30f);
		colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
		colors[ImGuiCol_NavHighlight] = ImVec4(0.44f, 0.27f, 0.81f, 1.00f);
		colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
		colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
		colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
	}

	io.Fonts->AddFontFromFileTTF("Content\\_Engine\\Fonts\\OpenSans-SemiBold.ttf", 18.0f);

	ImGui_ImplGlfw_InitForOpenGL((GLFWwindow*)m_pWindow->GetHandle(), true);
	ImGui_ImplOpenGL3_Init("#version 150");
}

auto Arg::EditorApplication::VOnShutDown() -> void
{
	ImGui_ImplGlfw_Shutdown();
	ImGui_ImplOpenGL3_Shutdown();
	ImGui::DestroyContext();

	m_pWindow->Destroy();
}

auto Arg::EditorApplication::VOnRun() -> void
{
	std::shared_ptr<Renderer::Renderer> renderer(new Renderer::Renderer());

	auto resourceCache = std::make_shared<Content::ResourceCache>();

	const Content::ContentSpec contentSpec{
		.RootDirectory = "Content",
		.ResourceCache = resourceCache,
	};
	auto content = std::make_shared<Content::Content>(contentSpec);
	content->Initialize();

	Editor::GUI::ContentBrowser contentBrowser;
	contentBrowser.Initialize(content, resourceCache);

	auto containerTextureHandle = resourceCache->CreateHandle<Content::TextureResource>("Textures\\container");

	std::shared_ptr<Renderer::Camera> camera(new Renderer::PerspectiveCamera());
	camera->SetPosition(Vec3(-20.0f, 0.0f, 1.0f));

	Renderer::RenderTarget renderTarget;

	while (IsRunning() && !m_pWindow->ShouldClose())
	{
		m_pWindow->Update();
		const auto& keyboardState = m_pWindow->GetInput().GetKeyboardState();
		if (keyboardState.IsKeyPressed(Input::KeyCode::F4, Input::KeyMods::Alt))
		{
			Close();
		}

		if (keyboardState.IsKeyPressed(Input::KeyCode::F1))
		{
			m_pWindow->SetMode(WindowMode::Fullscreen);
		}

		if (keyboardState.IsKeyPressed(Input::KeyCode::F2))
		{
			m_pWindow->SetMode(WindowMode::Windowed);
		}

		const Vec2i viewportSize = Vec2i(m_pWindow->GetWidth(), m_pWindow->GetHeight());

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("Window"))
			{
				if (ImGui::MenuItem("Content Browser"))
				{
					contentBrowser.Open();
				}

				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}

		ImGui::SetNextWindowPos(ImVec2(0.0f, ImGui::GetFrameHeight()));
		ImGui::SetNextWindowSize(ImVec2(
			viewportSize.x,
			viewportSize.y - ImGui::GetFrameHeight()
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

			if (ImGui::Begin("Window"))
			{
				ImGui::SetCursorPos(ImVec2(15.0f, 35.0f));
				ImGui::Text("Hello, World!");
				const uint32_t imageID = containerTextureHandle.IsValid() ? containerTextureHandle.Get()->GetRendererID() : 0;
				ImGui::Image((void*)(intptr_t)imageID, ImVec2(100, 100), ImVec2(1.0, 1.0), ImVec2(0.0, 0.0), ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
				ImGui::GetForegroundDrawList()->AddLine(
					ImVec2(ImGui::GetWindowPos().x + 15.0f, ImGui::GetWindowPos().y + 15.0f),
					ImVec2(ImGui::GetWindowPos().x + 30.0f, ImGui::GetWindowPos().y + 50.0f),
					0xFF0000FF,
					5.0f
				);
			}
			ImGui::End();

			if (ImGui::Begin("Second Window"))
			{
				ImGui::Text("Hello, Window 2!");

				static char input[4096] = "/0";
				ImGui::InputTextMultiline("Text", input, 4096, ImVec2(400.0f, 100.0f));
				float x = 10.0f;
				ImGui::SliderFloat("Slider", &x, -10.0f, 10.0f);
				bool xB;
				ImGui::Checkbox("X", &xB);
				ImGui::Button("Click me!", ImVec2(50.0f, 25.0f));

			}
			ImGui::End();

			{
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
				const bool isOpen = ImGui::Begin("Editor");
				ImGui::PopStyleVar();
				if (isOpen)
				{
					const Vec2i windowViewportSize = Vec2i(
						ImGui::GetWindowSize().x,
						ImGui::GetWindowSize().y - ImGui::GetFrameHeight()
					);
					const float aspectRatio = (float)windowViewportSize.x / windowViewportSize.y;
					const Mat4 proj = camera->VGetProjection(aspectRatio);
					const Mat4 view = camera->GetView();

					if (renderTarget.GetSize() != windowViewportSize)
					{
						renderTarget.SetSize(windowViewportSize);
					}


					ImGui::Image(
						(void*)(intptr_t)renderTarget.GetRendererID(),
						ImVec2(windowViewportSize.x, windowViewportSize.y)
					);

					ImVec2 orientationCenter = ImVec2(
						ImGui::GetWindowPos().x + 25.0f,
						ImGui::GetWindowPos().y + ImGui::GetWindowHeight() - 25.0f
					);

					ImGui::GetForegroundDrawList()->AddLine(
						orientationCenter,
						ImVec2(orientationCenter.x - 15.0f, orientationCenter.y + 15.0f),
						0xFF0000FF,
						2.0f
					);

					ImGui::GetForegroundDrawList()->AddLine(
						orientationCenter,
						ImVec2(orientationCenter.x + 15.0f, orientationCenter.y + 15.0f),
						0xFF00FF00,
						2.0f
					);

					ImGui::GetForegroundDrawList()->AddLine(
						orientationCenter,
						ImVec2(orientationCenter.x, orientationCenter.y - 25.0f),
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

			contentBrowser.Draw(content, resourceCache);

		}
		ImGui::End();



		ImGui::Render();

		renderTarget.Begin();

		const Renderer::FrameParams frameParams{
				.ViewportSize = renderTarget.GetSize(),
				.ClearColor = Vec4(0.5f, 0.5f, 0.5f, 1.0f)
		};
		renderer->BeginFrame(frameParams);

		renderTarget.End();

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		m_pWindow->SwapBuffers();

	}
}
