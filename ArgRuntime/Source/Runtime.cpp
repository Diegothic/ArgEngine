#include <arg_pch.hpp>
#include "Runtime.hpp"

#include "Content/Resource/GameResources/ShaderResource.hpp"
#include "Content/Resource/GameResources/StaticModelResource.hpp"
#include "Renderer/Camera/PerspectiveCamera.hpp"

Arg::Runtime::Runtime::Runtime(const RuntimeSpec& spec)
	: m_ConfigPath(spec.ConfigPath),
	  m_pWindow(spec.pWindow)
{
}

void Arg::Runtime::Runtime::Initialize()
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

	// Init Resources
	{
		m_pResourceCache = std::make_shared<Content::ResourceCache>();
		const Content::ContentSpec contentSpec{
			.RootDirectory = "Data\\Content",
			.ResourceCache = m_pResourceCache,
		};
		m_pContent = std::make_unique<Content::Content>(contentSpec);
		m_pContent->Initialize();
	}

	// Init Game Engine
	{
		m_pGameEngine = std::make_shared<GameEngine>();
		m_pGameEngine->Initialize(
			"Data",
			m_pResourceCache
		);
		m_pGameEngine->LoadScripts();
		m_pRenderer = std::make_shared<Renderer::Renderer>();
		m_pRenderer->Initialize();

		const auto startingMap = m_Config.StartingMap;
		if (!startingMap.empty())
		{
			m_pGameEngine->LoadWorld(startingMap);
		}
	}

	// Load shaders
	{
		auto basicShaderHandle = m_pResourceCache
			->CreateHandle<Content::ShaderResource>("Shaders\\BasicStatic");
		basicShaderHandle.AddRef();

		auto shadowMapShaderHandle = m_pResourceCache
			->CreateHandle<Content::ShaderResource>("Shaders\\ShadowMap");
		shadowMapShaderHandle.AddRef();

		auto skyboxShaderHandle = m_pResourceCache
			->CreateHandle<Content::ShaderResource>("Shaders\\Skybox");
		skyboxShaderHandle.AddRef();
	}

	// Load engine content
	{
		auto skyboxMeshHandle = m_pResourceCache
			->CreateHandle<Content::StaticModelResource>("Meshes\\Skybox");
		skyboxMeshHandle.AddRef();
	}
}

void Arg::Runtime::Runtime::CleanUp()
{
}

void Arg::Runtime::Runtime::InitPlay()
{
	m_pGameEngine->Play();
}

void Arg::Runtime::Runtime::BeginPlay()
{
}

void Arg::Runtime::Runtime::Update(float deltaTime)
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

	// Update Game Engine systems
	{
		m_pGameEngine->Update(deltaTime);
	}

	// Clear frame garbage
	{
		m_pGameEngine->ClearGarbage();
	}
}

void Arg::Runtime::Runtime::Render()
{
	const auto& viewportSize = Vec2i(m_pWindow->GetWidth(), m_pWindow->GetHeight());

	const auto basicShaderHandle = m_pResourceCache
		->CreateHandle<Content::ShaderResource>("Shaders\\BasicStatic");
	const auto basicShader = basicShaderHandle.IsValid()
		                         ? basicShaderHandle.Get()->GetShader()
		                         : nullptr;

	const auto shadowMapShaderHandle = m_pResourceCache
		->CreateHandle<Content::ShaderResource>("Shaders\\ShadowMap");
	const auto shadowMapShader = shadowMapShaderHandle.IsValid()
		                             ? shadowMapShaderHandle.Get()->GetShader()
		                             : nullptr;

	const auto skyboxShaderHandle = m_pResourceCache
		->CreateHandle<Content::ShaderResource>("Shaders\\Skybox");
	const auto skyboxShader = skyboxShaderHandle.IsValid()
		                          ? skyboxShaderHandle.Get()->GetShader()
		                          : nullptr;

	const auto skyboxMeshHandle = m_pResourceCache
		->CreateHandle<Content::StaticModelResource>("Meshes\\Skybox");
	const auto skyboxMesh = skyboxMeshHandle.IsValid()
		                        ? skyboxMeshHandle.Get()->GetStaticModel()
		                        : nullptr;

	std::unique_ptr<Renderer::Camera> cam = std::make_unique<Renderer::PerspectiveCamera>();
	const Renderer::RenderContextSpec renderContextSpec{
		.pCamera = cam.get(),
		.ViewportSize = viewportSize,
		.pBasicShader = basicShader.get(),
		.pShadowMapShader = shadowMapShader.get(),
		.pSkyboxShader = skyboxShader.get(),
		.pSkyboxMesh = skyboxMesh.get()
	};
	Renderer::RenderContext renderContext(renderContextSpec);

	m_pGameEngine->RenderGame(renderContext);
	renderContext.Render(*m_pRenderer, nullptr);
}
