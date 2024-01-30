#include <arg_pch.hpp>
#include "Renderer.hpp"

#include "RenderAPI.hpp"
#include "Core/Math/Math.hpp"

void Arg::Renderer::Renderer::Initialize()
{
	m_pWhiteTexture = std::make_unique<Texture>();
	{
		uint32_t whiteTexturePixel[] = {0xFFFFFFFF};
		const TextureData whiteTextureData{
			.Data = whiteTexturePixel,
			.Width = 1,
			.Height = 1,
			.Format = TextureFormat::FormatRGBA
		};
		m_pWhiteTexture->SetData(whiteTextureData);
	}
	m_pWhiteTexture->Unbind();

	m_pBlackTexture = std::make_unique<Texture>();
	{
		uint32_t blackTexturePixel[] = {0x000000FF};
		const TextureData blackTextureData{
			.Data = blackTexturePixel,
			.Width = 1,
			.Height = 1,
			.Format = TextureFormat::FormatRGBA
		};
		m_pBlackTexture->SetData(blackTextureData);
	}
	m_pBlackTexture->Unbind();
}

void Arg::Renderer::Renderer::BeginFrame(const FrameParams& params) const
{
	RenderAPI::Initialize();
	RenderAPI::SetViewport(Vec2(0.0f), params.ViewportSize);
	RenderAPI::SetClearColor(params.ClearColor);
	RenderAPI::ClearBuffers();

	m_pWhiteTexture->Bind(0);
	m_pBlackTexture->Bind(1);

	m_pWhiteTexture->Unbind();
	m_pBlackTexture->Unbind();
}

void Arg::Renderer::Renderer::EndFrame() const
{
	m_pWhiteTexture->Unbind();
	m_pBlackTexture->Unbind();
}

void Arg::Renderer::Renderer::BeginOpaque() const
{
	RenderAPI::SetBlendingEnabled(false);
}

void Arg::Renderer::Renderer::BeginTransparent() const
{
	RenderAPI::SetBlendingEnabled(true);
}

void Arg::Renderer::Renderer::BeginSkybox() const
{
	RenderAPI::SetDrawingBackground(true);
}

void Arg::Renderer::Renderer::EndSkybox() const
{
	RenderAPI::SetDrawingBackground(false);
}

void Arg::Renderer::Renderer::BeginDebug(float lineWidth) const
{
	RenderAPI::SetWireframe(true, lineWidth);
	RenderAPI::SetCullBackFace(false);
}

void Arg::Renderer::Renderer::EndDebug() const
{
	RenderAPI::SetWireframe(false);
	RenderAPI::SetCullBackFace(true);
}
