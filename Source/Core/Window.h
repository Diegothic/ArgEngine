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
		float GetFrameTime() const { return static_cast<float>(m_DeltaTime); }
		float GetFrameTimeAvg() const { return static_cast<float>(m_DeltaTimeAvg); }
		float GetFPS() const { return 1.0f / GetFrameTime(); }
		float GetFPSAvg() const { return 1.0f / GetFrameTimeAvg(); }

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
		virtual void VOnUpdate(Box<WindowInput>& input, double deltaTime) {}
		virtual void VOnRender(Box<Renderer>& renderer) {}
		virtual void VOnGUI() {}
		virtual void VOnDestroy() {}

		virtual void VOnResized() {}

	private:
		void OnResized(int newWidth, int newHeight);

	private:
		static std::map<GLFWwindow*, Window*> s_WindowRegistry;
		static void WindowResizeCallback(
			GLFWwindow* windowHandle,
			int newWidth,
			int newHeight
		);

	private:
		GLFWwindow* m_pWindowHandle;
		std::string m_Title;
		Vec2u m_Size;
		bool m_IsVSync;

		Box<Renderer> m_Renderer;
		Box<WindowInput> m_Input;

		double m_DeltaTime;
		int m_DeltaTimeSampleCount;
		double m_DeltaTimeSum;
		double m_DeltaTimeAvg;
	};
}
