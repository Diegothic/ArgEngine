#pragma once

#include <arg_pch.hpp>

#include "Resource.hpp"
#include "Core/Math/Math.hpp"

namespace Arg
{
	namespace Content
	{
		class ResourceFolder
		{
		public:
			ResourceFolder(const std::shared_ptr<Resource>& resource);
			ResourceFolder(const ResourceFolder&) = delete;
			~ResourceFolder() = default;

			auto operator==(const ResourceFolder& other) const -> bool;

			void ChangeColor(const Vec3& color);

			auto GetID() const -> const GUID&;
			auto GetPath() const-> const std::filesystem::path&;
			auto GetFullPath() const->std::filesystem::path;
			auto GetName() const ->const std::string&;
			auto GetColor() const-> const Vec3&;

			auto GetParentFolder() const -> const std::shared_ptr<ResourceFolder>& { return m_pParentFolder; }
			void SetParentFolder(const std::shared_ptr<ResourceFolder>& parentFolder);

			auto GetSubfolderCount() const -> size_t { return m_pSubfolders.size(); }
			auto GetSubfolder(const size_t index) const -> const std::shared_ptr<ResourceFolder>&;
			void AddSubfolder(const std::shared_ptr<ResourceFolder>& subfolder);
			void RemoveSubfolder(const std::shared_ptr<ResourceFolder>& subfolder);
			void SortSubfolders();

			auto GetResourceCount() const -> size_t { return m_pResources.size(); }
			auto GetResource(const size_t index) const -> const std::shared_ptr<Resource>&;
			void AddResource(const std::shared_ptr<Resource>& resource);
			void RemoveResource(const std::shared_ptr<Resource>& resource);

		private:
			std::shared_ptr<Resource> m_pResource = nullptr;
			std::shared_ptr<ResourceFolder> m_pParentFolder = nullptr;
			std::vector<std::shared_ptr<ResourceFolder>> m_pSubfolders;

			std::vector<std::shared_ptr<Resource>> m_pResources;
		};
	}
}
