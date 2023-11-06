#pragma once

#include "Renderer.h"
#include "Resources/Content.h"

namespace Arg
{
	struct RenderContext
	{
		Renderer* Renderer;
		uint32_t Shader;
		Content* Content;
	};
}
