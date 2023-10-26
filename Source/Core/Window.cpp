#include "Window.h"

#include <ranges>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>

#include "Debug/CoreLogger.h"

std::map<GLFWwindow*, Arg::Window*> Arg::Window::s_WindowRegistry;

Arg::Window::Window(const WindowSpec& spec)
	: m_pWindowHandle(nullptr),
	m_Input(nullptr),
	m_DeltaTime(0.0),
	m_DeltaTimeSampleCount(0),
	m_DeltaTimeSum(0.0),
	m_DeltaTimeAvg(0.0)
{
	m_Title = spec.Title;
	m_Size = glm::uvec2(spec.Width, spec.Height);
	m_IsVSync = spec.VSync;
}

bool Arg::Window::Create()
{
	AE_CORE_LOG_INFO("Creating a window");
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

	m_pWindowHandle = glfwCreateWindow(
		static_cast<int>(m_Size.x),
		static_cast<int>(m_Size.y),
		m_Title.c_str(),
		nullptr,
		nullptr
	);
	if (m_pWindowHandle == nullptr)
	{
		AE_CORE_LOG_ERR("Failed to create a window!");
		return false;
	}

	const RendererSpec rendererSpec{
		.WindowHandle = m_pWindowHandle,
	};
	m_Renderer = NewBox<Renderer>(rendererSpec);
	const bool rendererInitialized = m_Renderer->Initialize();
	if (!rendererInitialized)
	{
		AE_CORE_LOG_ERR("Failed to initialize the renderer!");
		return false;
	}

	m_Input = NewBox<WindowInput>();
	m_Input->Initialize(m_pWindowHandle);

	s_WindowRegistry[m_pWindowHandle] = this;

	SetVSync(m_IsVSync);

	glfwSetFramebufferSizeCallback(
		m_pWindowHandle,
		Window::WindowResizeCallback
	);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.IniFilename = "Temp/imgui.ini";

	ImGui::StyleColorsLight();
	io.Fonts->AddFontFromFileTTF("Content\\Engine\\Fonts\\Roboto-Regular.ttf", 18.0f);

	ImGui_ImplGlfw_InitForOpenGL(m_pWindowHandle, true);
	ImGui_ImplOpenGL3_Init("#version 150");

	VOnCreate();

	return true;
}

void Arg::Window::Start()
{
	m_DeltaTime = 1.0 / 30.0;

	VOnStart();
}

void Arg::Window::Update()
{
	m_DeltaTimeSum += m_DeltaTime;
	m_DeltaTimeSampleCount++;
	
	if (m_DeltaTimeSum >= 0.3)
	{
		m_DeltaTimeAvg = m_DeltaTimeSum / m_DeltaTimeSampleCount;
		m_DeltaTimeSum = 0.0;
		m_DeltaTimeSampleCount = 0;
	}

	const double updateBeginTime = glfwGetTime();

	m_Input->PrePullEvents();
	glfwPollEvents();
	m_Input->PostPullEvents();

	VOnUpdate(m_Input.get(), m_DeltaTime);

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	VOnGUI();

	ImGui::Render();

	const FrameParams frameParams{
		.Size = Vec2i(GetWidth(), GetHeight()),
	};
	m_Renderer->BeginFrame(frameParams);

	VOnRender(m_Renderer.get());

	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	m_Renderer->EndFrame();

	const double updateEndTime = glfwGetTime();
	m_DeltaTime = updateEndTime - updateBeginTime;
	if (m_DeltaTime > 1.0 / 30.0)
	{
		m_DeltaTime = 1.0 / 30.0;
	}
}

void Arg::Window::Destroy()
{
	AE_CORE_LOG_INFO("Destroying a window");

	VOnDestroy();

	ImGui_ImplGlfw_Shutdown();
	ImGui_ImplOpenGL3_Shutdown();
	ImGui::DestroyContext();

	s_WindowRegistry.erase(m_pWindowHandle);

	glfwDestroyWindow(m_pWindowHandle);
}

bool Arg::Window::ShouldClose() const
{
	return glfwWindowShouldClose(m_pWindowHandle);
}

void Arg::Window::SetVSync(bool enabled)
{
	m_IsVSync = enabled;
	glfwSwapInterval(m_IsVSync ? 1 : 0);
}

void Arg::Window::OnResized(int newWidth, int newHeight)
{
	m_Size.x = newWidth;
	m_Size.y = newHeight;

	VOnResized();
}

void Arg::Window::WindowResizeCallback(
	GLFWwindow* windowHandle,
	int newWidth,
	int newHeight
)
{
	if (!s_WindowRegistry.contains(windowHandle))
	{
		return;
	}

	Window* window = s_WindowRegistry[windowHandle];
	window->OnResized(newWidth, newHeight);
}
