#include <arg_pch.hpp>
#include "TextureImporter.hpp"

#include <stb/stb_image.h>

#include "Content/Resource/GameResources/TextureResource.hpp"
#include "Debug/Log.hpp"
#include "Content/Yaml/Yaml.hpp"

auto Arg::Import::TextureImporter::ImportFile(const std::string& file) -> bool
{
	stbi_set_flip_vertically_on_load(true);

	int nrChannels;
	m_TextureData = std::unique_ptr<unsigned char>(stbi_load(
		file.c_str(),
		&m_Data.Width,
		&m_Data.Height,
		&nrChannels,
		0));
	m_Data.Format = nrChannels == 3
		? Renderer::TextureFormat::FormatRGB
		: Renderer::TextureFormat::FormatRGBA;
	if (m_TextureData == nullptr)
	{
		ARG_CONSOLE_LOG_ERR("Failed to import texture \"%s\"!", file.c_str());
		return false;
	}

	m_Data.Data = m_TextureData.get();
	return true;
}

void Arg::Import::TextureImporter::Save(const std::string& fileName, const std::filesystem::path& destination)
{
	auto filePath = destination / fileName;
	filePath.replace_extension(Content::TEXTURE_RESOURCE_FILE_EXTENSION);
	std::string dataFileName = fileName;
	dataFileName.append(Content::RESOURCE_DATA_FILE_EXTENSION);

	YAML::Node resourceData;
	if (std::filesystem::exists(filePath))
	{
		resourceData = YAML::LoadFile(filePath.string());
	}

	auto header = resourceData["Texture"];
	header["Data"] = dataFileName;
	header["Width"] = m_Data.Width;
	header["Height"] = m_Data.Height;
	header["TextureFormat"] = static_cast<uint32_t>(m_Data.Format);

	std::ofstream file(filePath, std::ofstream::out | std::ofstream::trunc);
	file << resourceData;
	file.close();

	std::ofstream fileStream(destination / dataFileName,
		std::ios::out
		| std::ios::binary
		| std::ofstream::trunc
	);
	fileStream.write((char*)m_Data.Data,
		static_cast<std::streamsize>(
			m_Data.Width
			* m_Data.Height
			* (m_Data.Format == Renderer::TextureFormat::FormatRGB ? 3 : 4))
	);
	fileStream.close();
}

auto Arg::Import::TextureImporter::GetData() const -> const Renderer::TextureData&
{
	return m_Data;
}
