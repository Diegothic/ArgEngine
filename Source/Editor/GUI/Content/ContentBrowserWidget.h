#pragma once

#include <filesystem>

#include "Editor/GUI/EditorWidget.h"

namespace Arg
{
	class Content;

	class ContentBrowserWidget : public EditorWidget
	{
	public:
		ContentBrowserWidget(Vec2 position, Vec2 size, Content* content);

	protected:
		void VOnDraw() override;
		void VOnResized(Vec2 newPosition, Vec2 newSize) override;

	private:
		void DrawDirectory(const std::filesystem::path& path);
		void DrawContents(const std::filesystem::path& path);

	private:
		Content* m_pContent = nullptr;

		bool m_HasSelectedDirectory = false;
		std::filesystem::path m_SelectedDirectory;
		bool m_HasSelectedFile = false;
		std::filesystem::path m_SelectedFile;
	};
}
