#pragma once

#include <arg_pch.hpp>

#include "Renderer/Texture.hpp"

namespace Arg
{
	namespace Import
	{
		class TextureImporter
		{
		public:
			TextureImporter() = default;
			~TextureImporter() = default;

			auto ImportFile(const std::string& file) -> bool;

			void Save(const std::string& fileName, const std::filesystem::path& destination);

			auto GetData() const -> const Renderer::TextureData&;

		private:
			std::unique_ptr<unsigned char> m_TextureData = nullptr;
			Renderer::TextureData m_Data = Renderer::TextureData();
		};
	}
}