#pragma once

#include <arg_pch.hpp>

#include "IResourceImporter.hpp"
#include "Renderer/Animation/SkeletalAnimation.hpp"

namespace Arg
{
	namespace Import
	{
		class SkeletalAnimationImporter : public IResourceImporter
		{
		public:
			SkeletalAnimationImporter() = default;
			~SkeletalAnimationImporter() = default;

			void ScanFile(const std::string& file);
			auto GetAnimationCount() const -> size_t { return m_AnimationCount; }
			void SetIndex(size_t index);
			auto GetAnimationName(size_t index) -> const std::string&;

			virtual auto ImportFile(const std::string& file) -> bool override;
			virtual void Save(
				const std::string& fileName,
				const std::filesystem::path& destination
			) override;

			auto GetData() const -> const Renderer::SkeletalAnimationData&;

		private:
			Renderer::SkeletalAnimationData m_Data = Renderer::SkeletalAnimationData();
			size_t m_AnimationCount = 0;
			int32_t m_AnimationsMeshIndex = -1;
			size_t m_CurrentIndex = 0;
			std::vector<std::string> m_AnimationNames;
		};
	}
}
