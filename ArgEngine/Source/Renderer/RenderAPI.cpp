#include <arg_pch.hpp>
#include "RenderAPI.hpp"

#include <glad/glad.h>


void Arg::Renderer::RenderAPI::Initialize()
{
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
}

void Arg::Renderer::RenderAPI::SetViewport(const Vec2i& position, const Vec2i& size)
{
	glViewport(position.x, position.y, size.x, size.y);
}

void Arg::Renderer::RenderAPI::SetClearColor(const Vec3& color)
{
	glClearColor(color.r, color.g, color.b, 1.0f);
}

void Arg::Renderer::RenderAPI::ClearBuffers()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void Arg::Renderer::RenderAPI::SetBlendingEnabled(bool bIsEnabled)
{
	if (bIsEnabled)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
	else
	{
		glDisable(GL_BLEND);
	}
}

void Arg::Renderer::RenderAPI::SetDrawingBackground(bool bDrawingBackground)
{
	if (bDrawingBackground)
	{
		glDepthFunc(GL_LEQUAL);
	}
	else
	{
		glDepthFunc(GL_LESS);
	}
}

void Arg::Renderer::RenderAPI::SetWireframe(bool bWireframe, float lineWidth)
{
	if (bWireframe)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glLineWidth(lineWidth);
	}
	else
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glLineWidth(1.0f);
	}
}

void Arg::Renderer::RenderAPI::SetCullBackFace(bool bCull)
{
	if (bCull)
	{
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
	}
	else
	{
		glDisable(GL_CULL_FACE);
	}
}
