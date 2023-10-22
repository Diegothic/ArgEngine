#include "RenderAPI.h"

#include <glad/glad.h>

void Arg::RenderAPI::ClearBuffer() const
{
	glClear(GL_COLOR_BUFFER_BIT
		| GL_DEPTH_BUFFER_BIT
		| GL_STENCIL_BUFFER_BIT
	);
}

void Arg::RenderAPI::SetClearColor(Vec4 color) const
{
	glClearColor(color.r, color.g, color.b, color.a);
}

void Arg::RenderAPI::SetViewport(Vec2i position, Vec2i size) const
{
	glViewport(position.x, position.y, size.x, size.y);
}

void Arg::RenderAPI::SetDepthTestEnabled(bool value) const
{
	if (value)
	{
		glEnable(GL_DEPTH_TEST);
	}
	else
	{
		glDisable(GL_DEPTH_TEST);
	}
}
