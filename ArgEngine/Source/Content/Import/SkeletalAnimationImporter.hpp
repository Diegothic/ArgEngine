#pragma once

#include <arg_pch.hpp>

#include "Renderer/Animation/SkeletalAnimation.hpp"

namespace Arg
{
	namespace Import
	{
		class SkeletalAnimationImporter
		{
		public:
			SkeletalAnimationImporter() = default;
			~SkeletalAnimationImporter() = default;

			auto ImportFile(const std::string& file) -> bool;

			auto GetData() const -> const Renderer::SkeletalAnimationData&;

		private:
			Renderer::SkeletalAnimationData m_Data = Renderer::SkeletalAnimationData();
		};
	}
}
