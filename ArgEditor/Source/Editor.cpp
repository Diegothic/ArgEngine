#include <arg_pch.hpp>
#include "Editor.hpp"

#include "Renderer/RenderContext.hpp"
#include "Content/Resource/GameResources/ShaderResource.hpp"

auto Arg::Editor::EditorConfig::VOnSerialize(YAML::Node& node) const -> bool
{
	auto header = node["ArgEditorConfig"];

	auto gui = header["GUI"];
	gui["Font"] = GuiFont;
	gui["FontSize"] = GuiFontSize;

	auto window = header["Window"];
	window["Width"] = WindowWidth;
	window["Height"] = WindowHeight;
	window["IsVSync"] = bIsWindwoVSync;

	return true;
}

auto Arg::Editor::EditorConfig::VOnDeserialize(const YAML::Node& node) -> bool
{
	const auto header = node["ArgEditorConfig"];
	if (!header)
	{
		return false;
	}

	auto gui = header["GUI"];
	if (gui)
	{
		GuiFont = ValueOr<std::string>(gui["Font"], "OpenSans-SemiBold");
		GuiFontSize = ValueOr<float>(gui["FontSize"], 18.0f);
	}

	auto window = header["Window"];
	if (window)
	{
		WindowWidth = ValueOr<int32_t>(window["Width"], 1920);
		WindowHeight = ValueOr<int32_t>(window["Height"], 1920);
		bIsWindwoVSync = ValueOr<bool>(window["IsVSync"], true);
	}

	return true;
}

Arg::Editor::Editor::Editor(const EditorSpec& spec)
	: m_ConfigPath(spec.ConfigPath),
	  m_pWindow(spec.pWindow)
{
}

void Arg::Editor::Editor::Initialize()
{
	// Read config
	{
		if (!std::filesystem::exists(m_ConfigPath))
		{
			m_Config.Serialize(m_ConfigPath);
		}

		m_Config.Deserialize(m_ConfigPath);
	}

	// Init window
	{
		m_pWindow->SetWidth(m_Config.WindowWidth);
		m_pWindow->SetHeight(m_Config.WindowHeight);
		m_pWindow->SetVSyncEnabled(m_Config.bIsWindwoVSync);
	}

	// Init Editor resource management
	{
		m_pResourceCache = std::make_shared<Content::ResourceCache>();
		const Content::ContentSpec contentSpec{
			.RootDirectory = "Content",
			.ResourceCache = m_pResourceCache,
		};
		m_pContent = std::make_unique<Content::Content>(contentSpec);
		m_pContent->Initialize();
	}

	// Init Game Engine
	{
		m_pGameEngine = std::make_shared<GameEngine>();
		m_pCamera = std::make_unique<EditorCamera>(m_pWindow.get());
		m_pRenderer = std::make_shared<Renderer::Renderer>();
		m_pRenderer->Initialize();
		m_pEditorViewRenderTarget = std::make_unique<Renderer::RenderTarget>();
	}

	// Init GUI
	{
		const GUI::EditorGUISpec guiSpec{
			.ConfigFile = "UserConfig\\GUI.ini"
		};
		m_pGUI = std::make_unique<GUI::EditorGUI>(guiSpec);
		const GUI::EditorGUIContext guiContext{
			.pEditor = this
		};
		m_pGUI->Initialize(guiContext, m_pWindow->GetHandle());
		const auto fontPath = (std::filesystem::path("Content\\Fonts") / m_Config.GuiFont)
			.replace_extension(".ttf");
		m_pGUI->SetFont(fontPath, m_Config.GuiFontSize);
	}

	// Load shaders
	{
		auto basicShaderHandle = m_pResourceCache
			->CreateHandle<Content::ShaderResource>("_Engine\\Shaders\\BasicStatic");
		basicShaderHandle.AddRef();
	}
}

void Arg::Editor::Editor::CleanUp()
{
	m_pGUI->CleanUp();
}

void Arg::Editor::Editor::Update(const float deltaTime)
{
	const auto& keyboardState = m_pWindow->GetInput().GetKeyboardState();
	const auto& mouseState = m_pWindow->GetInput().GetMouseState();

	// Handle full-screen switching
	{
		if (keyboardState.IsKeyPressed(Input::KeyCode::F1))
		{
			m_pWindow->SetMode(WindowMode::Fullscreen);
		}

		if (keyboardState.IsKeyPressed(Input::KeyCode::F2))
		{
			m_pWindow->SetMode(WindowMode::Windowed);
		}
	}

	// Update Editor-specific systems
	{
		if (m_bIsCameraActive)
		{
			m_pCamera->Update(deltaTime, m_pWindow->GetInput());
		}
	}

	// Update Game Engine systems
	{
		m_pGameEngine->Update(deltaTime);
	}

	// Clear frame garbage
	{
		m_pGameEngine->ClearGarbage();
	}
}

void Arg::Editor::Editor::Render()
{
	// Render GUI
	{
		const GUI::EditorGUIContext guiContext{
			.pEditor = this
		};
		m_pGUI->OnGUI(guiContext);
	}

	// Render Editor view
	{
		auto camera = GetCamera()->GetCamera();
		const auto& viewportSize = m_pEditorViewRenderTarget->GetSize();

		auto basicShaderHandle = m_pResourceCache
			->CreateHandle<Content::ShaderResource>("_Engine\\Shaders\\BasicStatic");
		auto basicShader = basicShaderHandle.IsValid()
			                   ? basicShaderHandle.Get()->GetShader()
			                   : nullptr;

		const Renderer::RenderContextSpec renderContextSpec{
			.pCamera = camera,
			.ViewportSize = viewportSize,
			.pBasicShader = basicShader
		};
		Renderer::RenderContext renderContext(renderContextSpec);
		m_pGameEngine->RenderEditor(renderContext);

		m_pEditorViewRenderTarget->Begin();

		renderContext.Render(*m_pRenderer);

		m_pEditorViewRenderTarget->End();
	}

	// Apply GUI
	m_pGUI->RenderDrawData();

	// TODO: Render game view
}

auto Arg::Editor::Editor::GetEditorViewRendererID() const -> uint32_t
{
	return m_pEditorViewRenderTarget->GetRendererID();
}

auto Arg::Editor::Editor::GetEditorViewSize() const -> const Vec2i&
{
	return m_pEditorViewRenderTarget->GetSize();
}

void Arg::Editor::Editor::SetEditorViewSize(const Vec2i& size) const
{
	m_pEditorViewRenderTarget->SetSize(size);
}

void Arg::Editor::Editor::OpenProject(const std::filesystem::path& projectFile)
{
	DeselectActor();
	DeselectResource();

	if (m_pProject != nullptr)
	{
		m_pProject->Close();
	}

	m_pProject = std::make_unique<Project>();
	m_pProject->Open(projectFile);

	m_pWindow->SetTitle(std::format("ArgEngine - {}", m_pProject->GetName()));

	m_pGameEngine->Initialize(m_pProject->GetResourceCache());

	const auto editorMapPath = m_pProject->GetEditorMap();
	if (!editorMapPath.empty())
	{
		m_pGameEngine->LoadWorld(m_pProject->GetEditorMap());
	}

	m_pProject->Save();
}

auto Arg::Editor::Editor::GetSelectedActor(Gameplay::Actor*& pOutActor) const -> bool
{
	if (m_pGameEngine->IsWorldLoaded())
	{
		const auto& pWorld = m_pGameEngine->GetLoadedWorld();
		pOutActor = &pWorld->GetActor(m_SelectedActorID);
		return true;
	}

	pOutActor = nullptr;
	return false;
}

void Arg::Editor::Editor::SelectActor(const GUID actorID)
{
	m_SelectedActorID = actorID;
}

void Arg::Editor::Editor::DeselectActor()
{
	m_SelectedActorID = GUID::Empty;
}

auto Arg::Editor::Editor::GetSelectedResource() const -> std::shared_ptr<Content::Resource>&
{
	ARG_ASSERT(m_SelectedResourceID != GUID::Empty, "No resource selected! Check first!");
	return IsProjectOpened()
		       ? m_pProject->GetResourceCache()->GetResource(m_SelectedResourceID)
		       : m_pResourceCache->GetResource(m_SelectedResourceID);
}

void Arg::Editor::Editor::SelectResource(const GUID resourceID)
{
	auto& resourceCache = IsProjectOpened()
		                      ? m_pProject->GetResourceCache()
		                      : m_pResourceCache;

	if (m_SelectedResourceID != GUID::Empty
		&& resourceCache->IsValid(m_SelectedResourceID))
	{
		resourceCache->GetGameResource(m_SelectedResourceID)->FreeRef();
	}

	m_SelectedResourceID = resourceID;

	if (m_SelectedResourceID != GUID::Empty
		&& resourceCache->IsValid(m_SelectedResourceID))
	{
		resourceCache->GetGameResource(m_SelectedResourceID)->AddRef();
	}
}

void Arg::Editor::Editor::DeselectResource()
{
	m_SelectedResourceID = GUID::Empty;
}
