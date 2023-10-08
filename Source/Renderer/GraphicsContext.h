#pragma once

struct GLFWwindow;

namespace Arg
{
	class GraphicsContext
	{
	public:
		GraphicsContext(GLFWwindow* windowHandle);
		~GraphicsContext() = default;

		bool Initialize() const;
		void SwapBuffers() const;

	private:
		GLFWwindow* m_pWindowHandle;
	};
}
