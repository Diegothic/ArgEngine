#pragma once

#include <arg_pch.hpp>

#include "Panels/ContentBrowserPanel.hpp"
#include "Panels/WorldOutlinerPanel.hpp"
#include "Panels/DetailsPanel.hpp"
#include "Panels/ProjectSettingsPanel.hpp"
#include "Panels/ResourceDetailsPanel.hpp"
#include "Panels/WorldSettingsPanel.hpp"

namespace Arg
{
	namespace Editor
	{
		class Editor;

		namespace GUI
		{
			struct EditorGUIContext
			{
				Editor* pEditor = nullptr;
			};

			struct EditorGUISpec
			{
				std::filesystem::path ConfigFile;
			};

			enum EditorGizmoOperation
			{
				Translate,
				Rotate,
				Scale
			};

			enum EditorGizmoMode
			{
				Global,
				Local
			};

			class EditorGUI
			{
			public:
				EditorGUI(const EditorGUISpec& spec);

				void Initialize(
					const EditorGUIContext& context,
					const void* pWindowHandle
				);
				void CleanUp();

				void SetFont(
					const std::filesystem::path& fontFile,
					const float fontSize
				);

				void OnGUI(const EditorGUIContext& context);
				void RenderDrawData();

				void EditTransform(
					const EditorGizmoOperation& operation,
					const EditorGizmoMode& mode,
					const Renderer::Camera* pCamera,
					Mat4& outTransform
				);

			protected:
				void StyleColors();

			private:
				std::string m_ConfigFile;

				ContentBrowserPanel m_ContentBrowser;
				WorldOutlinerPanel m_WorldOutliner;
				DetailsPanel m_Details;
				ResourceDetailsPanel m_ResourceDetails;
				WorldSettingsPanel m_WorldSettings;
				ProjectSettingsPanel m_ProjectSettings;

				Content::ResourceHandle<Content::TextureResource> m_SaveIcon;
				Content::ResourceHandle<Content::TextureResource> m_ReloadScriptsIcon;
				Content::ResourceHandle<Content::TextureResource> m_PlayIcon;
				Content::ResourceHandle<Content::TextureResource> m_StopIcon;
				Content::ResourceHandle<Content::TextureResource> m_DebugShapesIcon;
				Content::ResourceHandle<Content::TextureResource> m_LocalIcon;
				Content::ResourceHandle<Content::TextureResource> m_GlobalIcon;
				Content::ResourceHandle<Content::TextureResource> m_MoveIcon;
				Content::ResourceHandle<Content::TextureResource> m_RotateIcon;
				Content::ResourceHandle<Content::TextureResource> m_ScaleIcon;
			};
		}
	}
}
