#include <arg_pch.hpp>
#include "GameDataResource.hpp"

Arg::Content::GameDataResource::GameDataResource(const std::shared_ptr<Resource>& resource)
	: GameResource(resource)
{

}

void Arg::Content::GameDataResource::VRemoveFiles()
{
	GameResource::VRemoveFiles();
	const auto dataFile = GetDataFilePath();
	std::filesystem::remove(dataFile);
}

void Arg::Content::GameDataResource::VRenameFiles(const std::string& name)
{
	GameResource::VRenameFiles(name);
	const auto dataFile = GetDataFilePath();
	auto newDataFile = GetResource()->GetPath() / name;
	newDataFile.replace_extension(RESOURCE_DATA_FILE_EXTENSION);

	std::filesystem::rename(
		dataFile,
		newDataFile
	);
}

void Arg::Content::GameDataResource::VMoveFiles(const std::filesystem::path& destination)
{
	GameResource::VMoveFiles(destination);
	const auto dataFile = GetDataFilePath();
	auto newDataFile = destination / GetResource()->GetName();
	newDataFile.replace_extension(RESOURCE_DATA_FILE_EXTENSION);

	std::filesystem::copy_file(
		dataFile,
		newDataFile
	);
	std::filesystem::remove(dataFile);
}

void Arg::Content::GameDataResource::VPreLoad()
{
	GameResource::VPreLoad();
	const auto dataFile = GetDataFilePath();
	std::ifstream fileStream(dataFile, std::ios::binary | std::ios::ate);
	const auto fileSize = fileStream.tellg();
	m_pData = std::make_unique<std::vector<char>>(fileSize);
	fileStream.seekg(0, std::ios::beg);
	fileStream.read(m_pData->data(), fileSize);
	fileStream.close();
}

void Arg::Content::GameDataResource::VPostLoad()
{
	GameResource::VPostLoad();
	m_pData = nullptr;
}

auto Arg::Content::GameDataResource::GetDataFilePath() const -> std::filesystem::path
{
	std::filesystem::path dataFile = GetResource()->GetFullPath();
	dataFile.replace_extension(RESOURCE_DATA_FILE_EXTENSION);
	return dataFile;
}
