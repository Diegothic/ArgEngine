#pragma once

#include <arg_pch.hpp>

#include "Resource.hpp"
#include "Content/Serialization/ISerializable.hpp"

namespace Arg
{
	namespace Content
	{
		class ResourceCache;

		class GameResource
		{
		public:
			GameResource(const std::shared_ptr<Resource>& resource);
			GameResource(const GameResource&) = delete;

			void AddRef();
			void FreeRef();

			auto GetName() const -> const std::string&;

		public:
			virtual void VRemoveFiles();
			virtual auto VIsSaveable() const -> bool { return true; }
			virtual void VSaveFiles() const;
			virtual void VRenameFiles(const std::string& name);
			virtual void VMoveFiles(const std::filesystem::path& destination);

		protected:
			virtual auto GetResourceFileExtension() const -> const std::string& = 0;
			auto GetResource() const -> const std::shared_ptr<Resource>& { return m_pResource; }
			auto GetResourceFilePath() const -> std::filesystem::path;
			auto GetResourceCache() -> ResourceCache*;

			void Load();

			virtual void VPreLoad()
			{
			}

			virtual void VOnLoad()
			{
			}

			virtual void VPostLoad()
			{
			}

			virtual auto VGetSerializableData() const -> ISerializable* = 0;
			virtual void VUnload() = 0;

		private:
			std::shared_ptr<Resource> m_pResource = nullptr;
			uint32_t m_RefCount = 0;
		};
	}
}
