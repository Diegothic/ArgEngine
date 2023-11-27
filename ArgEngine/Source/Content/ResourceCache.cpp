#include <arg_pch.hpp>
#include "ResourceCache.hpp"

#include "Debug/Assert.hpp"

void Arg::Content::ResourceCache::AddResource(const std::shared_ptr<Resource>& resource)
{
	ARG_ASSERT(!m_pResources.contains(resource->GetID()), "Invalid resource ID!");
	m_pResources[resource->GetID()] = resource;
	m_pResourcesByPathID[resource->GetPathID()] = resource;
}

void Arg::Content::ResourceCache::RemoveResource(const std::shared_ptr<Resource>& resource)
{
	const GUID resourceID = resource->GetID();
	const GUID resourcePathID = resource->GetPathID();
	ARG_ASSERT(m_pResources.contains(resourceID), "Invalid resource ID!");

	switch (resource->GetType())
	{
	case ResourceType::ResourceTypeTexture:
	{
		std::shared_ptr<TextureResource> textureResource = GetResourceByType(resourceID);
		textureResource->Remove();
		break;
	}
	default:
		break;
	}

	FreeResource(resourceID);
	resource->Remove();

	m_pResources.erase(resourceID);
	m_pResourcesByPathID.erase(resourcePathID);
}

void Arg::Content::ResourceCache::SaveResource(const GUID ID)
{
	ARG_ASSERT(m_pResources.contains(ID), "Invalid resource ID!");
	const auto& resource = m_pResources.at(ID);
	resource->Save();

	switch (resource->GetType())
	{
	case ResourceType::ResourceTypeTexture:
	{
		std::shared_ptr<TextureResource> textureResource = GetResourceByType(resource->GetID());
		textureResource->Save();
		break;
	}
	default:
		break;
	}
}

void Arg::Content::ResourceCache::RenameResource(
	const GUID ID,
	const std::string& name
)
{
	ARG_ASSERT(m_pResources.contains(ID), "Invalid resource ID!");
	const auto& resource = m_pResources.at(ID);

	switch (resource->GetType())
	{
	case ResourceType::ResourceTypeFolder:
	{
		std::filesystem::rename(
			resource->GetPath() / resource->GetName(),
			resource->GetPath() / name
		);
		break;
	}
	case ResourceType::ResourceTypeTexture:
	{
		std::shared_ptr<TextureResource> textureResource = GetResourceByType(resource->GetID());
		textureResource->Rename(name);
		break;
	}
	default:
		break;
	}

	resource->Rename(name);
}

void Arg::Content::ResourceCache::MoveResource(const GUID ID, const std::filesystem::path& destination)
{
	ARG_ASSERT(m_pResources.contains(ID), "Invalid resource ID!");
	const auto& resource = m_pResources.at(ID);
	resource->Move(destination);

	switch (resource->GetType())
	{
	case ResourceType::ResourceTypeTexture:
	{
		std::shared_ptr<TextureResource> textureResource = GetResourceByType(resource->GetID());
		textureResource->Move(destination);
		break;
	}
	default:
		break;
	}
}

void Arg::Content::ResourceCache::RemoveResource(const GUID ID)
{
	ARG_ASSERT(m_pResources.contains(ID), "Invalid resource ID!");
	const auto& resource = m_pResources[ID];
	RemoveResource(resource);
}

auto Arg::Content::ResourceCache::GetResourceByType(const GUID ID) -> std::shared_ptr<TextureResource>&
{
	const auto& resource = m_pResources.at(ID);
	ARG_ASSERT(resource->GetType() == ResourceType::ResourceTypeTexture, "Invalid resource type!");
	if (m_pLoadedTextures.contains(ID))
	{
		return m_pLoadedTextures.at(ID);
	}

	auto textureResource = std::make_shared<TextureResource>(resource);
	textureResource->Create();

	m_pLoadedTextures[ID] = textureResource;
	return m_pLoadedTextures[ID];
}

auto Arg::Content::ResourceCache::GetResource(const GUID ID) -> std::shared_ptr<Resource>&
{
	ARG_ASSERT(m_pResources.contains(ID), "Invalid resource ID!");
	return m_pResources[ID];
}

void Arg::Content::ResourceCache::FreeResource(const GUID ID)
{
	ARG_ASSERT(m_pResources.contains(ID), "Invalid resource ID!");
	const auto& resource = m_pResources[ID];
	switch (resource->GetType())
	{
	case ResourceType::ResourceTypeTexture:
	{
		if (m_pLoadedTextures.contains(ID))
		{
			m_pLoadedTextures[ID] = nullptr;
			m_pLoadedTextures.erase(ID);
		}
		break;
	}
	default:
		break;
	}
}

auto Arg::Content::ResourceCache::IsValid(const GUID ID) -> bool
{
	return m_pResources.contains(ID);
}
