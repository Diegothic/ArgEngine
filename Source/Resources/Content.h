#pragma once
#include <filesystem>
#include <string>
#include <unordered_map>

#include "Core/Memory/SmartPtr.h"
#include "Core/GUID.h"
#include "Resource.h"

namespace Arg
{
	struct ContentSpec
	{
		std::filesystem::path RootDirectory;
	};

	class Content
	{
	public:
		Content() = default;
		Content(const ContentSpec& spec);

		void Initialize();
		void Refresh();

		const std::filesystem::path& GetRootDirectory() const { return m_RootDirectory; }

		template <typename ResourceType>
		ResourceType* GetResource(GUID ID);
		template <typename ResourceType>
		ResourceType* GetResource(const std::filesystem::path& path);
		std::filesystem::path GetResourcePath(GUID ID);

	protected:
		template <typename ResourceType>
		void CreateResource(const std::filesystem::path& path, const YAML::Node& dataNode, GUID ID);
		void ScanDirectory(const std::filesystem::path& path);

	private:
		std::filesystem::path m_RootDirectory;
		std::vector<Rc<Resource>> m_Resources;
		std::unordered_map<GUID, Resource*> m_ResourceRegistry;
		std::unordered_map<GUID, std::filesystem::path> m_ResourcePaths;
	};

	template <typename ResourceType>
	ResourceType* Content::GetResource(GUID ID)
	{
		ResourceType* resource = dynamic_cast<ResourceType*>(m_ResourceRegistry[ID]);
		return resource;
	}

	template <typename ResourceType>
	ResourceType* Content::GetResource(const std::filesystem::path& path)
	{
		return GetResource<ResourceType>(std::hash<std::string>{}(path.string()));
	}

	template <typename ResourceType>
	void Content::CreateResource(const std::filesystem::path& path, const YAML::Node& dataNode, GUID ID)
	{
		const Rc<Resource> newResource = NewRc<ResourceType>(this,
			ID,
			path.filename().replace_extension().string(),
			dataNode
		);
		m_Resources.push_back(newResource);
		m_ResourceRegistry[ID] = newResource.get();
		m_ResourcePaths[ID] = path;
	}
}
