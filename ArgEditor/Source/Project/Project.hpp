#pragma once

#include <arg_pch.hpp>

#include "Content/Content.hpp"
#include "Content/Serialization/YamlSerializable.hpp"

namespace Arg
{
	namespace Editor
	{
		const std::string PROJECT_FILE_EXTENSION = ".aproj";

		struct ProjectSettings : public Content::YamlSerializable
		{
			std::string Name = "EmptyProject";
			std::string EditorMap;
			std::string GameMap;

		protected:
			auto VOnSerialize(YAML::Node& node) const -> bool override;
			auto VOnDeserialize(const YAML::Node& node) -> bool override;
		};

		class Project
		{
		public:
			auto GetName() const -> const std::string& { return m_Name; }
			void SetName(const std::string& name) { m_Name = name; }

			auto GetEditorMap() const -> const std::string& { return m_EditorMap; }
			auto SetEditorMap(const std::string& mapPath) { m_EditorMap = mapPath; }

			auto GetGameMap() const -> const std::string& { return m_GameMap; }
			auto SetGameMap(const std::string& mapPath) { m_GameMap = mapPath; }

		public:
			auto Open(
				const std::filesystem::path& projectFile
			) -> bool;
			void Close();
			void Save() const;

			auto GetRootDirectory() const -> const std::filesystem::path& { return m_RootDirectory; }
			auto GetResourceCache() -> std::shared_ptr<Content::ResourceCache>& { return m_pResourceCache; }
			auto GetContent() -> std::unique_ptr<Content::Content>& { return m_pContent; }

		private:
			ProjectSettings m_Settings;

			std::filesystem::path m_RootDirectory;
			std::filesystem::path m_SettingsFile;
			std::string m_Name;
			std::string m_EditorMap;
			std::string m_GameMap;

			std::shared_ptr<Content::ResourceCache> m_pResourceCache = nullptr;
			std::unique_ptr<Content::Content> m_pContent = nullptr;
		};
	}
}
