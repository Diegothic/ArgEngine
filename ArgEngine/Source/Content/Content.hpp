#pragma once

#include <arg_pch.hpp>

#include "ResourceCache.hpp"
#include "Resource/ResourceFolder.hpp"
#include "Core/Random/Random.hpp"
#include "Serialization/YamlSerializable.hpp"

namespace Arg
{
	namespace Content
	{
		struct ContentConfig : public YamlSerializable
		{
			uint64_t LastGeneratedID = 0;

		protected:
			auto VOnSerialize(YAML::Node& node) const -> bool override;
			auto VOnDeserialize(const YAML::Node& node) -> bool override;
		};

		struct ContentSpec
		{
			std::string RootDirectory;
			std::shared_ptr<ResourceCache> ResourceCache;
			bool bAutoSaveConfig = true;
		};

		class Content
		{
		public:
			Content(const ContentSpec& spec);
			~Content() = default;

			void Initialize();
			auto GetRootFolder() const -> const std::shared_ptr<ResourceFolder>&;

			void CreateFolder(const std::shared_ptr<ResourceFolder>& parent);
			void RemoveFolder(const std::shared_ptr<ResourceFolder>& folder);
			void RenameFolder(
				const std::shared_ptr<ResourceFolder>& folder,
				const std::string& name
			);

			auto CreateResource(
				const std::string& name,
				const ResourceType type,
				const std::shared_ptr<ResourceFolder>& parent
			) -> std::shared_ptr<Resource>&;
			void RemoveResource(
				const std::shared_ptr<Resource>& resource,
				const std::shared_ptr<ResourceFolder>& parentFolder
			);
			void MoveResource(
				const std::shared_ptr<Resource>& resource,
				const std::shared_ptr<ResourceFolder>& source,
				const std::shared_ptr<ResourceFolder>& destination
			);

			void Save() const;

		private:
			void ScanDirectory(
				const std::filesystem::path& directory,
				const std::shared_ptr<ResourceFolder>& folder
			);

			void UpdatePaths(const std::shared_ptr<ResourceFolder>& folder);

			auto GenerateID() -> GUID;

			void LoadConfig();
			void SaveConfig() const;

		private:
			std::filesystem::path m_RootDirectory;
			std::shared_ptr<ResourceCache> m_pResourceCache;
			std::vector<std::shared_ptr<ResourceFolder>> m_pFolders;

			ContentConfig m_Config;
			std::filesystem::path m_ConfigFile;
			bool m_bAutoSaveConfig = true;
			Random::Random m_IDGenerator;
		};
	}
}