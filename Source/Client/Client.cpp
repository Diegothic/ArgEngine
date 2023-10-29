#include "Core/Application.h"
#include "Editor/EditorWindow.h"
#include "Core/Memory/SmartPtr.h"

Arg::Rc<Arg::Window> Arg::Client_CreateWindow()
{
	const Arg::WindowSpec windowSpec{
		.Title = "ArgEngine",
		.Width = 1600,
		.Height = 900,
		.VSync = true,
	};

	return Arg::NewRc<Arg::EditorWindow>(windowSpec);
}
