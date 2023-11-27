#include <arg_pch.hpp>
#include "Resource.hpp"

#include "Content/Yaml/Yaml.hpp"

auto Arg::Content::Resource::operator==(const Resource& other) const -> bool
{
	return m_ID == other.m_ID;
}

void Arg::Content::Resource::Create(
	const std::filesystem::path& file,
	const std::filesystem::path& rootDirectory
)
{
	const auto resourceData = YAML::LoadFile(file.string());
	auto header = resourceData["Resource"];

	const auto relativePath = std::filesystem::relative(
		file,
		rootDirectory
	).replace_extension("");
	const auto fileName = relativePath.filename().string();

	if (header["ID"])
	{
		m_ID = header["ID"].as<GUID>();
	}

	m_PathID = GUID(std::hash<std::string>{}(relativePath.string()));
	m_Path = file.parent_path();
	m_Name = fileName;
	m_Type = static_cast<ResourceType>(header["Type"].as<uint32_t>());
	if (header["Color"])
	{
		m_Color = header["Color"].as<Vec3>();
	}
}

void Arg::Content::Resource::Remove()
{
	const auto filePath = (m_Path / m_Name).replace_extension("meta");
	std::filesystem::remove(filePath);
}

void Arg::Content::Resource::Create(
	const GUID ID,
	const std::string& name,
	const ResourceType type,
	const std::filesystem::path& path,
	const std::filesystem::path& rootDirectory
)
{
	const auto relativePath = std::filesystem::relative(
		path / name,
		rootDirectory
	).replace_extension("");
	m_ID = ID;
	m_PathID = GUID(std::hash<std::string>{}(relativePath.string()));
	m_Path = path;
	m_Name = name;
	m_Type = type;
}

void Arg::Content::Resource::Save() const
{
	const auto filePath = (m_Path / m_Name).replace_extension("meta");
	YAML::Node resourceData;
	if (std::filesystem::exists(filePath))
	{
		resourceData = YAML::LoadFile(filePath.string());
	}

	auto header = resourceData["Resource"];
	header["ID"] = m_ID;
	header["Type"] = static_cast<uint32_t>(m_Type);
	header["Color"] = m_Color;

	std::ofstream file(filePath, std::ofstream::out | std::ofstream::trunc);
	file << resourceData;
	file.close();
}

void Arg::Content::Resource::Rename(const std::string& name)
{
	std::filesystem::rename(
		(GetPath() / GetName()).replace_extension(".meta"),
		(GetPath() / name).replace_extension(".meta")
	);
	m_Name = name;
}

void Arg::Content::Resource::Move(const std::filesystem::path& destination)
{
	const auto filePath = (m_Path / m_Name).replace_extension("meta");
	std::filesystem::copy_file(filePath, (destination / m_Name).replace_extension("meta"));
	std::filesystem::remove(filePath);
}

void Arg::Content::Resource::SetPath(
	const std::filesystem::path& path,
	const std::filesystem::path& rootDirectory
)
{
	m_Path = path;
	const auto relativePath = std::filesystem::relative(
		path / m_Name,
		rootDirectory
	).replace_extension("");
	m_PathID = GUID(std::hash<std::string>{}(relativePath.string()));
}
