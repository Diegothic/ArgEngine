#pragma once

#include <string>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

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

		bool Create();

		void StartUp();
		void Update() const;
		void ShutDown();

		bool ShouldClose() const;

	private:
		GLFWwindow* m_pWindowHandle;
		std::string m_Title;
		glm::uvec2 m_Size;
	};
}
