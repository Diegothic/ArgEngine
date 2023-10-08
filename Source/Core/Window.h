#pragma once

#include <string>

#include "Arg/Memory.h"
#include "Arg/Math.h"
#include "Arg/Input.h"
#include "Arg/Renderer.h"

struct GLFWwindow;

namespace Arg
{
	struct WindowSpec
	{
		std::string Title;
		uint32_t Width;
		uint32_t Height;
		bool VSync;
	};

	class Window
	{
	public:
		Window(const WindowSpec& spec);
		virtual ~Window() = default;

		uint32_t GetWidth() const { return m_Size.x; }
		uint32_t GetHeight() const { return m_Size.y; }

		bool Create();
		void Start();
		void Update();
		void Destroy();

		bool ShouldClose() const;
		bool IsVSync() const { return m_IsVSync; }
		void SetVSync(bool enabled);

	protected:
		virtual void VOnCreate() {}
		virtual void VOnStart() {}
		virtual void VOnUpdate(double deltaTime) {}
		virtual void VOnRender() {}
		virtual void VOnGUI() {}
		virtual void VOnDestroy() {}

		virtual void VOnResized() {}

	private:
		void OnResized(int newWidth, int newHeight);

	private:
		GLFWwindow* m_pWindowHandle;
		std::string m_Title;
		Vec2u m_Size;
		bool m_IsVSync;

		Box<Renderer> m_Renderer;
		Box<WindowInput> m_Input;

		double m_DeltaTime;
	};
}
