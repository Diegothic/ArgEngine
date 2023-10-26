#include <iostream>
#include <windows.h>

#include "Core/Application.h"
#include "Debug/CoreLogger.h"

namespace Arg
{
	int Main(int argc, char** argv)
	{
		Arg::StartUpCore();
		const Arg::Box<Arg::Application> app = Arg::CreateApplication();
		if (app == nullptr)
		{
			AE_CORE_LOG_ERR("Failed to create the application!");
			return EXIT_FAILURE;
		}

		app->Run();
		Arg::ShutDownCore();

		return EXIT_SUCCESS;
	}
}

#ifndef AE_DEBUG

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR pCmdLine, int nCmdShow)
{
	return Arg::Main(__argc, __argv);
}

#else

int main(int argc, char** argv)
{
	return Arg::Main(argc, argv);
}

#endif // AE_DEBUG
