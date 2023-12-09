#pragma once

#include <arg_pch.hpp>

#include "Renderer/StaticModel.hpp"

namespace Arg
{
	namespace Import
	{
		class StaticModelImporter
		{
		public:
			StaticModelImporter() = default;
			~StaticModelImporter() = default;

			auto ImportFile(const std::string& file) -> bool;

			void Save(
				const std::string& fileName, 
				const std::filesystem::path& destination
			);

			auto GetData() const -> const Renderer::StaticModelData&;

		private:
			Renderer::StaticModelData m_Data;
		};
	}
}
