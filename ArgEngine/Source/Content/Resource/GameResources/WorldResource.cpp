#include <arg_pch.hpp>
#include "WorldResource.hpp"

Arg::Content::WorldResource::WorldResource(const std::shared_ptr<Resource>& resource)
	: GameResource(resource)
{
}

auto Arg::Content::WorldResource::GetResourceFileExtension() const -> const std::string&
{
	return WORLD_RESOURCE_FILE_EXTENSION;
}

void Arg::Content::WorldResource::VPreLoad()
{
	GameResource::VPreLoad();
	m_pWorld = std::make_shared<Gameplay::GameWorld>(GetResource().get());
	m_pWorld->Create();
}

auto Arg::Content::WorldResource::VGetSerializableData() const -> ISerializable*
{
	return m_pWorld.get();
}

void Arg::Content::WorldResource::VUnload()
{
	m_pWorld = nullptr;
}
