#pragma once

#include "Inspector.h"
#include "Core/Window.h"
#include "Gameplay/Scene/Scene.h"
#include "GUI/EditorWidget.h"

namespace Arg
{
	class EditorWindow : public Window
	{
	public:
		EditorWindow(const WindowSpec& spec);
		~EditorWindow() override = default;

	protected:
		void VOnCreate() override;
		void VOnStart() override;
		void VOnUpdate(WindowInput* input, double deltaTime) override;
		void VOnRender(Renderer* renderer) override;
		void VOnGUI() override;
		void VOnDestroy() override;

		void VOnResized() override;

	private:
		Box<Scene> m_Scene = nullptr;
		Box<Inspector> m_Inspector = nullptr;
		uint32_t m_FrameBufferID = 0;
		uint32_t m_FrameBufferTextureID = 0;
		uint32_t m_FrameBufferDepthTextureID = 0;

		Box<EditorWidget> m_LeftPanelWidget = nullptr;
		Box<EditorWidget> m_RightPanelWidget = nullptr;
		Box<EditorWidget> m_CenterPanelWidget = nullptr;
		Box<EditorWidget> m_ContentBrowserWidget = nullptr;
		Box<EditorWidget> m_ConsoleLogWidget = nullptr;
	};
}
