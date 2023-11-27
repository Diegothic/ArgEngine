#include <arg_pch.hpp>
#include "Texture.hpp"

#include <glad/glad.h>

#include "Debug/Assert.hpp"

uint32_t Arg::Renderer::Texture::s_CurrentBoundID = -1;

Arg::Renderer::Texture::Texture()
{
	glGenTextures(1, &m_RendererID);
}

Arg::Renderer::Texture::~Texture()
{
	Unbind();
	glDeleteTextures(1, &m_RendererID);
}

void Arg::Renderer::Texture::SetData(const TextureData& data) const
{
	Bind();
	int32_t textureFormat = 0;
	switch (data.Format)
	{
	case TextureFormat::FormatRGB:
		textureFormat = GL_RGB;
		break;
	case TextureFormat::FormatRGBA:
		textureFormat = GL_RGBA;
		break;
	case TextureFormat::FormatDepth:
		textureFormat = GL_DEPTH_COMPONENT;
		break;
	case TextureFormat::FormatDepthStencil:
		textureFormat = GL_DEPTH_STENCIL;
		break;
	}

	ARG_ASSERT(textureFormat != 0, "Invalid texture format!");

	int32_t internalFormat = 0;
	switch (data.Format)
	{
	case TextureFormat::FormatRGB:
		internalFormat = GL_RGB;
		break;
	case TextureFormat::FormatRGBA:
		internalFormat = GL_RGBA;
		break;
	case TextureFormat::FormatDepth:
		internalFormat = GL_DEPTH_COMPONENT;
		break;
	case TextureFormat::FormatDepthStencil:
		internalFormat = GL_DEPTH24_STENCIL8;
		break;
	}

	uint32_t dataType = 0;
	switch (data.Format)
	{
	case TextureFormat::FormatRGB:
		dataType = GL_UNSIGNED_BYTE;
		break;
	case TextureFormat::FormatRGBA:
		dataType = GL_UNSIGNED_BYTE;
		break;
	case TextureFormat::FormatDepth:
		dataType = GL_FLOAT;
		break;
	case TextureFormat::FormatDepthStencil:
		dataType = GL_UNSIGNED_INT_24_8;
		break;
	}

	glTexImage2D(GL_TEXTURE_2D,
		0,
		internalFormat,
		data.Width,
		data.Height,
		0,
		textureFormat,
		dataType,
		data.Data);
	glGenerateMipmap(GL_TEXTURE_2D);
}

void Arg::Renderer::Texture::SetWrapMode(const TextureWrapMode mode) const
{
	Bind();
	int textureMode = 0;
	switch (mode)
	{
	case WrapModeRepeat:
		textureMode = GL_REPEAT;
		break;
	case WrapModeMirror:
		textureMode = GL_MIRRORED_REPEAT;
		break;
	case WrapModeClampToEdge:
		textureMode = GL_CLAMP_TO_EDGE;
		break;
	case WrapModeClampToBorder:
		textureMode = GL_CLAMP_TO_BORDER;
		break;
	}
	ARG_ASSERT(textureMode != 0, "Invalid texture mode!");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, textureMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, textureMode);
}

void Arg::Renderer::Texture::SetBorderColor(const Vec4& color) const
{
	Bind();
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, Math::ValuePtr(color));
}

void Arg::Renderer::Texture::SetFilter(const TextureFilter filter) const
{
	Bind();
	int textureMinFilter = 0;
	int textureMagFilter = 0;
	switch (filter)
	{
	case TextureFilter::FilterNearest:
		textureMinFilter = GL_NEAREST;
		textureMagFilter = GL_NEAREST;
		break;
	case TextureFilter::FilterLinear:
		textureMinFilter = GL_LINEAR;
		textureMagFilter = GL_LINEAR;
		break;
	}
	ARG_ASSERT(textureMinFilter != 0, "Invalid texture filter!");
	ARG_ASSERT(textureMagFilter != 0, "Invalid texture filter!");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, textureMinFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, textureMagFilter);
}

void Arg::Renderer::Texture::Bind() const
{
	if (s_CurrentBoundID == m_RendererID)
	{
		return;
	}

	s_CurrentBoundID = m_RendererID;
	glBindTexture(GL_TEXTURE_2D, m_RendererID);
}

void Arg::Renderer::Texture::AttachToFrameBuffer(const TextureAttachmentSlot slot) const
{
	Bind();
	uint32_t attachment = 0;
	switch (slot)
	{
	case TextureAttachmentSlot::AttachmentSlotColor:
		attachment = GL_COLOR_ATTACHMENT0;
		break;
	case TextureAttachmentSlot::AttachmentSlotDepth:
		attachment = GL_DEPTH_ATTACHMENT;
		break;
	case TextureAttachmentSlot::AttachmentSlotDepthStencil:
		attachment = GL_DEPTH_STENCIL_ATTACHMENT;
		break;
	}
	ARG_ASSERT(attachment != 0, "Invalid texture attachment!");
	glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, m_RendererID, 0);
}

void Arg::Renderer::Texture::Bind(const int32_t unit) const
{
	ARG_ASSERT(unit >= 0 && unit < 16, "Invalid texture unit!");
	glActiveTexture(GL_TEXTURE0 + unit);
	s_CurrentBoundID = m_RendererID;
	glBindTexture(GL_TEXTURE_2D, m_RendererID);
}

void Arg::Renderer::Texture::Unbind() const
{
	s_CurrentBoundID = -1;
	glBindTexture(GL_TEXTURE_2D, 0);
}
