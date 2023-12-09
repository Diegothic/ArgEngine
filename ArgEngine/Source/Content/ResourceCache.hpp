#pragma once

#include <arg_pch.hpp>

#include "Debug/Assert.hpp"
#include "Resource/Resource.hpp"
#include "Resource/GameResource.hpp"

namespace Arg
{
	namespace Content
	{
		class ResourceCache;

		template <typename TResourceType>
		class ResourceHandle
		{
		public:
			ResourceHandle() = default;
			ResourceHandle(const GUID ID, ResourceCache* resourceCache);
			ResourceHandle(const ResourceHandle<TResourceType>& other);
			~ResourceHandle();

			auto operator=(const ResourceHandle<TResourceType>& other)->ResourceHandle<TResourceType>&;
			auto operator==(const ResourceHandle<TResourceType>& other) -> bool;

			auto GetID() const -> const GUID& { return m_ResourceID; }
			auto IsValid() const -> bool;
			auto Get() const->std::shared_ptr<TResourceType>;

			void AddRef();
			void FreeRef();

		private:
			GUID m_ResourceID = GUID::Empty;
			ResourceCache* m_pResourceCache = nullptr;
		};

		class ResourceCache
		{
		public:
			ResourceCache() = default;
			ResourceCache(const ResourceCache&) = delete;
			~ResourceCache() = default;

			template<typename TResourceType>
			auto CreateHandle(const GUID ID) -> ResourceHandle<TResourceType>;

			template<typename TResourceType>
			auto CreateHandle(const std::string& path) -> ResourceHandle<TResourceType>;

			void AddResource(const std::shared_ptr<Resource>& resource);
			void RemoveResource(const std::shared_ptr<Resource>& resource);
			void RemoveResource(const GUID ID);

			void SaveResource(const GUID ID) const;
			void RenameResource(
				const GUID ID,
				const std::string& name
			);
			void MoveResource(
				const GUID ID,
				const std::filesystem::path& destination
			);

			auto IsValid(const GUID ID) -> bool;

			auto GetResource(const GUID ID) -> std::shared_ptr<Resource>&;

			auto GetGameResource(const GUID ID) -> std::shared_ptr<GameResource>&;
			auto GetGameResource(const std::string& path) -> std::shared_ptr<GameResource>&;

			void FreeResource(const GUID ID);

			void SaveAll() const;

		private:
			std::unordered_map<GUID, std::shared_ptr<Resource>> m_pResources;
			std::unordered_map<GUID, std::shared_ptr<Resource>> m_pResourcesByPathID;

			std::unordered_map<GUID, std::shared_ptr<GameResource>> m_pGameResources;
		};

		template <typename TResourceType>
		Arg::Content::ResourceHandle<TResourceType>::ResourceHandle(const GUID ID, ResourceCache* resourceCache)
		{
			m_ResourceID = ID;
			m_pResourceCache = resourceCache;
		}

		template <typename TResourceType>
		Arg::Content::ResourceHandle<TResourceType>::ResourceHandle(const ResourceHandle<TResourceType>& other)
		{
			m_ResourceID = other.m_ResourceID;
			m_pResourceCache = other.m_pResourceCache;
		}

		template <typename TResourceType>
		Arg::Content::ResourceHandle<TResourceType>::~ResourceHandle()
		{

		}

		template <typename TResourceType>
		auto Arg::Content::ResourceHandle<TResourceType>::operator=(
			const ResourceHandle<TResourceType>& other
			) ->ResourceHandle<TResourceType>&
		{
			m_ResourceID = other.m_ResourceID;
			m_pResourceCache = other.m_pResourceCache;
			return *this;
		}

		template <typename TResourceType>
		auto Arg::Content::ResourceHandle<TResourceType>::operator==(
			const ResourceHandle<TResourceType>& other
			) -> bool
		{
			return m_ResourceID == other.m_ResourceID;
		}

		template <typename TResourceType>
		auto Arg::Content::ResourceHandle<TResourceType>::IsValid() const -> bool
		{
			return m_ResourceID != GUID::Empty && m_pResourceCache->IsValid(m_ResourceID);
		}

		template <typename TResourceType>
		auto Arg::Content::ResourceHandle<TResourceType>::Get() const -> std::shared_ptr<TResourceType>
		{
			return dynamic_pointer_cast<TResourceType>(m_pResourceCache->GetGameResource(m_ResourceID));
		}

		template <typename TResourceType>
		void Arg::Content::ResourceHandle<TResourceType>::AddRef()
		{
			m_pResourceCache->GetGameResource(m_ResourceID)->AddRef();
		}

		template <typename TResourceType>
		void Arg::Content::ResourceHandle<TResourceType>::FreeRef()
		{
			m_pResourceCache->GetGameResource(m_ResourceID)->FreeRef();
		}

		template<typename TResourceType>
		auto Arg::Content::ResourceCache::CreateHandle(const GUID ID) -> ResourceHandle<TResourceType>
		{
			ResourceHandle<TResourceType> handle(ID, this);
			return handle;
		}

		template<typename TResourceType>
		auto Arg::Content::ResourceCache::CreateHandle(const std::string& path) ->ResourceHandle<TResourceType>
		{
			const GUID pathID = std::hash<std::string>{}(path);
			const GUID ID = m_pResourcesByPathID.contains(pathID)
				? m_pResourcesByPathID.at(pathID)->GetID()
				: GUID::Empty;
			ResourceHandle<TResourceType> handle(ID, this);
			return handle;
		}
	}
}
