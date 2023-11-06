#include "TextureResource.h"

Arg::TextureResource::TextureResource(Content* content, GUID ID, std::string name, const YAML::Node& dataNode)
	:Resource(content, ID, std::move(name), dataNode)
{
	const auto width = dataNode["width"].as<uint32_t>();
	const auto height = dataNode["height"].as<uint32_t>();
	const auto formatN = dataNode["format"].as<uint32_t>();

	TextureFormat format = TextureFormat::RGB;
	switch (formatN)
	{
	case 3:
		format = TextureFormat::RGB;
		break;
	case 4:
		format = TextureFormat::RGBA;
		break;
	default:
		break;
	}

	m_Spec = TextureSpec
	{
		.Format = format,
		.Width = width,
		.Height = height,
	};
}

void Arg::TextureResource::VOnLoad(const char* data)
{
	m_Texture = NewBox<Texture>(m_Spec);
	m_Texture->SetData(data);
}

void Arg::TextureResource::VOnUnload()
{
	m_Texture = nullptr;
}
