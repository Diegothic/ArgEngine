#include <arg_pch.hpp>
#include "ResourceFolder.hpp"

#include "Debug/Assert.hpp"

Arg::Content::ResourceFolder::ResourceFolder(const std::shared_ptr<Resource>& resource)
{
	m_pResource = resource;
}

auto Arg::Content::ResourceFolder::operator==(const ResourceFolder& other) const -> bool
{
	return GetID() == other.GetID();
}

void Arg::Content::ResourceFolder::ChangeColor(const Vec3& color)
{
	m_pResource->SetColor(color);
}

auto Arg::Content::ResourceFolder::GetID() const -> const GUID&
{
	return m_pResource->GetID();
}

auto Arg::Content::ResourceFolder::GetPath() const -> const std::filesystem::path&
{
	return m_pResource->GetPath();
}

auto Arg::Content::ResourceFolder::GetFullPath() const -> std::filesystem::path
{
	return m_pResource->GetFullPath();
}

auto Arg::Content::ResourceFolder::GetName() const -> const std::string&
{
	return m_pResource->GetName();
}

auto Arg::Content::ResourceFolder::GetColor() const -> const Vec3&
{
	return m_pResource->GetColor();
}

void Arg::Content::ResourceFolder::SetParentFolder(const std::shared_ptr<ResourceFolder>& parentFolder)
{
	m_pParentFolder = parentFolder;
}

auto Arg::Content::ResourceFolder::GetSubfolder(const size_t index) const -> const std::shared_ptr<ResourceFolder>&
{
	ARG_ASSERT(index < m_pSubfolders.size());
	return m_pSubfolders[index];
}

void Arg::Content::ResourceFolder::AddSubfolder(const std::shared_ptr<ResourceFolder>& subfolder)
{
	m_pSubfolders.push_back(subfolder);
}

void Arg::Content::ResourceFolder::RemoveSubfolder(const std::shared_ptr<ResourceFolder>& subfolder)
{
	std::erase(m_pSubfolders, subfolder);
}

void Arg::Content::ResourceFolder::SortSubfolders()
{
	std::ranges::sort(
		m_pSubfolders,
		[](
		const std::shared_ptr<ResourceFolder>& a,
		const std::shared_ptr<ResourceFolder>& b
	)
		{
			auto nameA = a->GetName();
			auto nameB = b->GetName();
			std::ranges::transform(nameA, nameA.begin(), static_cast<int(*)(int)>(std::tolower));
			std::ranges::transform(nameB, nameB.begin(), static_cast<int(*)(int)>(std::tolower));
			return nameA < nameB;
		}
	);
}

auto Arg::Content::ResourceFolder::GetResource(const size_t index) const -> const std::shared_ptr<Resource>&
{
	ARG_ASSERT(index < m_pResources.size());
	return m_pResources[index];
}

void Arg::Content::ResourceFolder::AddResource(const std::shared_ptr<Resource>& resource)
{
	m_pResources.push_back(resource);
}

void Arg::Content::ResourceFolder::RemoveResource(const std::shared_ptr<Resource>& resource)
{
	std::erase(m_pResources, resource);
}
