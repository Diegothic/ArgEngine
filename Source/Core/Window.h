#pragma once

#include <string>

#include "Arg/Memory.h"
#include "Arg/Math.h"
#include "Arg/Input.h"

struct GLFWwindow;

namespace Arg
{
	struct WindowSpec
	{
		std::string Title;
		uint32_t Width;
		uint32_t Height;
	};

	class Window
	{
	public:
		Window(const WindowSpec& spec);
		virtual ~Window() = default;

		uint32_t GetWidth() const { return m_Size.x; }
		uint32_t GetHeight() const { return m_Size.y; }

		bool Create();
		void Update();
		void Destroy();

		bool ShouldClose() const;

	protected:
		virtual void VOnCreate() {}
		virtual void VOnUpdate() {}
		virtual void VOnDestroy() {}

		virtual void VOnResized() {}

	private:
		void OnResized(int newWidth, int newHeight);

	private:
		GLFWwindow* m_pWindowHandle;
		std::string m_Title;
		Vec2u m_Size;

		Box<WindowInput> m_pWindowInput;
	};
}
