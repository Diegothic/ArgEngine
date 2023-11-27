#pragma once

#include <arg_pch.hpp>

#include "Content/Resource/Resource.hpp"
#include "Renderer/Texture.hpp"

namespace Arg
{
	namespace Content
	{
		const std::string TEXTURE_RESOURCE_FILE_EXTENSION = "atex";
		const std::string TEXTURE_DATA_FILE_EXTENSION = "data";

		class TextureResource
		{
		public:
			TextureResource(const std::shared_ptr<Resource>& resource);
			TextureResource(const TextureResource&) = delete;
			~TextureResource() = default;

			void Create();
			void Remove();
			void Save() const;
			void Rename(const std::string& name);
			void Move(const std::filesystem::path& destination);

			auto GetWidth() const -> const int32_t& { return m_Width; }
			auto GetHeight() const -> const int32_t& { return m_Height; }
			auto GetFormat() const -> const Renderer::TextureFormat& { return m_Format; }

			auto GetRendererID() const-> const uint32_t&;
			void Bind(const int32_t unit) const;

		private:
			std::shared_ptr<Resource> m_pResource = nullptr;
			int32_t m_Width = 0;
			int32_t m_Height = 0;
			Renderer::TextureFormat m_Format = Renderer::TextureFormat::FormatRGB;
			std::unique_ptr<Renderer::Texture> m_pTexture = nullptr;
		};
	}
}
