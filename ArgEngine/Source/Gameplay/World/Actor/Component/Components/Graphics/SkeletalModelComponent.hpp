#pragma once

#include <arg_pch.hpp>

#include "Gameplay/World/Actor/Component/ActorComponent.hpp"
#include "Content/ResourceCache.hpp"
#include "Content/Resource/GameResources/SkeletalModelResource.hpp"
#include "Content/Resource/GameResources/MaterialResource.hpp"
#include "Content/Resource/GameResources/SkeletalAnimationResource.hpp"
#include "Content/Resource/GameResources/SkeletonResource.hpp"
#include "Core/Event/Event.hpp"
#include "Gameplay/World/Actor/ActorHandle.hpp"

namespace Arg
{
	using MaterialHandle = Content::ResourceHandle<Content::MaterialResource>;
	using SkeletalModelHandle = Content::ResourceHandle<Content::SkeletalModelResource>;
	using SkeletonHandle = Content::ResourceHandle<Content::SkeletonResource>;
	using SkeletalAnimationHandle = Content::ResourceHandle<Content::SkeletalAnimationResource>;

	namespace Gameplay
	{
		struct SkeletonAttachment
		{
			int32_t ChildActorIndex;
			int32_t BoneIndex;
		};

		enum SkeletalAnimationState
		{
			StateIdle,
			StateStarted,
			StatePlaying,
			StatePaused,
			StateFinished
		};

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
			auto VGetDisplayName() const -> const std::string& override;

			void VBeginPlay() override;
			void VEndPlay() override;
			void VTick(const GameTime& gameTime, const GameInput& gameInput) override;
			void VRender(Renderer::RenderContext& context) override;

			void VOnComponentAdded() override;
			void VOnComponentRemoved() override;

			void VClone(const ActorComponent* pActorComponent) override;

		public:
			Event<void()> Ev_OnAnimationStart;
			Event<void()> Ev_OnAnimationEnd;
			Event<void(const std::string&)> Ev_OnAnimationEvent;

		public:
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
			void SetPlayOnStart(bool bPlayOnStart) { m_bPlayOnStart = bPlayOnStart; }

			auto GetLooping() const -> bool { return m_bLooping; }
			void SetLooping(bool bLooping) { m_bLooping = bLooping; }

			auto GetAttachmentCount() const -> size_t { return m_Attachments.size(); }
			auto GetAttachment(size_t index) const -> const SkeletonAttachment&;
			void SetAttachment(size_t index, const SkeletonAttachment& attachment);
			void AddAttachment(const SkeletonAttachment& attachment);
			void RemoveAttachment(size_t index);

		public:
			void Play(const SkeletalAnimationHandle& animation);
			void Pause();
			void Unpause();
			void Stop();

		protected:
			auto VOnSerialize(YAML::Node& node) const -> bool override;
			auto VOnDeserialize(const YAML::Node& node) -> bool override;

		private:
			SkeletonHandle m_Skeleton;
			SkeletalModelHandle m_Model;
			std::vector<MaterialHandle> m_Materials;

			Renderer::SkeletonPose m_Pose;
			Renderer::SkeletonPose m_CachedPose;
			std::vector<Mat4> m_AnimationBoneTransforms;

			bool m_bReceiveShadows = true;
			bool m_bCastShadows = true;

			SkeletalAnimationHandle m_CurrentAnimation;
			SkeletalAnimationState m_CurrentAnimationState = StateIdle;
			bool m_bPlayOnStart = true;
			bool m_bLooping = true;
			float m_ElapsedTime = 0.0f;
			int32_t m_CurrentAnimationFrame = -1;

			std::vector<SkeletonAttachment> m_Attachments;
		};
	}
}
