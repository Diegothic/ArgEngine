#include <arg_pch.hpp>
#include "Content.hpp"

#include "Yaml/Yaml.hpp"

#include "Debug/Log.hpp"
#include "Resource/Resource.hpp"

Arg::Content::Content::Content(const ContentSpec& spec)
	: m_RootDirectory(spec.RootDirectory),
	m_pResourceCache(spec.ResourceCache)
{

}

void Arg::Content::Content::Initialize()
{
	m_pFolders.clear();

	if (!std::filesystem::exists(m_RootDirectory)
		|| !std::filesystem::is_directory(m_RootDirectory))
	{
		std::filesystem::create_directory(m_RootDirectory);
	}

	auto resource = std::make_shared<Resource>();
	resource->Create(
		GUID::Empty,
		"Content",
		ResourceType::ResourceTypeFolder,
		m_RootDirectory.parent_path(),
		m_RootDirectory
	);

	m_pResourceCache->AddResource(resource);

	const auto rootFolderResource = std::make_shared<ResourceFolder>(resource);
	m_pFolders.push_back(rootFolderResource);

	ScanDirectory(m_RootDirectory, rootFolderResource);
}

auto Arg::Content::Content::GetRootFolder() const -> const std::shared_ptr<ResourceFolder>&
{
	return m_pFolders[0];
}

void Arg::Content::Content::CreateFolder(const std::shared_ptr<ResourceFolder>& parent)
{
	const auto folderPath = parent->GetPath() / parent->GetName();
	std::string folderName = "New folder";
	if (std::filesystem::exists(folderPath / folderName))
	{
		for (size_t i = 0; i < 999; i++)
		{
			folderName = std::format("New folder ({})", i);
			if (!std::filesystem::exists(folderPath / folderName))
			{
				break;
			}
		}
	}

	std::filesystem::create_directory(folderPath / folderName);

	auto resource = std::make_shared<Resource>();
	resource->Create(
		GUID(m_IDGenerator.Next()),
		folderName,
		ResourceType::ResourceTypeFolder,
		folderPath,
		m_RootDirectory
	);

	m_pResourceCache->AddResource(resource);

	auto resourceFolder = std::make_shared<ResourceFolder>(resource);
	resourceFolder->SetParentFolder(parent);
	parent->AddSubfolder(resourceFolder);
	m_pFolders.push_back(resourceFolder);
	parent->SortSubfolders();

	m_pResourceCache->SaveResource(resource->GetID());
}

void Arg::Content::Content::RemoveFolder(const std::shared_ptr<ResourceFolder>& folder)
{
	const GUID ID = folder->GetID();
	const auto parent = folder->GetParentFolder();
	parent->RemoveSubfolder(folder);
	const size_t subfolderCount = folder->GetSubfolderCount();
	for (size_t i = subfolderCount; i > 0; i--)
	{
		RemoveFolder(folder->GetSubfolder(i - 1));
	}

	const size_t resourceCount = folder->GetResourceCount();
	for (size_t i = resourceCount; i > 0; i--)
	{
		m_pResourceCache->RemoveResource(folder->GetResource(i - 1)->GetID());
	}

	const auto folderPath = folder->GetPath() / folder->GetName();
	auto folderMetaFilePath = folderPath;
	folderMetaFilePath.replace_extension(".meta");
	std::filesystem::remove(folderMetaFilePath);
	std::filesystem::remove_all(folderPath);

	m_pResourceCache->RemoveResource(ID);
	const auto it = std::ranges::find(m_pFolders, folder);
	if (it != m_pFolders.end())
	{
		m_pFolders.erase(it);
	}
}

void Arg::Content::Content::RenameFolder(
	const std::shared_ptr<ResourceFolder>& folder,
	const std::string& name
)
{
	const GUID ID = folder->GetID();
	const auto& resource = m_pResourceCache->GetResource(ID);
	m_pResourceCache->RenameResource(ID, name);

	UpdatePaths(folder);
}

auto Arg::Content::Content::CreateResource(
	const std::string& name,
	const ResourceType type,
	const std::shared_ptr<ResourceFolder>& parent
) ->std::shared_ptr<Resource>&
{
	auto resource = std::make_shared<Resource>();
	resource->Create(
		GUID(m_IDGenerator.Next()),
		name,
		type,
		parent->GetPath() / parent->GetName(),
		m_RootDirectory
	);
	m_pResourceCache->AddResource(resource);
	parent->AddResource(resource);

	m_pResourceCache->SaveResource(resource->GetID());
	return m_pResourceCache->GetResource(resource->GetID());
}

void Arg::Content::Content::RemoveResource(
	const std::shared_ptr<Resource>& resource,
	const std::shared_ptr<ResourceFolder>& parentFolder
)
{
	const GUID resourceID = resource->GetID();
	parentFolder->RemoveResource(resource);
	m_pResourceCache->RemoveResource(resourceID);
}

void Arg::Content::Content::MoveResource(
	const std::shared_ptr<Resource>& resource, 
	const std::shared_ptr<ResourceFolder>& source, 
	const std::shared_ptr<ResourceFolder>& destination
)
{
	if (source == destination)
	{
		return;
	}

	m_pResourceCache->MoveResource(
		resource->GetID(), 
		destination->GetPath() / destination->GetName()
	);
	source->RemoveResource(resource);
	destination->AddResource(resource);

	UpdatePaths(destination);
}

void Arg::Content::Content::ScanDirectory(
	const std::filesystem::path& directory,
	const std::shared_ptr<ResourceFolder>& folder
)
{
	for (const auto& entry : std::filesystem::directory_iterator(directory))
	{
		const auto& entryPath = entry.path();
		if (entryPath.extension() == ".meta")
		{
			auto foundResource = std::make_shared<Resource>();
			foundResource->Create(entryPath, m_RootDirectory);
			if (foundResource->GetID() == GUID::Empty)
			{
				const GUID generatedID = GUID(m_IDGenerator.Next());
				foundResource->SetID(generatedID);
			}

			m_pResourceCache->AddResource(foundResource);

			if (foundResource->GetType() == ResourceType::ResourceTypeFolder)
			{
				auto foderDirectory = entryPath;
				foderDirectory.replace_extension("");

				auto resourceFolder = std::make_shared<ResourceFolder>(foundResource);
				resourceFolder->SetParentFolder(folder);
				folder->AddSubfolder(resourceFolder);
				m_pFolders.push_back(resourceFolder);

				ScanDirectory(foderDirectory, resourceFolder);
			}
			else
			{
				folder->AddResource(foundResource);
			}
		}
	}

	folder->SortSubfolders();
}

void Arg::Content::Content::UpdatePaths(const std::shared_ptr<ResourceFolder>& folder)
{
	const auto newPath = folder->GetPath() / folder->GetName();

	const size_t subfolderCount = folder->GetSubfolderCount();
	for (size_t i = subfolderCount; i > 0; i--)
	{
		const std::shared_ptr<ResourceFolder>& subfolder = folder->GetSubfolder(i - 1);

		const auto& subfolderResource = m_pResourceCache->GetResource(subfolder->GetID());
		subfolderResource->SetPath(
			newPath,
			m_RootDirectory
		);

		UpdatePaths(subfolder);
	}

	const size_t resourceCount = folder->GetResourceCount();
	for (size_t i = resourceCount; i > 0; i--)
	{
		const std::shared_ptr<Resource>& resourceChild = folder->GetResource(i - 1);
		resourceChild->SetPath(
			newPath,
			m_RootDirectory
		);
	}
}
