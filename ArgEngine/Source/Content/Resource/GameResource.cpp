#include <arg_pch.hpp>
#include "GameResource.hpp"

#include "Debug/Assert.hpp"
#include "Content/ResourceCache.hpp"

Arg::Content::GameResource::GameResource(const std::shared_ptr<Resource>& resource)
	: m_pResource(resource)
{

}

void Arg::Content::GameResource::AddRef()
{
	m_RefCount++;
	if (m_RefCount == 1)
	{
		Load();
	}
}

void Arg::Content::GameResource::FreeRef()
{
	ARG_ASSERT(m_RefCount > 0, "Invalid ref count!");
	m_RefCount--;
	if (m_RefCount == 0)
	{
		VUnload();
	}
}

auto Arg::Content::GameResource::GetName() const-> const std::string&
{
	return m_pResource->GetName();
}

void Arg::Content::GameResource::VRemoveFiles()
{
	const auto resourceFile = GetResourceFilePath();
	std::filesystem::remove(resourceFile);
}

void Arg::Content::GameResource::VSaveFiles() const
{
	const auto resourceFile = GetResourceFilePath();
	const auto serializableData = VGetSerializableData();
	if (serializableData == nullptr)
	{
		return;
	}

	serializableData->Serialize(resourceFile);
}

void Arg::Content::GameResource::VRenameFiles(const std::string& name)
{
	const auto resourceFile = GetResourceFilePath();
	auto newResourceFile = GetResource()->GetPath() / name;
	newResourceFile.replace_extension(GetResourceFileExtension());

	std::filesystem::rename(
		resourceFile,
		newResourceFile
	);
}

void Arg::Content::GameResource::VMoveFiles(const std::filesystem::path& destination)
{
	const auto resourceFile = GetResourceFilePath();
	auto newResourceFile = destination / GetResource()->GetName();
	newResourceFile.replace_extension(GetResourceFileExtension());

	std::filesystem::copy_file(
		resourceFile,
		newResourceFile
	);
	std::filesystem::remove(resourceFile);
}

auto Arg::Content::GameResource::GetResourceFilePath() const -> std::filesystem::path
{
	std::filesystem::path resourceFile = GetResource()->GetFullPath();
	resourceFile.replace_extension(GetResourceFileExtension());
	return resourceFile;
}

auto Arg::Content::GameResource::GetResourceCache() -> ResourceCache*
{
	return GetResource()->GetResourceCache();
}

void Arg::Content::GameResource::Load()
{
	VPreLoad();
	const auto resourceFile = GetResourceFilePath();
	const auto serializableData = VGetSerializableData();
	serializableData->Deserialize(resourceFile);
	VOnLoad();
	VPostLoad();
}
