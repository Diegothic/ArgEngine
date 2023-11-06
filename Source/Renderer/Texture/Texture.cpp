#include "Texture.h"


Arg::Texture::Texture(const TextureSpec& spec)
	: m_Format(spec.Format),
	m_Width(spec.Width),
	m_Height(spec.Height)
{
	glGenTextures(1, &m_RendererID);
	glBindTexture(GL_TEXTURE_2D, m_RendererID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

Arg::Texture::~Texture()
{
	glDeleteTextures(1, &m_RendererID);
}

void Arg::Texture::SetData(const void* data) const
{
	glBindTexture(GL_TEXTURE_2D, m_RendererID);
	glTexImage2D(GL_TEXTURE_2D,
		0,
		GL_RGB,
		static_cast<int>(m_Width),
		static_cast<int>(m_Height),
		0,
		m_Format,
		GL_UNSIGNED_BYTE,
		data);
	glGenerateMipmap(GL_TEXTURE_2D);
}

void Arg::Texture::Bind() const
{
	glBindTexture(GL_TEXTURE_2D, m_RendererID);
}
