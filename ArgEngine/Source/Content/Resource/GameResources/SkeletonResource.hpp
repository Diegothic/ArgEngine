#pragma once

#include <arg_pch.hpp>

#include "Debug/Assert.hpp"
#include "GameDataResource.hpp"
#include "Renderer/Animation/Skeleton.hpp"

namespace Arg
{
	namespace Content
	{
		const std::string SKELETON_RESOURCE_FILE_EXTENSION = ".askeleton";

		class SkeletonResource : public GameDataResource
		{
		public:
			SkeletonResource(const std::shared_ptr<Resource>& resource);
			SkeletonResource(const SkeletonResource&) = delete;
			virtual ~SkeletonResource() = default;

			auto GetSkeleton() const -> const Renderer::Skeleton&
			{
				ARG_ASSERT(m_pSkeleton != nullptr);
				return *m_pSkeleton;
			}

		protected:
			auto GetResourceFileExtension() const -> const std::string& override;

			void VPreLoad() override;
			void VOnLoad() override;
			auto VGetSerializableData() const -> ISerializable* override;
			void VUnload() override;

		private:
			std::unique_ptr<Renderer::SkeletonSpec> m_pSkeletonSpec = nullptr;
			std::unique_ptr<Renderer::Skeleton> m_pSkeleton = nullptr;
		};
	}
}
