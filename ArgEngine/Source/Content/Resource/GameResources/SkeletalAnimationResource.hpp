#pragma once

#include <arg_pch.hpp>

#include "Debug/Assert.hpp"
#include "GameDataResource.hpp"
#include "Renderer/Animation/SkeletalAnimation.hpp"

namespace Arg
{
	namespace Content
	{
		const std::string SKELETAL_ANIMATION_RESOURCE_FILE_EXTENSION = ".askanim";

		class SkeletalAnimationResource : public GameDataResource
		{
		public:
			SkeletalAnimationResource(const std::shared_ptr<Resource>& resource);
			SkeletalAnimationResource(const SkeletalAnimationResource&) = delete;
			virtual ~SkeletalAnimationResource() = default;

			auto GetAnimation() const -> const Renderer::SkeletalAnimation&
			{
				ARG_ASSERT(m_pAnimation != nullptr, "")
				return *m_pAnimation;
			}

		protected:
			auto GetResourceFileExtension() const -> const std::string& override;

			void VPreLoad() override;
			void VOnLoad() override;
			auto VGetSerializableData() const -> ISerializable* override;
			void VUnload() override;

		private:
			std::unique_ptr<Renderer::SkeletalAnimationSpec> m_pAnimationSpec = nullptr;
			std::unique_ptr<Renderer::SkeletalAnimation> m_pAnimation = nullptr;
		};
	}
}
