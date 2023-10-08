#include "GraphicsContext.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Arg/Debug.h"

Arg::GraphicsContext::GraphicsContext(GLFWwindow* windowHandle)
	: m_pWindowHandle(windowHandle)
{
}

bool Arg::GraphicsContext::Initialize() const
{
	glfwMakeContextCurrent(m_pWindowHandle);

	AE_CORE_LOG_INFO("Initializing OpenGL");
	const int gladLoaderState = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	if (!gladLoaderState)
	{
		AE_CORE_LOG_ERR("Failed to initialize GLAD!");
		return false;
	}

	const char* graphicsVersion = (const char*)glGetString(GL_VERSION);
	const char* graphicsSLVersion = (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION);
	AE_CORE_LOG_INFO("Using OpenGL version:\n\t%s", graphicsVersion);
	AE_CORE_LOG_INFO("Using GLSL version:\n\t%s", graphicsSLVersion);

	const char* graphicsVendorName = (const char*)glGetString(GL_VENDOR);
	const char* graphicsRendererName = (const char*)glGetString(GL_RENDERER);
	AE_CORE_LOG_INFO("Detected graphics device:\n\t%s: %s",
		graphicsVendorName,
		graphicsRendererName
	);

	return true;
}

void Arg::GraphicsContext::SwapBuffers() const
{
	glfwSwapBuffers(m_pWindowHandle);
}
