#include <arg_pch.hpp>
#include "CubeMap.hpp"

#include <glad/glad.h>

#include "Debug/Assert.hpp"

Arg::Renderer::CubeMap::CubeMap()
{
	glGenTextures(1, &m_RendererID);
	Bind();

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	Unbind();
}

Arg::Renderer::CubeMap::~CubeMap()
{
	Unbind();
	glDeleteTextures(1, &m_RendererID);
}

void Arg::Renderer::CubeMap::SetFaces(const std::vector<TextureHandle>& textures) const
{
	ARG_ASSERT(textures.size() == 6);
	Bind();

	for (size_t i = 0; i < 6; i++)
	{
		const auto& texture = textures[i].Get();
		const char* textureData = texture->GetTextureData();

		glTexImage2D(
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
			0,
			GL_RGB,
			texture->GetTexture()->GetWidth(),
			texture->GetTexture()->GetHeight(),
			0,
			GL_RGB,
			GL_UNSIGNED_BYTE,
			textureData
		);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	Unbind();
}

void Arg::Renderer::CubeMap::Bind() const
{
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID);
}

void Arg::Renderer::CubeMap::Bind(int32_t unit) const
{
	ARG_ASSERT_M(unit >= 0 && unit < 16, "Invalid texture unit!");
	glActiveTexture(GL_TEXTURE0 + unit);
	Bind();
}

void Arg::Renderer::CubeMap::Unbind() const
{
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}
