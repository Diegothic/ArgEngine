#pragma once

#include <arg_pch.hpp>

#include "Renderer/Animation/Skeleton.hpp"

namespace Arg
{
	namespace Import
	{
		class SkeletonImporter
		{
		public:
			SkeletonImporter() = default;
			~SkeletonImporter() = default;

			auto ImportFile(const std::string& file) -> bool;

			auto GetData() const -> const Renderer::SkeletonSpec&;

		private:
			Renderer::SkeletonSpec m_Data = Renderer::SkeletonSpec();
		};
	}
}
