#pragma once

#include <arg_pch.hpp>

namespace Arg
{
	class Application
	{
	public:
		Application() = default;
		virtual ~Application() = default;

		void StartUp();
		void Shutdown();
		void Run();
		void Close();

		auto IsRunning() const -> bool { return m_bIsRunning; }

	protected:
		virtual void VOnStartUp() = 0;
		virtual void VOnShutDown() = 0;
		virtual void VOnRun() = 0;

	private:
		bool m_bIsRunning = false;
	};

	std::unique_ptr<Application> Client_CreateApplication();
}
