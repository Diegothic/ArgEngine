#include "Window.h"

#include <iostream>
#include <ranges>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>

#include "Arg/Debug.h"

Arg::Window::Window(const WindowSpec& spec)
	: m_pWindowHandle(nullptr),
	m_Input(nullptr),
	m_DeltaTime(0.0)
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
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

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

	SetVSync(m_IsVSync);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.IniFilename = "Temp/imgui.ini";

	ImGui::StyleColorsLight();

	ImGui_ImplGlfw_InitForOpenGL(m_pWindowHandle, true);
	ImGui_ImplOpenGL3_Init("#version 150");

	VOnCreate();

	return true;
}

void Arg::Window::Start()
{
	m_DeltaTime = 1.0f / 30.0f;

	VOnStart();
}

void Arg::Window::Update()
{
	const double updateBeginTime = glfwGetTime();

	m_Input->PrePullEvents();
	glfwPollEvents();
	m_Input->PostPullEvents();

	// TODO: Remove input test
	const Rc<KeyboardState>& keyboardState = m_Input->GetKeyboardState();
	const Rc<MouseState>& mouseState = m_Input->GetMouseState();
	const Rc<GamepadState>& gamepadState = m_Input->GetGamepadState(0);

	if (keyboardState->IsKeyPressed(KeyCode::A))
	{
		std::cout << "A Pressed" << std::endl;
	}

	if (keyboardState->IsKeyPressed(KeyCode::A, KeyMods::Ctrl))
	{
		std::cout << "A Pressed with Ctrl" << std::endl;
	}

	if (keyboardState->IsKeyPressed(KeyCode::A, KeyMods::Ctrl | KeyMods::Shift))
	{
		std::cout << "A Pressed with Ctrl and Shift" << std::endl;
	}

	if (keyboardState->IsKeyDown(KeyCode::A))
	{
		std::cout << "A Down" << std::endl;
	}

	if (keyboardState->IsKeyReleased(KeyCode::A))
	{
		std::cout << "A Released" << std::endl;
	}

	if (keyboardState->IsKeyPressed(KeyCode::S, KeyMods::Ctrl))
	{
		std::cout << "S Pressed with Ctrl" << std::endl;
	}

	if (mouseState->IsButtonPressed(MouseButton::Right))
	{
		std::cout << "Right button Pressed" << std::endl;
	}

	if (mouseState->IsButtonDown(MouseButton::Right))
	{
		std::cout << "Right button Down" << std::endl;
	}

	if (mouseState->IsButtonReleased(MouseButton::Right))
	{
		std::cout << "Right button Released" << std::endl;
	}

	if (gamepadState != nullptr)
	{
		if (gamepadState->GetAxis2D(GamepadAxis2D::Left) != Vec2(0.0))
		{
			const Vec2 gamepadAxisLeft = gamepadState->GetAxis2DDelta(GamepadAxis2D::Left);
			std::cout << "Gamepad axis left: "
				<< gamepadAxisLeft.x
				<< " "
				<< gamepadAxisLeft.y
				<< std::endl;
		}

		if (gamepadState->IsButtonPressed(GamepadButton::South))
		{
			std::cout << "Gamepad A button Pressed" << std::endl;
		}

		if (gamepadState->IsButtonDown(GamepadButton::South))
		{
			std::cout << "Gamepad A button Down" << std::endl;
		}

		if (gamepadState->IsButtonReleased(GamepadButton::South))
		{
			std::cout << "Gamepad A button Released" << std::endl;
		}
	}

	VOnUpdate(m_DeltaTime);

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Stats");
	ImGui::Text("FrameTime: %f", m_DeltaTime);
	ImGui::Text("FPS: %f", 1.0 / m_DeltaTime);
	ImGui::End();

	VOnGUI();

	ImGui::Render();

	const FrameParams frameParams{
		.Size = Vec2i(GetWidth(), GetHeight()),
	};
	m_Renderer->BeginFrame(frameParams);

	VOnRender();

	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	m_Renderer->EndFrame();

	const double updateEndTime = glfwGetTime();
	m_DeltaTime = updateEndTime - updateBeginTime;
	if (m_DeltaTime > 1.0f / 30.0f)
	{
		m_DeltaTime = 1.0f / 30.0f;
	}
}

void Arg::Window::Destroy()
{
	AE_CORE_LOG_INFO("Destroying a window");

	VOnDestroy();

	ImGui_ImplGlfw_Shutdown();
	ImGui_ImplOpenGL3_Shutdown();
	ImGui::DestroyContext();

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
