#include "Content.h"

#include <filesystem>
#include <yaml-cpp/yaml.h>

#include "Core/Analytics/DebugTimer.h"
#include "Import/TextureImporter.h"
#include "ResourceTypes/TextureResource.h"

Arg::Content::Content(const ContentSpec& spec)
	: m_RootDirectory(spec.RootDirectory)
{
}

void Arg::Content::Initialize()
{
	if (!std::filesystem::exists(m_RootDirectory))
	{
		std::filesystem::create_directory(m_RootDirectory);
	}

	{
		const TextureImporter importer("Content/Textures/awesomeface.png");
		const std::filesystem::path destination = m_RootDirectory / "Textures";
		importer.Import("awesomeface", m_RootDirectory, destination);
	}

	{
		const TextureImporter importer("Content/Textures/container.jpg");
		const std::filesystem::path destination = m_RootDirectory / "Textures";
		importer.Import("container", m_RootDirectory, destination);
	}

	Refresh();
}

void Arg::Content::Refresh()
{
	ScanDirectory(m_RootDirectory);
}

std::filesystem::path Arg::Content::GetResourcePath(GUID ID)
{
	return m_RootDirectory / m_ResourcePaths[ID];
}

void Arg::Content::ScanDirectory(const std::filesystem::path& path)
{
	if (!std::filesystem::exists(path)
		|| !std::filesystem::is_directory(path))
	{
		return;
	}

	for (const auto& entry : std::filesystem::directory_iterator(path))
	{
		if (std::filesystem::is_directory(entry.path()))
		{
			ScanDirectory(entry.path().string());
			continue;
		}

		if (entry.path().extension() == ".rsrc")
		{
			const YAML::Node resource = YAML::LoadFile(entry.path().string());
			if (const YAML::Node header = resource["resource"])
			{
				const auto id = header["id"].as<uint64_t>();
				const auto type = header["type"].as<std::string>();
				const auto file = header["file"].as<std::string>();
				std::filesystem::path dataPath = std::filesystem::relative(path / file, m_RootDirectory);
				const YAML::Node dataNode = resource["data"];

				if (type == "Texture")
				{
					CreateResource<TextureResource>(dataPath, dataNode, GUID(id));
				}
			}
		}
	}
}
