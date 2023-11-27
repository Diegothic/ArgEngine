#pragma once

#include <arg_pch.hpp>

#include "Core/GUID.hpp"
#include "Core/Math/Math.hpp"

namespace Arg
{
	namespace Content
	{
		enum ResourceType
		{
			ResourceTypeFolder = 0,
			ResourceTypeConfig,
			// Data resources
			ResourceTypeTexture,
			ResourceTypeStaticModel,
			ResourceTypeSkeletalModel,
			ResourceTypeSkeletalAnimation,
			ResourceTypeShader,
			ResourceTypeMaterial,
			// Gameplay resources
			ResourceTypePrefab,
			ResourceTypeMap,
			ResourceTypeScriptComponent,
			ResourceTypeScriptData,
			// For enumeration:
			RESOURCE_TYPE_MAX
		};

		class Resource
		{
		public:
			Resource() = default;
			Resource(const Resource&) = delete;
			~Resource() = default;

			void Create(
				const GUID ID,
				const std::string& name,
				const ResourceType type,
				const std::filesystem::path& path,
				const std::filesystem::path& rootDirectory
			);
			void Create(
				const std::filesystem::path& file,
				const std::filesystem::path& rootDirectory
			);
			void Remove();
			void Save() const;
			void Rename(const std::string& name);
			void Move(const std::filesystem::path& destination);

			auto operator==(const Resource& other) const -> bool;

			auto GetID() const -> const GUID& { return m_ID; }
			void SetID(const GUID ID) { m_ID = ID; }

			auto GetPathID() const -> const GUID& { return m_PathID; }
			auto GetPath() const -> const std::filesystem::path& { return m_Path; }
			void SetPath(
				const std::filesystem::path& path,
				const std::filesystem::path& rootDirectory
			);

			auto GetName() const -> const std::string& { return m_Name; }

			auto GetType() const -> const ResourceType& { return m_Type; }
			void SetType(const ResourceType& type) { m_Type = type; }

			auto GetColor() const -> const Vec3& { return m_Color; }
			void SetColor(const Vec3& color) { m_Color = color; }

		private:
			GUID m_ID = GUID::Empty;
			GUID m_PathID = GUID::Empty;

			std::filesystem::path m_Path;
			std::string m_Name;
			ResourceType m_Type = ResourceType::ResourceTypeFolder;
			Vec3 m_Color = Vec3(0.3f);
		};
	}
}
