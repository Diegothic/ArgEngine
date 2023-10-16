#pragma once

#include "Arg/Memory.h"

namespace Arg
{
	class Window;
	Rc<Window> Client_CreateWindow();

	void StartUpCore();
	void ShutDownCore();

	class Application
	{
	public:
		Application(const Rc<Window>& window);
		virtual ~Application() = default;

		void Run() const;

	private:
		Rc<Window> m_pWindow;
	};

	Box<Application> CreateApplication();
}
