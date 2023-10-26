#pragma once

#include "Core/Window.h"
#include "EditorWidget.h"
#include "Gameplay/Scene/Scene.h"
#include "Inspector/Inpector.h"

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
		uint32_t m_FrameBufferID;
		uint32_t m_FrameBufferTextureID;
		uint32_t m_FrameBufferDepthTextureID;

		Box<EditorWidget> m_LeftPanelWidget = nullptr;
		Box<EditorWidget> m_RightPanelWidget = nullptr;
		Box<EditorWidget> m_CenterPanelWidget = nullptr;
		Box<EditorWidget> m_BottomPanelWidget = nullptr;
	};
}
