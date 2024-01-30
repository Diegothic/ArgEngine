#pragma once

#include <arg_pch.hpp>

#include "GameEngine.hpp"
#include "Content/Serialization/YamlSerializable.hpp"
#include "Core/Window/Window.hpp"
#include "GUI/EditorGUI.hpp"
#include "Project/Project.hpp"
#include "Renderer/Renderer.hpp"
#include "Renderer/RenderTarget.hpp"
#include "EditorCamera.hpp"

namespace Arg
{
	namespace Editor
	{
		struct EditorSpec
		{
			std::filesystem::path ConfigPath;
			std::shared_ptr<Window> pWindow;
		};

		struct EditorConfig : public Content::YamlSerializable
		{
			std::string GuiFont = "OpenSans-SemiBold";
			float GuiFontSize = 18.0f;
			int32_t WindowWidth = 1920;
			int32_t WindowHeight = 1080;
			bool bIsWindowVSync = true;

		protected:
			auto VOnSerialize(YAML::Node& node) const -> bool override;
			auto VOnDeserialize(const YAML::Node& node) -> bool override;
		};

		class Editor
		{
		public:
			Editor(const EditorSpec& spec);

			void Initialize();
			void CleanUp();

			void PlayGame();
			void StopGame();
			void ReloadScripts();

			void Update(const float deltaTime);
			void Render();

			auto GetGameEngine() -> std::shared_ptr<GameEngine>& { return m_pGameEngine; }
			auto GetWindow() -> std::shared_ptr<Window>& { return m_pWindow; }
			auto GetResourceCache() -> std::shared_ptr<Content::ResourceCache>& { return m_pResourceCache; }
			auto GetContent() -> std::unique_ptr<Content::Content>& { return m_pContent; }
			auto IsProjectOpened() const -> bool { return m_pProject != nullptr; }
			auto GetProject() -> std::unique_ptr<Project>& { return m_pProject; }

			auto GetEditorViewRendererID() const -> uint32_t;
			auto GetEditorViewSize() const -> const Vec2i&;
			void SetEditorViewSize(const Vec2i& size) const;

			void OpenProject(const std::filesystem::path& projectFile);
			void CreateProject(const std::filesystem::path& destination);

			auto HasSelectedActor() const -> bool { return m_SelectedActorID != GUID::Empty; }
			auto GetSelectedActor(Arg::Gameplay::Actor*& pOutActor) const -> bool;
			void SelectActor(const GUID actorID);
			void DeselectActor();

			auto HasSelectedResource() const -> bool { return m_SelectedResourceID != GUID::Empty; }
			auto GetSelectedResource() const -> std::shared_ptr<Content::Resource>&;
			void SelectResource(const GUID resourceID);
			void DeselectResource();

			auto GetCamera() const -> const std::unique_ptr<EditorCamera>& { return m_pCamera; }
			auto IsCameraActive() const -> bool { return m_bIsCameraActive; }
			void SetCameraActive(const bool isActive) { m_bIsCameraActive = isActive; }

			auto IsGameFocused() const -> bool { return m_bIsGameFocused; }
			void SetGameFocused(const bool bFocused);

			auto GetGuizmoOperation() const -> const GUI::EditorGizmoOperation& { return m_GuizmoOperation; }
			void SetGuizmoOperation(const GUI::EditorGizmoOperation& operation) { m_GuizmoOperation = operation; }
			auto GetGuizmoMode() const -> const GUI::EditorGizmoMode& { return m_GuizmoMode; }
			void SetGuizmoMode(const GUI::EditorGizmoMode& mode) { m_GuizmoMode = mode; }

			auto GetDrawDebug() const -> bool { return m_bDrawDebug; }
			void SetDrawDebug(bool bDrawDebug) { m_bDrawDebug = bDrawDebug; }

		private:
			std::filesystem::path m_ConfigPath;
			EditorConfig m_Config;

			std::unique_ptr<EditorCamera> m_pCamera = nullptr;
			bool m_bIsCameraActive = false;
			std::shared_ptr<GameEngine> m_pGameEngine = nullptr;
			std::unique_ptr<YAML::Node> m_pWorldCacheNode = nullptr;
			bool m_bIsGameFocused = false;
			bool m_bDrawDebug = true;

			std::shared_ptr<Window> m_pWindow = nullptr;
			std::shared_ptr<Content::ResourceCache> m_pResourceCache = nullptr;
			std::unique_ptr<Content::Content> m_pContent = nullptr;
			std::unique_ptr<GUI::EditorGUI> m_pGUI = nullptr;
			std::unique_ptr<Project> m_pProject = nullptr;

			std::shared_ptr<Renderer::Renderer> m_pRenderer = nullptr;
			std::unique_ptr<Renderer::RenderTarget> m_pEditorViewRenderTarget = nullptr;
			std::unique_ptr<Renderer::RenderTarget> m_pGameViewRenderTarget = nullptr;

			GUI::EditorGizmoOperation m_GuizmoOperation = GUI::EditorGizmoOperation::Translate;
			GUI::EditorGizmoMode m_GuizmoMode = GUI::EditorGizmoMode::Global;

			GUID m_SelectedActorID = GUID::Empty;
			GUID m_SelectedResourceID = GUID::Empty;
		};
	}
}
