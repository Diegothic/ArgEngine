#pragma once

#include <cstdint>

#include "TextureFormat.h"

namespace Arg
{
	struct TextureSpec
	{
		TextureFormat Format;
		uint32_t Width;
		uint32_t Height;
	};

	class Texture
	{
	public:
		Texture(const TextureSpec& spec);
		~Texture();

		const uint32_t& GetWidth() const { return m_Width; }
		const uint32_t& GetHeight() const { return m_Height; }

		void SetData(const void* data) const;
		void Bind() const;

	private:
		uint32_t m_RendererID = 0;
		TextureFormat m_Format = RGB;
		uint32_t m_Width = 0;
		uint32_t m_Height = 0;
	};
}
