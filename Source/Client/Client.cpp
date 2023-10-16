#include "Arg/Core.h"
#include "Editor/EditorWindow.h"

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
