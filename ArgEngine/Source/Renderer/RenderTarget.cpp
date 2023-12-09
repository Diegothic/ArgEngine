#include <arg_pch.hpp>
#include "RenderTarget.hpp"


Arg::Renderer::RenderTarget::RenderTarget()
{
	m_ColorAttachment.Bind();
	{
		const TextureData textureData{
			.Data = nullptr,
			.Width = m_Size.x,
			.Height = m_Size.y,
			.Format = TextureFormat::FormatRGB
		};
		m_ColorAttachment.SetFilter(FilterNearest);
		m_ColorAttachment.SetData(textureData);
	}
	m_ColorAttachment.Unbind();

	m_StencilDepthAttachment.Bind();
	{
		const TextureData textureData{
					.Data = nullptr,
					.Width = m_Size.x,
					.Height = m_Size.y,
					.Format = TextureFormat::FormatDepthStencil
		};
		m_StencilDepthAttachment.SetFilter(FilterNearest);
		m_StencilDepthAttachment.SetData(textureData);
	}
	m_StencilDepthAttachment.Unbind();

	m_Buffer.Bind();
	m_ColorAttachment.AttachToFrameBuffer(TextureAttachmentSlot::AttachmentSlotColor);
	m_ColorAttachment.Unbind();
	m_StencilDepthAttachment.AttachToFrameBuffer(TextureAttachmentSlot::AttachmentSlotDepthStencil);
	m_StencilDepthAttachment.Unbind();
	m_Buffer.Unbind();
}

void Arg::Renderer::RenderTarget::Begin() const
{
	m_Buffer.Bind();
}

void Arg::Renderer::RenderTarget::End() const
{
	m_Buffer.Unbind();
}

auto Arg::Renderer::RenderTarget::GetRendererID() const -> const uint32_t&
{
	return m_ColorAttachment.GetRendererID();
}

void Arg::Renderer::RenderTarget::SetSize(const Vec2i& size)
{
	m_Size = size;
	m_Buffer.Bind();
	m_ColorAttachment.Bind();
	{
		const TextureData textureData{
			.Data = nullptr,
			.Width = m_Size.x,
			.Height = m_Size.y,
			.Format = TextureFormat::FormatRGB
		};
		m_ColorAttachment.SetData(textureData);
	}
	m_ColorAttachment.Unbind();

	m_StencilDepthAttachment.Bind();
	{
		const TextureData textureData{
					.Data = nullptr,
					.Width = m_Size.x,
					.Height = m_Size.y,
					.Format = TextureFormat::FormatDepthStencil
		};
		m_StencilDepthAttachment.SetData(textureData);
	}
	m_Buffer.Unbind();
}
