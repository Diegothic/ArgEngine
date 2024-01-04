#pragma once

#include <arg_pch.hpp>

#include "IResourceImporter.hpp"
#include "Renderer/SkeletalModel.hpp"

namespace Arg
{
	namespace Import
	{
		class SkeletalModelImporter : public IResourceImporter
		{
		public:
			SkeletalModelImporter() = default;

			virtual auto ImportFile(const std::string& file) -> bool override;
			virtual void Save(
				const std::string& fileName,
				const std::filesystem::path& destination
			) override;

			auto GetData() const -> const Renderer::SkeletalModelData&;

		private:
			Renderer::SkeletalModelData m_Data;
		};
	}
}
