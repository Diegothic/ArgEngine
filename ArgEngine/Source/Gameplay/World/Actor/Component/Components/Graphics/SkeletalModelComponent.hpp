#pragma once

#include <arg_pch.hpp>

#include "Gameplay/World/Actor/Component/ActorComponent.hpp"
#include "Content/ResourceCache.hpp"
#include "Content/Resource/GameResources/SkeletalModelResource.hpp"
#include "Content/Resource/GameResources/MaterialResource.hpp"
#include "Content/Resource/GameResources/SkeletalAnimationResource.hpp"
#include "Content/Resource/GameResources/SkeletonResource.hpp"

namespace Arg
{
	using MaterialHandle = Content::ResourceHandle<Content::MaterialResource>;
	using SkeletalModelHandle = Content::ResourceHandle<Content::SkeletalModelResource>;
	using SkeletonHandle = Content::ResourceHandle<Content::SkeletonResource>;
	using SkeletalAnimationHandle = Content::ResourceHandle<Content::SkeletalAnimationResource>;

	namespace Gameplay
	{
		class SkeletalModelComponent : public ActorComponent
		{
		public:
			static const GUID COMPONENT_ID;
			static const std::string COMPONENT_NAME;

		public:
			SkeletalModelComponent();

			auto VCreateDefault() -> std::shared_ptr<ActorComponent> override;

			auto VGetID() const -> GUID override { return COMPONENT_ID; }
			auto VGetName() const -> const std::string& override { return COMPONENT_NAME; }

			void VBeginPlay() override;
			void VTick(const GameTime& gameTime) override;
			void VRender(Renderer::RenderContext& context) override;

			void VOnComponentAdded() override;
			void VOnComponentRemoved() override;

			auto GetSkeleton() const -> SkeletonHandle { return m_Skeleton; }
			void SetSkeleton(const SkeletonHandle& skeleton);

			auto GetModel() const -> SkeletalModelHandle { return m_Model; }
			void SetModel(const SkeletalModelHandle& model);

			auto GetMaterialCount() const -> size_t { return m_Materials.size(); }
			auto GetMaterial(size_t index) const -> const MaterialHandle&;
			void SetMaterial(size_t index, const MaterialHandle& material);

			auto GetReceiveShadows() const -> bool { return m_bReceiveShadows; }
			void SetReceiveShadows(bool bReceiveShadows);

			auto GetCastShadows() const -> bool { return m_bCastShadows; }
			void SetCastShadows(bool bCastShadows);

			auto GetCurrentAnimation() const -> SkeletalAnimationHandle { return m_CurrentAnimation; }
			void SetCurrentAnimation(const SkeletalAnimationHandle& animation);

			auto GetPlayOnStart() const -> bool { return m_bPlayOnStart; }
			void SetPlayOnStart(bool bPlayOnStart) {m_bPlayOnStart = bPlayOnStart;}

			auto GetLooping() const -> bool { return m_bLooping; }
			void SetLooping(bool bLooping) {m_bLooping = bLooping;}

		public:
			void Play(const SkeletalAnimationHandle& animation);
			void Stop();

		protected:
			auto VOnSerialize(YAML::Node& node) const -> bool override;
			auto VOnDeserialize(const YAML::Node& node) -> bool override;

		private:
			SkeletonHandle m_Skeleton;
			SkeletalModelHandle m_Model;
			std::vector<MaterialHandle> m_Materials;

			Renderer::SkeletonPose m_Pose;
			std::vector<Mat4> m_AnimationBoneTransforms;

			bool m_bReceiveShadows = true;
			bool m_bCastShadows = true;
			
			SkeletalAnimationHandle m_CurrentAnimation;
			bool m_bPlayOnStart = true;
			bool m_bLooping = true;

			bool m_bIsPlaying = false;
			float m_ElapsedTime = 0.0f;
		};
	}
}
