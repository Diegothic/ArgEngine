#pragma once

#include <string>
#include <map>
#include <Arg/Math.h>
#include <Arg/Input.h>

struct GLFWwindow;

namespace Arg
{
	struct WindowSpec
	{
		std::string title;
		uint32_t width;
		uint32_t height;
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
		void OnKeyPressed(int key, int mods);
		void OnKeyReleased(int key);

	private:
		static std::map<GLFWwindow*, Window*> s_WindowRegistry;
		static void WindowResizeCallback(GLFWwindow* windowHandle, int newWidth, int newHeight);
		static void InputKeyCallback(GLFWwindow* windowHandle, int key, int scanCode, int action, int mods);

	private:
		GLFWwindow* m_pWindowHandle;
		std::string m_Title;
		Vec2u m_Size;

		KeyboardState m_KeyboardState;
	};
}
