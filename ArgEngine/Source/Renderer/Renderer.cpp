#include <arg_pch.hpp>
#include "Renderer.hpp"

#include "RenderAPI.hpp"

void Arg::Renderer::Renderer::BeginFrame(const FrameParams& params) const
{
	RenderAPI::Initialize();
	RenderAPI::SetViewport(Vec2(0.0f), params.ViewportSize);
	RenderAPI::SetClearColor(params.ClearColor);
	RenderAPI::ClearBuffers();
}

void Arg::Renderer::Renderer::BeginOpaque() const
{
	RenderAPI::SetBlendingEnabled(false);
}

void Arg::Renderer::Renderer::BeginTransparent() const
{
	RenderAPI::SetBlendingEnabled(true);
}

void Arg::Renderer::Renderer::DrawStaticMesh(
	const std::shared_ptr<const StaticMesh>& mesh,
	const std::shared_ptr<const Material>& material,
	const Mat4& transform
) const
{
	mesh->Draw();
}
