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
			std::string EditorMap = "";

		protected:
			auto VOnSerialize(YAML::Node& node) const -> bool override;
			auto VOnDeserialize(const YAML::Node& node) -> bool override;
		};

		class Project
		{
		public:
			auto GetName() const -> const std::string& { return m_Name; }
			auto GetEditorMap() const -> const std::string& { return m_EditorMap; }

			auto Open(
				const std::filesystem::path& projectFile
			) -> bool;
			void Close();
			void Save() const;

			auto GetResourceCache() -> std::shared_ptr<Content::ResourceCache>& { return m_pResourceCache; }
			auto GetContent() -> std::unique_ptr<Content::Content>& { return m_pContent; }

		private:
			ProjectSettings m_Settings;

			std::filesystem::path m_RootDirectory;
			std::filesystem::path m_SettingsFile;
			std::string m_Name;
			std::string m_EditorMap;

			std::shared_ptr<Content::ResourceCache> m_pResourceCache = nullptr;
			std::unique_ptr<Content::Content> m_pContent = nullptr;
		};
	}
}
