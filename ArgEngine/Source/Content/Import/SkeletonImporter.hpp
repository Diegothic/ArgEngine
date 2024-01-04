#pragma once

#include <arg_pch.hpp>

#include "IResourceImporter.hpp"
#include "Renderer/Animation/Skeleton.hpp"

namespace Arg
{
	namespace Import
	{
		class SkeletonImporter : public IResourceImporter
		{
		public:
			SkeletonImporter() = default;

			virtual auto ImportFile(const std::string& file) -> bool override;
			virtual void Save(
				const std::string& fileName,
				const std::filesystem::path& destination
			) override;

			auto GetData() const -> const Renderer::SkeletonData&;

		private:
			Renderer::SkeletonData m_Data;
		};
	}
}
