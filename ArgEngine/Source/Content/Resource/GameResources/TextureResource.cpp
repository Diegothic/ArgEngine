#include <arg_pch.hpp>
#include "TextureResource.hpp"

#include "Content/Yaml/Yaml.hpp"

Arg::Content::TextureResource::TextureResource(const std::shared_ptr<Resource>& resource)
	: GameDataResource(resource)
{

}

auto Arg::Content::TextureResource::GetTextureData() -> char*
{
	GameDataResource::VPreLoad();
	return GetData()->data();
}

void Arg::Content::TextureResource::FreeTextureData()
{
	GameDataResource::VPostLoad();
}

auto Arg::Content::TextureResource::GetResourceFileExtension() const -> const std::string&
{
	return TEXTURE_RESOURCE_FILE_EXTENSION;
}

void Arg::Content::TextureResource::VPreLoad()
{
	GameDataResource::VPreLoad();
	m_pTexture = std::make_shared<Renderer::Texture>();
}

void Arg::Content::TextureResource::VOnLoad()
{
	GameDataResource::VOnLoad();
	const Renderer::TextureData textureData = {
		.Data = GetData()->data(),
		.Width = m_pTexture->GetWidth(),
		.Height = m_pTexture->GetHeight(),
		.Format = m_pTexture->GetFormat()
	};
	m_pTexture->SetData(textureData);
}

auto Arg::Content::TextureResource::VGetSerializableData() const->ISerializable*
{
	return m_pTexture.get();
}

void Arg::Content::TextureResource::VUnload()
{
	m_pTexture = nullptr;
}
