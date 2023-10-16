#pragma once

#include "Core/Window.h"

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
		void VOnUpdate(Box<WindowInput>& input, double deltaTime) override;
		void VOnRender(Box<Renderer>& renderer) override;
		void VOnGUI() override;
		void VOnDestroy() override;

		void VOnResized() override;

	private:
		uint32_t m_FrameBufferID;
		uint32_t m_FrameBufferTextureID;
	};
}
