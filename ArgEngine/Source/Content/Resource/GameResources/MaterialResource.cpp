#include <arg_pch.hpp>
#include "MaterialResource.hpp"

Arg::Content::MaterialResource::MaterialResource(const std::shared_ptr<Resource>& resource)
	: GameResource(resource)
{

}

auto Arg::Content::MaterialResource::GetResourceFileExtension() const -> const std::string&
{
	return MATERIAL_RESOURCE_FILE_EXTENSION;
}

void Arg::Content::MaterialResource::VPreLoad()
{
	m_pMaterial = std::make_shared<Renderer::Material>(GetResourceCache());
}

auto Arg::Content::MaterialResource::VGetSerializableData() const->ISerializable*
{
	return m_pMaterial.get();
}

void Arg::Content::MaterialResource::VPostLoad()
{
	m_pMaterial->Reserve();
}

void Arg::Content::MaterialResource::VUnload()
{
	m_pMaterial->Release();
	m_pMaterial = nullptr;
}
