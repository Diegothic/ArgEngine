#pragma once

#include <arg_pch.hpp>

#include "Renderer/SkeletalMesh.hpp"

namespace Arg
{
	namespace Import
	{
		class SkeletalMeshImporter
		{
		public:
			SkeletalMeshImporter() = default;
			~SkeletalMeshImporter() = default;

			auto ImportFile(const std::string& file) -> bool;

			auto GetData() const -> const Renderer::SkeletalMeshData&;

		private:
			Renderer::SkeletalMeshData m_Data = Renderer::SkeletalMeshData();
		};
	}
}
