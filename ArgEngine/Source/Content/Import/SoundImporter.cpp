#include <arg_pch.hpp>
#include "SoundImporter.hpp"

auto Arg::Import::SoundImporter::ImportFile(const std::string& file) -> bool
{
	const std::filesystem::path filePath(file);
	if (filePath.has_extension())
	{
		m_SoundData.Format = filePath.extension().string();
	}

	std::ifstream fileStream(filePath, std::ios::binary | std::ios::ate);
	const auto fileSize = fileStream.tellg();
	m_pData = std::make_unique<std::vector<char>>(fileSize);
	fileStream.seekg(0, std::ios::beg);
	fileStream.read(m_pData->data(), fileSize);
	fileStream.close();

	return true;
}

void Arg::Import::SoundImporter::Save(
	const std::string& fileName,
	const std::filesystem::path& destination
)
{
	auto filePath = destination / fileName;
	filePath.replace_extension(Content::SOUND_RESOURCE_FILE_EXTENSION);
	std::string dataFileName = fileName;
	dataFileName.append(Content::RESOURCE_DATA_FILE_EXTENSION);

	YAML::Node resourceData;
	if (std::filesystem::exists(filePath))
	{
		resourceData = YAML::LoadFile(filePath.string());
	}

	auto header = resourceData["Sound"];
	header["Format"] = m_SoundData.Format;

	std::ofstream file(filePath, std::ofstream::out | std::ofstream::trunc);
	file << resourceData;
	file.close();

	std::ofstream fileStream(
		destination / dataFileName,
		std::ios::out
		| std::ios::binary
		| std::ofstream::trunc
	);
	fileStream.write(
		(char*)m_pData->data(),
		static_cast<std::streamsize>(m_pData->size())
	);
	fileStream.close();

	m_pData = nullptr;
}
