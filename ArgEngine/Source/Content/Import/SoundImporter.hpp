#pragma once

#include <arg_pch.hpp>

#include "IResourceImporter.hpp"
#include "Content/Resource/GameResources/SoundResource.hpp"

namespace Arg
{
	namespace Import
	{
		class SoundImporter : public IResourceImporter
		{
		public:
			SoundImporter() = default;

			virtual auto ImportFile(const std::string& file) -> bool override;
			virtual void Save(
				const std::string& fileName,
				const std::filesystem::path& destination
			) override;

		private:
			std::unique_ptr<std::vector<char>> m_pData = nullptr;
			Content::SoundData m_SoundData;
		};
	}
}
