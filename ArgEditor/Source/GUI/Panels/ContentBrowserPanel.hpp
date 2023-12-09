#pragma once

#include <arg_pch.hpp>

#include "GUI/EditorGUIPanel.hpp"
#include "Content/Content.hpp"
#include "Content/Resource/GameResources/TextureResource.hpp"

namespace Arg
{
	namespace Editor
	{
		namespace GUI
		{
			class ContentBrowserPanel : public EditorGUIPanel
			{
			public:
				void OnInitialize(const EditorGUIContext& context) override;
				void OnDraw(const EditorGUIContext& context) override;
				void DrawBrowser(
					const EditorGUIContext& context
				);
				void DrawFolderTree(
					const EditorGUIContext& context,
					const std::shared_ptr<Content::ResourceFolder>& folder,
					const int32_t treeLevel
				);

			private:
				std::shared_ptr<Content::ResourceFolder> m_pOpenedFolder;
				Content::ResourceHandle<Content::TextureResource> m_FolderOpenTexture;
				Content::ResourceHandle<Content::TextureResource> m_FolderClosedTexture;
				Content::ResourceHandle<Content::TextureResource> m_FileTexture;
			};
		}
	}
}
