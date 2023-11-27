#include <arg_pch.hpp>
#include "TextureResource.hpp"

#include "Content/Yaml/Yaml.hpp"

Arg::Content::TextureResource::TextureResource(const std::shared_ptr<Resource>& resource)
	:m_pResource(resource)
{

}

void Arg::Content::TextureResource::Create()
{
	std::filesystem::path resourceFile = m_pResource->GetPath() / m_pResource->GetName();
	resourceFile.replace_extension(TEXTURE_RESOURCE_FILE_EXTENSION);

	const auto resourceData = YAML::LoadFile(resourceFile.string());
	const auto header = resourceData["Texture"];

	m_Width = header["Width"].as<int32_t>();
	m_Height = header["Height"].as<int32_t>();
	m_Format = static_cast<Renderer::TextureFormat>(header["TextureFormat"].as<uint32_t>());

	m_pTexture = std::make_unique<Renderer::Texture>();

	std::filesystem::path dataFile = m_pResource->GetPath() / m_pResource->GetName();
	dataFile.replace_extension(TEXTURE_DATA_FILE_EXTENSION);

	std::ifstream fileStream(dataFile, std::ios::binary | std::ios::ate);
	const auto fileSize = fileStream.tellg();
	std::vector<char> data(fileSize);
	fileStream.seekg(0, std::ios::beg);
	fileStream.read(data.data(), fileSize);
	fileStream.close();

	const Renderer::TextureData textureData = {
		.Data = data.data(),
		.Width = m_Width,
		.Height = m_Height,
		.Format = m_Format
	};
	m_pTexture->SetData(textureData);
}

void Arg::Content::TextureResource::Remove()
{
	std::filesystem::path resourceFile = m_pResource->GetPath() / m_pResource->GetName();
	resourceFile.replace_extension(TEXTURE_RESOURCE_FILE_EXTENSION);

	std::filesystem::path dataFile = m_pResource->GetPath() / m_pResource->GetName();
	dataFile.replace_extension(TEXTURE_DATA_FILE_EXTENSION);

	std::filesystem::remove(resourceFile);
	std::filesystem::remove(dataFile);
}

void Arg::Content::TextureResource::Save() const
{
	const auto filePath = (m_pResource->GetPath() / m_pResource->GetName())
		.replace_extension(TEXTURE_RESOURCE_FILE_EXTENSION);
	YAML::Node resourceData;
	if (std::filesystem::exists(filePath))
	{
		resourceData = YAML::LoadFile(filePath.string());
	}

	auto header = resourceData["Texture"];
	header["Width"] = m_Width;
	header["Height"] = m_Height;
	header["TextureFormat"] = static_cast<uint32_t>(m_Format);

	std::ofstream file(filePath, std::ofstream::out | std::ofstream::trunc);
	file << resourceData;
	file.close();
}

void Arg::Content::TextureResource::Rename(const std::string& name)
{
	std::filesystem::path resourceFile = m_pResource->GetPath() / m_pResource->GetName();
	resourceFile.replace_extension(TEXTURE_RESOURCE_FILE_EXTENSION);

	std::filesystem::path dataFile = m_pResource->GetPath() / m_pResource->GetName();
	dataFile.replace_extension(TEXTURE_DATA_FILE_EXTENSION);

	std::filesystem::path newResourceFile = m_pResource->GetPath() / name;
	resourceFile.replace_extension(TEXTURE_RESOURCE_FILE_EXTENSION);

	std::filesystem::path newDataFile = m_pResource->GetPath() / name;
	dataFile.replace_extension(TEXTURE_DATA_FILE_EXTENSION);

	std::filesystem::rename(
		resourceFile,
		newResourceFile
	);

	std::filesystem::rename(
		dataFile,
		newDataFile
	);
}

void Arg::Content::TextureResource::Move(const std::filesystem::path& destination)
{
	std::filesystem::path resourceFile = m_pResource->GetPath() / m_pResource->GetName();
	resourceFile.replace_extension(TEXTURE_RESOURCE_FILE_EXTENSION);

	std::filesystem::path dataFile = m_pResource->GetPath() / m_pResource->GetName();
	dataFile.replace_extension(TEXTURE_DATA_FILE_EXTENSION);

	std::filesystem::path newResourceFile = destination / m_pResource->GetName();
	resourceFile.replace_extension(TEXTURE_RESOURCE_FILE_EXTENSION);

	std::filesystem::path newDataFile = destination / m_pResource->GetName();
	dataFile.replace_extension(TEXTURE_DATA_FILE_EXTENSION);

	std::filesystem::copy_file(
		resourceFile,
		newResourceFile
	);
	std::filesystem::remove(resourceFile);

	std::filesystem::copy_file(
		dataFile,
		newDataFile
	);
	std::filesystem::remove(dataFile);
}

void Arg::Content::TextureResource::Bind(const int32_t unit) const
{
	m_pTexture->Bind(unit);
}

auto Arg::Content::TextureResource::GetRendererID() const-> const uint32_t&
{
	return m_pTexture->GetRendererID();
}
