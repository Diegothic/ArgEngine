#pragma once

#include <arg_pch.hpp>

#include "Debug/Assert.hpp"
#include "Resource/Resource.hpp"
#include "Resource/ResourceTypes/TextureResource.hpp"

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

			auto IsValid() const -> bool;
			auto Get() const->std::shared_ptr<TResourceType>&;

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

			void SaveResource(const GUID ID);
			void RenameResource(
				const GUID ID,
				const std::string& name
			);
			void MoveResource(
				const GUID ID,
				const std::filesystem::path& destination
			);

			auto GetResource(const GUID ID) -> std::shared_ptr<Resource>&;
			template<typename TResourceType>
			auto GetResource(const GUID ID) -> std::shared_ptr<TResourceType>&;
			template<typename TResourceType>
			auto GetResource(const std::string& path) -> std::shared_ptr<TResourceType>&;

			void FreeResource(const GUID ID);

			auto IsValid(const GUID ID) -> bool;

		private:
			auto GetResourceByType(const GUID ID) -> std::shared_ptr<TextureResource>&;

		private:
			std::unordered_map<GUID, std::shared_ptr<Resource>> m_pResources;
			std::unordered_map<GUID, std::shared_ptr<Resource>> m_pResourcesByPathID;

			std::unordered_map<GUID, std::shared_ptr<TextureResource>> m_pLoadedTextures;
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
		auto Arg::Content::ResourceHandle<TResourceType>::operator=(const ResourceHandle<TResourceType>& other) ->ResourceHandle<TResourceType>&
		{
			m_ResourceID = other.m_ResourceID;
			m_pResourceCache = other.m_pResourceCache;
			return *this;
		}

		template <typename TResourceType>
		auto Arg::Content::ResourceHandle<TResourceType>::operator==(const ResourceHandle<TResourceType>& other) -> bool
		{
			return m_ResourceID == other.m_ResourceID;
		}

		template <typename TResourceType>
		auto Arg::Content::ResourceHandle<TResourceType>::IsValid() const -> bool
		{
			return m_ResourceID != GUID::Empty && m_pResourceCache->IsValid(m_ResourceID);
		}

		template <typename TResourceType>
		auto Arg::Content::ResourceHandle<TResourceType>::Get() const -> std::shared_ptr<TResourceType>&
		{
			return m_pResourceCache->GetResource<TResourceType>(m_ResourceID);
		}

		template<typename TResourceType>
		auto Arg::Content::ResourceCache::GetResource(const std::string& path) -> std::shared_ptr<TResourceType>&
		{
			const GUID pathID = std::hash<std::string>{}(path);
			ARG_ASSERT(m_pResourcesByPathID.contains(pathID), "Invalid resource Path!");
			const GUID ID = m_pResourcesByPathID.at(pathID)->GetID();
			return GetResourceByType(ID);
		}

		template<typename TResourceType>
		auto Arg::Content::ResourceCache::GetResource(const GUID ID) -> std::shared_ptr<TResourceType>&
		{
			ARG_ASSERT(m_pResources.contains(ID), "Invalid resource ID!");
			return GetResourceByType(ID);
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
