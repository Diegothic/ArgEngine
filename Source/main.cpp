#include <iostream>
#include <windows.h>

#include "Core.h"

namespace Arg
{
	int Main(int argc, char** argv)
	{
		Arg::StartUpCore();
		const Arg::Application* app = Arg::CreateApplication();
		if (app == nullptr)
		{
			std::cout << "Error: Failed to create the application!" << std::endl;
			return EXIT_FAILURE;
		}

		app->Run();
		delete app;
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
