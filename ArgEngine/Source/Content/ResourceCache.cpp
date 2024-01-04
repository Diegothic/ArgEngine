#include <arg_pch.hpp>
#include "ResourceCache.hpp"

#include "Debug/Assert.hpp"

#include "Resource/GameResources/ShaderResource.hpp"
#include "Resource/GameResources/TextureResource.hpp"
#include "Resource/GameResources/StaticModelResource.hpp"
#include "Resource/GameResources/WorldResource.hpp"
#include "Resource/GameResources/MaterialResource.hpp"
#include "Resource/GameResources/SkeletalAnimationResource.hpp"
#include "Resource/GameResources/SkeletalModelResource.hpp"
#include "Resource/GameResources/SkeletonResource.hpp"

void Arg::Content::ResourceCache::AddResource(const std::shared_ptr<Resource>& resource)
{
	ARG_ASSERT(!m_pResources.contains(resource->GetID()), "Invalid resource ID!");
	m_pResources[resource->GetID()] = resource;
	m_pResourcesByPathID[resource->GetPathID()] = resource;

	if (resource->GetType() != ResourceType::ResourceTypeFolder)
	{
		ARG_ASSERT(!m_pGameResources.contains(resource->GetID()), "Invalid resource ID!");
		switch (resource->GetType())
		{
		case ResourceType::ResourceTypeShader:
			{
				m_pGameResources[resource->GetID()] = std::make_shared<ShaderResource>(resource);
				break;
			}
		case ResourceType::ResourceTypeMaterial:
			{
				m_pGameResources[resource->GetID()] = std::make_shared<MaterialResource>(resource);
				break;
			}
		case ResourceType::ResourceTypeTexture:
			{
				m_pGameResources[resource->GetID()] = std::make_shared<TextureResource>(resource);
				break;
			}
		case ResourceType::ResourceTypeStaticModel:
			{
				m_pGameResources[resource->GetID()] = std::make_shared<StaticModelResource>(resource);
				break;
			}
		case ResourceType::ResourceTypeSkeletalModel:
			{
				m_pGameResources[resource->GetID()] = std::make_shared<SkeletalModelResource>(resource);
				break;
			}
		case ResourceType::ResourceTypeSkeleton:
			{
				m_pGameResources[resource->GetID()] = std::make_shared<SkeletonResource>(resource);
				break;
			}
		case ResourceType::ResourceTypeSkeletalAnimation:
			{
				m_pGameResources[resource->GetID()] = std::make_shared<SkeletalAnimationResource>(resource);
				break;
			}
		case ResourceType::ResourceTypeWorld:
			{
				m_pGameResources[resource->GetID()] = std::make_shared<WorldResource>(resource);
				break;
			}
		}
	}
}

void Arg::Content::ResourceCache::RemoveResource(const std::shared_ptr<Resource>& resource)
{
	const GUID resourceID = resource->GetID();
	const GUID resourcePathID = resource->GetPathID();
	ARG_ASSERT(m_pResources.contains(resourceID), "Invalid resource ID!");

	if (resource->GetType() != ResourceType::ResourceTypeFolder)
	{
		ARG_ASSERT(m_pGameResources.contains(resourceID), "Invalid resource ID!");
		m_pGameResources.at(resourceID)->VRemoveFiles();
	}

	FreeResource(resourceID);
	resource->Remove();

	m_pResources.erase(resourceID);
	m_pResourcesByPathID.erase(resourcePathID);
}

void Arg::Content::ResourceCache::SaveResource(const GUID ID) const
{
	ARG_ASSERT(m_pResources.contains(ID), "Invalid resource ID!");
	const auto& resource = m_pResources.at(ID);
	resource->Save();

	if (resource->GetType() != ResourceType::ResourceTypeFolder)
	{
		ARG_ASSERT(m_pGameResources.contains(ID), "Invalid resource ID!");
		const auto& gameResource = m_pGameResources.at(ID);
		if (gameResource->VIsSaveable())
		{
			m_pGameResources.at(ID)->VSaveFiles();
		}
	}
}

void Arg::Content::ResourceCache::RenameResource(
	const GUID ID,
	const std::string& name
)
{
	ARG_ASSERT(m_pResources.contains(ID), "Invalid resource ID!");
	const auto& resource = m_pResources.at(ID);

	if (resource->GetType() == ResourceType::ResourceTypeFolder)
	{
		std::filesystem::rename(
			resource->GetPath() / resource->GetName(),
			resource->GetPath() / name
		);
	}
	else
	{
		ARG_ASSERT(m_pGameResources.contains(ID), "Invalid resource ID!");
		m_pGameResources.at(ID)->VRenameFiles(name);
	}

	resource->Rename(name);
}

void Arg::Content::ResourceCache::MoveResource(
	const GUID ID,
	const std::filesystem::path& destination
)
{
	ARG_ASSERT(m_pResources.contains(ID), "Invalid resource ID!");
	const auto& resource = m_pResources.at(ID);
	resource->Move(destination);

	if (resource->GetType() != ResourceType::ResourceTypeFolder)
	{
		ARG_ASSERT(m_pGameResources.contains(ID), "Invalid resource ID!");
		m_pGameResources.at(ID)->VMoveFiles(destination);
	}
}

void Arg::Content::ResourceCache::RemoveResource(const GUID ID)
{
	ARG_ASSERT(m_pResources.contains(ID), "Invalid resource ID!");
	const auto& resource = m_pResources[ID];
	RemoveResource(resource);
}

auto Arg::Content::ResourceCache::GetResource(const GUID ID) -> std::shared_ptr<Resource>&
{
	ARG_ASSERT(m_pResources.contains(ID), "Invalid resource ID!");
	return m_pResources[ID];
}

auto Arg::Content::ResourceCache::GetGameResource(const GUID ID) -> std::shared_ptr<GameResource>&
{
	ARG_ASSERT(m_pResources.contains(ID), "Invalid resource ID!");
	ARG_ASSERT(m_pGameResources.contains(ID), "Invalid resource ID!");
	return m_pGameResources.at(ID);
}

auto Arg::Content::ResourceCache::GetGameResource(const std::string& path) -> std::shared_ptr<GameResource>&
{
	const GUID pathID = std::hash<std::string>{}(path);
	ARG_ASSERT(m_pResourcesByPathID.contains(pathID), "Invalid resource Path!");
	const GUID ID = m_pResourcesByPathID.at(pathID)->GetID();
	ARG_ASSERT(m_pResources.contains(ID), "Invalid resource ID!");
	ARG_ASSERT(m_pGameResources.contains(ID), "Invalid resource ID!");
	return m_pGameResources.at(ID);
}

void Arg::Content::ResourceCache::FreeResource(const GUID ID)
{
	ARG_ASSERT(m_pResources.contains(ID), "Invalid resource ID!");
	const auto& resource = m_pResources[ID];
	if (resource->GetType() != ResourceType::ResourceTypeFolder)
	{
		ARG_ASSERT(m_pGameResources.contains(ID), "Invalid resource ID!");
		m_pGameResources[ID] = nullptr;
		m_pGameResources.erase(ID);
	}
}

void Arg::Content::ResourceCache::SaveAll() const
{
	for (const auto& resourceKey : m_pResources
	     | std::ranges::views::keys
	)
	{
		SaveResource(resourceKey);
	}
}

auto Arg::Content::ResourceCache::IsValid(const GUID ID) -> bool
{
	return m_pResources.contains(ID);
}
