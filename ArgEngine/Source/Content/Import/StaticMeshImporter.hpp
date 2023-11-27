#pragma once

#include <arg_pch.hpp>

#include "Renderer/StaticMesh.hpp"

namespace Arg
{
	namespace Import
	{
		class StaticMeshImporter
		{
		public:
			StaticMeshImporter() = default;
			~StaticMeshImporter() = default;

			auto ImportFile(const std::string& file) -> bool;

			auto GetData() const -> const Renderer::StaticMeshData&;

		private:
			Renderer::StaticMeshData m_Data = Renderer::StaticMeshData();
		};
	}
}
