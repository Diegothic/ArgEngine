#pragma once

#include <arg_pch.hpp>

#include "Core/Input/Input.hpp"

namespace Arg
{
	namespace WindowSystem
	{
		void StartUp();
		void Shutdown();
		void InitializeGraphics();
	}

	enum WindowMode
	{
		Windowed,
		Fullscreen,
	};

	struct WindowSpec
	{
		std::string Title = "Window";
		int32_t Width = 1280;
		int32_t Height = 720;
		WindowMode Mode = WindowMode::Windowed;
		bool bIsVSync = true;
	};

	class Window
	{
	public:
		Window(const WindowSpec& spec);
		~Window() = default;

		auto Create() -> bool;
		void MakeCurrent();
		void SwapBuffers();
		void Destroy();

		void Update();
		auto ShouldClose() const -> bool;
		void Close();

		auto GetHandle() const -> void* { return m_pWindowHandle; }
		auto GetInput() const -> const Input::Input& { return m_Input; }
		auto GetElapsedTime() const -> float;
		auto GetTitle() const -> const std::string& { return m_Title; }
		auto GetWidth() const -> int32_t;
		void SetWidth(int width);
		auto GetHeight() const -> int32_t;
		void SetHeight(int height);
		auto GetMode() const -> const WindowMode& { return m_Mode; }
		void SetMode(WindowMode mode);
		auto IsVSyncEnabled() const -> const bool& { return m_bIsVSync; }
		void SetVSyncEnabled(bool bIsEnabled);
		void SetCursorMode(Input::CursorMode cursorMode);

	protected:
		static bool s_bHasInitializedGraphics;

	private:
		void* m_pWindowHandle = nullptr;

		std::string m_Title;
		int32_t m_Width;
		int32_t m_Height;
		WindowMode m_Mode;
		bool m_bIsVSync;

		Input::Input m_Input;
	};
}
