#include <arg_pch.hpp>

#include "Core/Application.hpp"
#include "Debug/Assert.hpp"

namespace Arg
{
	int Main(int argc, char** argv)
	{
		const auto app = Arg::Client_CreateApplication();
		ARG_ASSERT(app != nullptr, "Failed to create the application!");
		app->StartUp();
		app->Run();
		app->Shutdown();
		return EXIT_SUCCESS;
	}
}

#ifdef ARG_RELEASE

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR pCmdLine, int nCmdShow)
{
	return Arg::Main(__argc, __argv);
}

#else

int main(int argc, char** argv)
{
	return Arg::Main(argc, argv);
}

#endif // ARG_RELEASE
