#pragma once

#include <arg_pch.hpp>

#include "Content/Resource/Resource.hpp"
#include "Content/Resource/GameResources/GameDataResource.hpp"
#include "Renderer/Texture.hpp"

namespace Arg
{
	namespace Content
	{
		const std::string TEXTURE_RESOURCE_FILE_EXTENSION = ".atex";

		class TextureResource : public GameDataResource
		{
		public:
			TextureResource(const std::shared_ptr<Resource>& resource);
			TextureResource(const TextureResource&) = delete;
			~TextureResource() = default;

			auto GetTexture() const -> const std::shared_ptr<Renderer::Texture>&
			{
				return m_pTexture;
			}

			auto GetTextureData() -> char*;
			void FreeTextureData();

		protected:
			auto GetResourceFileExtension() const -> const std::string& override;

			void VPreLoad() override;
			void VOnLoad() override;
			auto VGetSerializableData() const -> ISerializable* override;
			void VUnload() override;

		private:
			std::shared_ptr<Renderer::Texture> m_pTexture = nullptr;
		};
	}
}
