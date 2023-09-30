#pragma once

namespace Arg
{
	class Window;

	void StartUpCore();
	void ShutDownCore();

	class Application
	{
	public:
		Application(Window* window);
		virtual ~Application();

		void Run() const;

	private:
		Window* m_pWindow;
	};

	Application* CreateApplication();
}
