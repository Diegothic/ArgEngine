#pragma once

#include <arg_pch.hpp>

#include "Content/Resource/Resource.hpp"
#include "Content/Resource/GameResources/GameDataResource.hpp"
#include "Content/Serialization/YamlSerializable.hpp"

namespace Arg
{
	namespace Content
	{
		const std::string SOUND_RESOURCE_FILE_EXTENSION = ".asnd";

		struct SoundData : public YamlSerializable
		{
		public:
			std::string Format;
			
		protected:
			auto VOnSerialize(YAML::Node& node) const -> bool override;
			auto VOnDeserialize(const YAML::Node& node) -> bool override;
		};
		
		class SoundResource : public GameDataResource
		{
		public:
			SoundResource(const std::shared_ptr<Resource>& resource);
			SoundResource(const SoundResource&) = delete;
			~SoundResource() = default;

		public:
			auto GetSoundData() -> char*;
			auto GetSoundDataSize() const ->size_t;
			void FreeSoundData();

			auto GetFormat() const -> const std::string&;

		protected:
			auto GetResourceFileExtension() const -> const std::string& override;

			void VPreLoad() override;
			void VOnLoad() override;
			auto VGetSerializableData() const -> ISerializable* override;
			void VUnload() override;

		private:
			std::unique_ptr<SoundData> m_pSoundData = nullptr;
		};
	}
}
