#include <arg_pch.hpp>
#include "Editor.hpp"

#include "Renderer/RenderContext.hpp"
#include "Content/Resource/GameResources/ShaderResource.hpp"
#include "Content/Resource/GameResources/SoundResource.hpp"
#include "Sound/SoundEngine.hpp"

auto Arg::Editor::EditorConfig::VOnSerialize(YAML::Node& node) const -> bool
{
	auto header = node["ArgEditorConfig"];

	auto gui = header["GUI"];
	gui["Font"] = GuiFont;
	gui["FontSize"] = GuiFontSize;

	auto window = header["Window"];
	window["Width"] = WindowWidth;
	window["Height"] = WindowHeight;
	window["IsVSync"] = bIsWindowVSync;

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
		WindowHeight = ValueOr<int32_t>(window["Height"], 1080);
		bIsWindowVSync = ValueOr<bool>(window["IsVSync"], true);
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
		m_pWindow->SetVSyncEnabled(m_Config.bIsWindowVSync);
	}

	// Init Editor resource management
	{
		m_pResourceCache = std::make_shared<Content::ResourceCache>();
		const Content::ContentSpec contentSpec{
			.RootDirectory = "Content",
			.ResourceCache = m_pResourceCache,
			.bAutoSaveConfig = false
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

		auto shadowMapShaderHandle = m_pResourceCache
			->CreateHandle<Content::ShaderResource>("_Engine\\Shaders\\ShadowMap");
		shadowMapShaderHandle.AddRef();

		auto skyboxShaderHandle = m_pResourceCache
			->CreateHandle<Content::ShaderResource>("_Engine\\Shaders\\Skybox");
		skyboxShaderHandle.AddRef();

		auto debugShaderHandle = m_pResourceCache
			->CreateHandle<Content::ShaderResource>("_Engine\\Shaders\\Debug");
		debugShaderHandle.AddRef();
	}

	// Load engine content
	{
		auto skyboxMeshHandle = m_pResourceCache
			->CreateHandle<Content::StaticModelResource>("_Engine\\Meshes\\Skybox");
		skyboxMeshHandle.AddRef();

		auto lineMeshHandle = m_pResourceCache
			->CreateHandle<Content::StaticModelResource>("_Engine\\Meshes\\Line");
		lineMeshHandle.AddRef();

		auto boxMeshHandle = m_pResourceCache
			->CreateHandle<Content::StaticModelResource>("_Engine\\Meshes\\Box");
		boxMeshHandle.AddRef();

		auto sphereMeshHandle = m_pResourceCache
			->CreateHandle<Content::StaticModelResource>("_Engine\\Meshes\\Sphere");
		sphereMeshHandle.AddRef();

		auto cylinderMeshHandle = m_pResourceCache
			->CreateHandle<Content::StaticModelResource>("_Engine\\Meshes\\Cylinder");
		cylinderMeshHandle.AddRef();

		auto cameraMeshHandle = m_pResourceCache
			->CreateHandle<Content::StaticModelResource>("_Engine\\Meshes\\Camera");
		cameraMeshHandle.AddRef();
	}
}

void Arg::Editor::Editor::CleanUp()
{
	m_pGUI->CleanUp();
}

void Arg::Editor::Editor::PlayGame()
{
	ARG_ASSERT(m_pGameEngine->IsWorldLoaded(), "No world loaded!");
	if (m_pGameEngine->IsPlaying())
	{
		return;
	}

	SetCameraActive(false);
	DeselectActor();
	DeselectResource();

	m_pWorldCacheNode = std::make_unique<YAML::Node>();
	m_pGameEngine->GetLoadedWorld()->Serialize(*m_pWorldCacheNode);
	m_pGameEngine->Play();

	SetGameFocused(true);
}

void Arg::Editor::Editor::StopGame()
{
	ARG_ASSERT(m_pGameEngine->IsWorldLoaded(), "No world loaded!");
	if (!m_pGameEngine->IsPlaying())
	{
		return;
	}

	DeselectActor();
	DeselectResource();

	m_pGameEngine->Stop();
	m_pGameEngine->GetLoadedWorld()->Deserialize(*m_pWorldCacheNode);
	m_pWorldCacheNode = nullptr;
}

void Arg::Editor::Editor::ReloadScripts()
{
	DeselectActor();
	DeselectResource();

	bool bReloadWorld = false;
	if (m_pGameEngine->IsWorldLoaded())
	{
		bReloadWorld = true;
		m_pWorldCacheNode = std::make_unique<YAML::Node>();
		m_pGameEngine->GetLoadedWorld()->Serialize(*m_pWorldCacheNode);
	}

	m_pGameEngine->RebuildScripts();

	if (bReloadWorld)
	{
		m_pGameEngine->GetLoadedWorld()->Deserialize(*m_pWorldCacheNode);
		m_pWorldCacheNode = nullptr;
	}
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

	// Handle Editor shortcuts
	{
		if (keyboardState.IsKeyPressed(Input::KeyCode::F1, Input::KeyMods::Shift))
		{
			SetGameFocused(false);
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
		const auto camera = GetCamera()->GetCamera();
		const auto& viewportSize = m_pEditorViewRenderTarget->GetSize();

		const auto basicShaderHandle = m_pResourceCache
			->CreateHandle<Content::ShaderResource>("_Engine\\Shaders\\BasicStatic");
		const auto basicShader = basicShaderHandle.IsValid()
			                         ? basicShaderHandle.Get()->GetShader()
			                         : nullptr;

		const auto shadowMapShaderHandle = m_pResourceCache
			->CreateHandle<Content::ShaderResource>("_Engine\\Shaders\\ShadowMap");
		const auto shadowMapShader = shadowMapShaderHandle.IsValid()
			                             ? shadowMapShaderHandle.Get()->GetShader()
			                             : nullptr;

		const auto skyboxShaderHandle = m_pResourceCache
			->CreateHandle<Content::ShaderResource>("_Engine\\Shaders\\Skybox");
		const auto skyboxShader = skyboxShaderHandle.IsValid()
			                          ? skyboxShaderHandle.Get()->GetShader()
			                          : nullptr;

		const auto debugShaderHandle = m_pResourceCache
			->CreateHandle<Content::ShaderResource>("_Engine\\Shaders\\Debug");
		const auto debugShader = debugShaderHandle.IsValid()
			                         ? debugShaderHandle.Get()->GetShader()
			                         : nullptr;

		const auto skyboxMeshHandle = m_pResourceCache
			->CreateHandle<Content::StaticModelResource>("_Engine\\Meshes\\Skybox");
		const auto skyboxMesh = skyboxMeshHandle.IsValid()
			                        ? skyboxMeshHandle.Get()->GetStaticModel()
			                        : nullptr;

		const auto lineMeshHandle = m_pResourceCache
			->CreateHandle<Content::StaticModelResource>("_Engine\\Meshes\\Line");
		const auto lineMesh = lineMeshHandle.IsValid()
			                      ? lineMeshHandle.Get()->GetStaticModel()
			                      : nullptr;

		const auto boxMeshHandle = m_pResourceCache
			->CreateHandle<Content::StaticModelResource>("_Engine\\Meshes\\Box");
		const auto boxMesh = boxMeshHandle.IsValid()
			                     ? boxMeshHandle.Get()->GetStaticModel()
			                     : nullptr;

		const auto sphereMeshHandle = m_pResourceCache
			->CreateHandle<Content::StaticModelResource>("_Engine\\Meshes\\Sphere");
		const auto sphereMesh = sphereMeshHandle.IsValid()
			                        ? sphereMeshHandle.Get()->GetStaticModel()
			                        : nullptr;

		const auto cylinderMeshHandle = m_pResourceCache
			->CreateHandle<Content::StaticModelResource>("_Engine\\Meshes\\Cylinder");
		const auto cylinderMesh = cylinderMeshHandle.IsValid()
			                          ? cylinderMeshHandle.Get()->GetStaticModel()
			                          : nullptr;

		const auto cameraMeshHandle = m_pResourceCache
			->CreateHandle<Content::StaticModelResource>("_Engine\\Meshes\\Camera");
		const auto cameraMesh = cameraMeshHandle.IsValid()
			                        ? cameraMeshHandle.Get()->GetStaticModel()
			                        : nullptr;

		const Renderer::RenderContextSpec renderContextSpec{
			.pCamera = camera.get(),
			.ViewportSize = viewportSize,
			.bDebugEnabled = true,

			.pBasicShader = basicShader.get(),
			.pShadowMapShader = shadowMapShader.get(),
			.pSkyboxShader = skyboxShader.get(),
			.pDebugShader = debugShader.get(),

			.pSkyboxMesh = skyboxMesh.get(),
			.pLineMesh = lineMesh.get(),
			.pBoxMesh = boxMesh.get(),
			.pSphereMesh = sphereMesh.get(),
			.pCylinderMesh = cylinderMesh.get(),
			.pCameraMesh = cameraMesh.get()
		};
		Renderer::RenderContext renderContext(renderContextSpec);

		if (m_pGameEngine->IsPlaying())
		{
			m_pGameEngine->RenderGame(renderContext);
		}
		else
		{
			m_pGameEngine->RenderEditor(renderContext);
		}

		if (m_bDrawDebug)
		{
			m_pGameEngine->RenderDebug(renderContext);
		}

		m_pEditorViewRenderTarget->Begin();

		renderContext.Render(*m_pRenderer, m_pEditorViewRenderTarget.get());

		m_pEditorViewRenderTarget->End();
	}

	// Apply GUI
	m_pGUI->RenderDrawData();
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

	m_pCamera->Reset();

	if (m_pProject != nullptr)
	{
		m_pGameEngine->Deinitialize();
		m_pProject->Close();
	}

	m_pProject = std::make_unique<Project>();
	m_pProject->Open(projectFile);

	m_pWindow->SetTitle(std::format("ArgEngine - {}", m_pProject->GetName()));

	m_pGameEngine->Initialize(
		projectFile.parent_path(),
		m_pProject->GetResourceCache()
	);

	m_pGameEngine->RebuildScripts();

	const auto editorMapPath = m_pProject->GetEditorMap();
	if (!editorMapPath.empty())
	{
		m_pGameEngine->LoadWorld(editorMapPath);
	}

	m_pProject->Save();
}

void Arg::Editor::Editor::CreateProject(const std::filesystem::path& destination)
{
	const std::string directoryName = destination.filename().string();
	ProjectSettings projectSettings;
	projectSettings.Name = directoryName;

	std::filesystem::path settingsFile = destination / directoryName;
	settingsFile.replace_extension(".aproj");
	projectSettings.Serialize(settingsFile);
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

void Arg::Editor::Editor::SetGameFocused(const bool bFocused)
{
	m_bIsGameFocused = bFocused;
	m_pWindow->SetCursorMode(bFocused ? Input::CursorMode::Locked : Input::CursorMode::Normal);
}
