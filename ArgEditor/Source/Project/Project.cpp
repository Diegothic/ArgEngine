#include <arg_pch.hpp>
#include "Project.hpp"

auto Arg::Editor::ProjectSettings::VOnSerialize(YAML::Node& node) const -> bool
{
	auto header = node["ArgProject"];

	header["Name"] = Name;
	header["EditorMap"] = EditorMap;

	return true;
}

auto Arg::Editor::ProjectSettings::VOnDeserialize(const YAML::Node& node) -> bool
{
	const auto header = node["ArgProject"];
	if (!header)
	{
		return false;
	}

	Name = ValueOr<std::string>(header["Name"], "EmptyProject");
	EditorMap = ValueOr<std::string>(header["EditorMap"], "");

	return true;
}

auto Arg::Editor::Project::Open(
	const std::filesystem::path& projectFile
) -> bool
{
	const bool isSuccess = m_Settings.Deserialize(projectFile);
	if (!isSuccess)
	{
		return false;
	}

	m_SettingsFile = projectFile;
	m_RootDirectory = m_SettingsFile.parent_path();
	m_Name = m_Settings.Name;
	m_EditorMap = m_Settings.EditorMap;

	m_pResourceCache = std::make_shared<Content::ResourceCache>();

	const std::filesystem::path contentPath = m_RootDirectory / "Content";
	const Content::ContentSpec contentSpec{
		.RootDirectory = contentPath.string(),
		.ResourceCache = m_pResourceCache,
	};
	m_pContent = std::make_unique<Content::Content>(contentSpec);
	m_pContent->Initialize();

	return true;
}

void Arg::Editor::Project::Close()
{
	m_pContent = nullptr;
	m_pResourceCache = nullptr;
}

void Arg::Editor::Project::Save() const
{
	const ProjectSettings settings = m_Settings;
	settings.Serialize(m_SettingsFile);

	m_pContent->Save();
}
