#include <arg_pch.hpp>
#include "SkeletalModelComponent.hpp"

#include "Gameplay/World/Actor/Actor.hpp"

const Arg::GUID Arg::Gameplay::SkeletalModelComponent::COMPONENT_ID
	= std::hash<std::string>{}("SkeletalModelComponent");

const std::string Arg::Gameplay::SkeletalModelComponent::COMPONENT_NAME = "SkeletalModelComponent";

Arg::Gameplay::SkeletalModelComponent::SkeletalModelComponent()
{
}

auto Arg::Gameplay::SkeletalModelComponent::VCreateDefault() -> std::shared_ptr<ActorComponent>
{
	return std::make_shared<SkeletalModelComponent>();
}

void Arg::Gameplay::SkeletalModelComponent::VBeginPlay()
{
	ActorComponent::VBeginPlay();

	m_CurrentAnimationState = SkeletalAnimationState::StateIdle;
	if (m_bPlayOnStart && m_CurrentAnimation.IsValid())
	{
		Play(m_CurrentAnimation);
	}
}

void Arg::Gameplay::SkeletalModelComponent::VEndPlay()
{
	ActorComponent::VEndPlay();

	Ev_OnAnimationStart.Clear();
	Ev_OnAnimationEnd.Clear();
	Ev_OnAnimationEvent.Clear();
}

void Arg::Gameplay::SkeletalModelComponent::VTick(const GameTime& gameTime, const GameInput& gameInput)
{
	ActorComponent::VTick(gameTime, gameInput);

	if (m_CurrentAnimation.IsValid())
	{
		const Renderer::SkeletalAnimation* pAnimation = m_CurrentAnimation.Get()->GetAnimation();
		switch (m_CurrentAnimationState)
		{
		case SkeletalAnimationState::StateIdle:
			{
				break;
			}
		case SkeletalAnimationState::StateStarted:
			{
				m_ElapsedTime = 0.0f;
				Ev_OnAnimationStart.Invoke();
				m_CurrentAnimationState = SkeletalAnimationState::StatePlaying;
				break;
			}
		case SkeletalAnimationState::StatePlaying:
			{
				m_ElapsedTime += gameTime.GetDeltaTime();
				const int32_t currentAnimationFrame = static_cast<int32_t>(Math::floor(
					Math::mod(m_ElapsedTime, pAnimation->GetDuration() / pAnimation->GetTicksPerSecond())
					/ (1.0f / pAnimation->GetTicksPerSecond())
				));
				if (currentAnimationFrame != m_CurrentAnimationFrame)
				{
					m_CurrentAnimationFrame = currentAnimationFrame;
					for (size_t i = 0; i < pAnimation->GetEventsCount(); i++)
					{
						const Renderer::SkeletalAnimationEvent& event = pAnimation->GetEvent(i);
						if (event.Frame == m_CurrentAnimationFrame)
						{
							Ev_OnAnimationEvent.Invoke(event.Name);
						}
					}
				}

				if (
					!m_bLooping
					&& m_ElapsedTime * pAnimation->GetTicksPerSecond() >= pAnimation->GetDuration()
				)
				{
					m_CurrentAnimationState = SkeletalAnimationState::StateFinished;
				}
				break;
			}
		case SkeletalAnimationState::StatePaused:
			{
				break;
			}
		case SkeletalAnimationState::StateFinished:
			{
				Ev_OnAnimationEnd.Invoke();
				m_CurrentAnimationState = SkeletalAnimationState::StateIdle;
				break;
			}
		}
	}

	const Actor* pOwner = GetOwner();
	if (m_Model.IsValid() && m_Skeleton.IsValid())
	{
		const auto& skeleton = m_Skeleton.Get()->GetSkeleton();
		if (m_CurrentAnimation.IsValid())
		{
			m_CurrentAnimation.Get()->GetAnimation()->CalculateTransforms(
				m_ElapsedTime,
				m_bLooping,
				m_AnimationBoneTransforms
			);
			skeleton.CalculatePose(m_AnimationBoneTransforms, m_Pose);
		}
		else
		{
			skeleton.CalculateRestPose(m_Pose);
		}

		for (const auto& attachment : m_Attachments)
		{
			if (attachment.ChildActorIndex < 0
				|| attachment.ChildActorIndex >= static_cast<int32_t>(pOwner->GetChildActorsCount())
				|| attachment.BoneIndex >= static_cast<int32_t>(m_Pose.BoneTransforms.size()))
			{
				continue;
			}

			Actor* pChildActor = pOwner->GetChildActor(attachment.ChildActorIndex);
			const Mat4& boneTransform = m_Skeleton.Get()->GetSkeleton()
			                                      .FindWorldBoneTransform(attachment.BoneIndex, m_Pose);
			Vec3 position;
			Quat rotation;
			Vec3 scale;
			Math::Decompose(boneTransform, position, rotation, scale);
			pChildActor->SetLocalPosition(position);
			pChildActor->SetLocalRotation(rotation);
			pChildActor->SetLocalScale(scale);
		}
	}
}

void Arg::Gameplay::SkeletalModelComponent::VRender(Renderer::RenderContext& context)
{
	ActorComponent::VRender(context);

	if (m_Model.IsValid() && m_Skeleton.IsValid())
	{
		const auto& model = m_Model.Get()->GetModel();
		const auto& transform = GetOwner()->GetTransform();

		std::vector<Renderer::Material*> materials(m_Materials.size());
		for (size_t i = 0; i < m_Materials.size(); i++)
		{
			materials[i] = m_Materials[i].IsValid()
				               ? m_Materials[i].Get()->GetMaterial().get()
				               : nullptr;
		}

		context.DrawSkeletalModel(
			m_Pose,
			model,
			transform,
			materials,
			m_bReceiveShadows,
			m_bCastShadows
		);
	}
}

void Arg::Gameplay::SkeletalModelComponent::VOnComponentAdded()
{
	if (m_Model.IsValid() && m_Skeleton.IsValid())
	{
		m_Skeleton.AddRef();
		m_Model.AddRef();
		for (auto& material : m_Materials)
		{
			if (material.IsValid())
			{
				material.AddRef();
			}
		}

		if (m_CurrentAnimation.IsValid())
		{
			m_CurrentAnimation.AddRef();
		}
	}
}

void Arg::Gameplay::SkeletalModelComponent::VOnComponentRemoved()
{
	for (const auto& material : m_Materials)
	{
		if (material.IsValid())
		{
			material.Get()->FreeRef();
		}
	}
	m_Materials.clear();
	if (m_Model.IsValid())
	{
		m_Model.FreeRef();
	}
	if (m_Skeleton.IsValid())
	{
		m_Skeleton.FreeRef();
	}
	if (m_CurrentAnimation.IsValid())
	{
		m_CurrentAnimation.FreeRef();
	}
}

void Arg::Gameplay::SkeletalModelComponent::VClone(const ActorComponent* pActorComponent)
{
	const SkeletalModelComponent* pSkeletalModelComponent = dynamic_cast<const SkeletalModelComponent*>(
		pActorComponent
	);
	ARG_ASSERT(pSkeletalModelComponent != nullptr);
	SetSkeleton(pSkeletalModelComponent->GetSkeleton());
	SetModel(pSkeletalModelComponent->GetModel());
	SetCastShadows(pSkeletalModelComponent->GetCastShadows());
	SetReceiveShadows(pSkeletalModelComponent->GetReceiveShadows());
	for (size_t i = 0; i < pSkeletalModelComponent->GetMaterialCount(); i++)
	{
		SetMaterial(i, pSkeletalModelComponent->GetMaterial(i));
	}
	SetCurrentAnimation(pSkeletalModelComponent->GetCurrentAnimation());
	SetPlayOnStart(pSkeletalModelComponent->GetPlayOnStart());
	SetLooping(pSkeletalModelComponent->GetLooping());
	for (size_t i = 0; i < pSkeletalModelComponent->GetAttachmentCount(); i++)
	{
		AddAttachment(pSkeletalModelComponent->GetAttachment(i));
	}
}

void Arg::Gameplay::SkeletalModelComponent::SetSkeleton(const SkeletonHandle& skeleton)
{
	m_Attachments.clear();

	if (m_Skeleton.IsValid())
	{
		m_Skeleton.FreeRef();
	}

	SetCurrentAnimation(SkeletalAnimationHandle(GUID::Empty, nullptr));
	SetModel(SkeletalModelHandle(GUID::Empty, nullptr));

	m_Skeleton = skeleton;
	if (m_Skeleton.IsValid())
	{
		m_Skeleton.AddRef();
		skeleton.Get()->GetSkeleton().CalculateRestPose(m_Pose);
	}
}

void Arg::Gameplay::SkeletalModelComponent::SetModel(const SkeletalModelHandle& model)
{
	for (const auto& material : m_Materials)
	{
		if (material.IsValid())
		{
			material.Get()->FreeRef();
		}
	}
	m_Materials.clear();

	if (m_Model.IsValid())
	{
		m_Model.FreeRef();
	}

	m_Model = model;

	if (m_Model.IsValid())
	{
		m_Model.AddRef();
		m_Materials = std::vector<MaterialHandle>(
			m_Model.Get()->GetModel()
			       .GetMaterialCount()
		);
	}
}

auto Arg::Gameplay::SkeletalModelComponent::GetMaterial(size_t index) const -> const MaterialHandle&
{
	ARG_ASSERT(index < m_Materials.size());
	return m_Materials[index];
}

void Arg::Gameplay::SkeletalModelComponent::SetMaterial(size_t index, const MaterialHandle& material)
{
	if (m_Materials[index].IsValid())
	{
		m_Materials[index].FreeRef();
	}

	m_Materials[index] = material;

	if (m_Materials[index].IsValid())
	{
		m_Materials[index].AddRef();
	}
}

void Arg::Gameplay::SkeletalModelComponent::SetReceiveShadows(bool bReceiveShadows)
{
	m_bReceiveShadows = bReceiveShadows;
}

void Arg::Gameplay::SkeletalModelComponent::SetCastShadows(bool bCastShadows)
{
	m_bCastShadows = bCastShadows;
}

void Arg::Gameplay::SkeletalModelComponent::SetCurrentAnimation(const SkeletalAnimationHandle& animation)
{
	m_CurrentAnimationState = SkeletalAnimationState::StateIdle;

	if (m_CurrentAnimation.IsValid())
	{
		m_CurrentAnimation.FreeRef();
	}

	m_CurrentAnimation = animation;

	if (m_CurrentAnimation.IsValid())
	{
		m_CurrentAnimation.AddRef();
	}
}

auto Arg::Gameplay::SkeletalModelComponent::GetAttachment(size_t index) const -> const SkeletonAttachment&
{
	ARG_ASSERT(index < m_Attachments.size());
	return m_Attachments[index];
}

void Arg::Gameplay::SkeletalModelComponent::SetAttachment(size_t index, const SkeletonAttachment& attachment)
{
	ARG_ASSERT(index < m_Attachments.size());
	for (auto& currentAttachment : m_Attachments)
	{
		if (currentAttachment.ChildActorIndex == attachment.ChildActorIndex)
		{
			currentAttachment.ChildActorIndex = -1;
		}
	}

	m_Attachments[index] = attachment;
}

void Arg::Gameplay::SkeletalModelComponent::AddAttachment(const SkeletonAttachment& attachment)
{
	for (auto& currentAttachment : m_Attachments)
	{
		if (currentAttachment.ChildActorIndex == attachment.ChildActorIndex)
		{
			currentAttachment.ChildActorIndex = -1;
		}
	}

	m_Attachments.push_back(attachment);
}

void Arg::Gameplay::SkeletalModelComponent::RemoveAttachment(size_t index)
{
	ARG_ASSERT(index < m_Attachments.size());
	m_Attachments.erase(m_Attachments.begin() + static_cast<long long>(index));
}

void Arg::Gameplay::SkeletalModelComponent::Play(const SkeletalAnimationHandle& animation)
{
	SetCurrentAnimation(animation);
	m_CurrentAnimationState = SkeletalAnimationState::StateStarted;
}

void Arg::Gameplay::SkeletalModelComponent::Pause()
{
	if (m_CurrentAnimationState != SkeletalAnimationState::StatePlaying)
	{
		return;
	}

	m_CurrentAnimationState = SkeletalAnimationState::StatePaused;
}

void Arg::Gameplay::SkeletalModelComponent::Unpause()
{
	if (m_CurrentAnimationState != SkeletalAnimationState::StatePaused)
	{
		return;
	}

	m_CurrentAnimationState = SkeletalAnimationState::StatePlaying;
}

void Arg::Gameplay::SkeletalModelComponent::Stop()
{
	m_CurrentAnimationState = SkeletalAnimationState::StateFinished;
}

bool Arg::Gameplay::SkeletalModelComponent::VOnSerialize(YAML::Node& node) const
{
	const bool bIsSuccess = ActorComponent::VOnSerialize(node);
	if (!bIsSuccess)
	{
		return false;
	}

	node["SkeletonID"] = m_Skeleton.GetID();
	node["ModelID"] = m_Model.GetID();

	node["CastShadows"] = m_bCastShadows;
	node["ReceiveShadows"] = m_bReceiveShadows;

	auto materialsNode = node["Materials"];
	materialsNode.reset();
	for (const auto& material : m_Materials)
	{
		YAML::Node materialNode;
		materialNode["ID"] = material.GetID();
		materialsNode.push_back(materialNode);
	}
	node["Materials"] = materialsNode;

	auto attachmentsNode = node["Attachments"];
	attachmentsNode.reset();
	for (const auto& attachment : m_Attachments)
	{
		YAML::Node attachmentNode;
		attachmentNode["ChildActorIndex"] = attachment.ChildActorIndex;
		attachmentNode["BoneIndex"] = attachment.BoneIndex;
		attachmentsNode.push_back(attachmentNode);
	}
	node["Attachments"] = attachmentsNode;

	node["AnimationID"] = m_CurrentAnimation.GetID();
	node["PlayOnStart"] = m_bPlayOnStart;
	node["Looping"] = m_bLooping;

	return true;
}

bool Arg::Gameplay::SkeletalModelComponent::VOnDeserialize(const YAML::Node& node)
{
	const bool bIsSuccess = ActorComponent::VOnDeserialize(node);
	if (!bIsSuccess)
	{
		return false;
	}

	const GUID skeletonID = ValueOr<GUID>(node["SkeletonID"], GUID::Empty);
	SetSkeleton(GetResourceCache()->CreateHandle<Content::SkeletonResource>(skeletonID));

	const GUID modelID = ValueOr<GUID>(node["ModelID"], GUID::Empty);
	SetModel(GetResourceCache()->CreateHandle<Content::SkeletalModelResource>(modelID));

	SetCastShadows(ValueOr<bool>(node["CastShadows"], true));
	SetReceiveShadows(ValueOr<bool>(node["ReceiveShadows"], true));

	m_Materials.clear();
	const auto& materialsNode = node["Materials"];
	if (materialsNode)
	{
		const size_t materialsCount = materialsNode.size();
		m_Materials = std::vector<MaterialHandle>(materialsCount);
		for (size_t i = 0; i < materialsCount; i++)
		{
			const GUID materialID = ValueOr<GUID>(materialsNode[i]["ID"], GUID::Empty);
			SetMaterial(i, GetResourceCache()->CreateHandle<Content::MaterialResource>(materialID));
		}
	}
	else
	{
		m_Materials = std::vector<MaterialHandle>(1);
		SetMaterial(0, GetResourceCache()->CreateHandle<Content::MaterialResource>(GUID::Empty));
	}

	m_Attachments.clear();
	const auto& attachmentsNode = node["Attachments"];
	if (attachmentsNode)
	{
		const size_t attachmentsCount = attachmentsNode.size();
		for (size_t i = 0; i < attachmentsCount; i++)
		{
			AddAttachment({
				ValueOr<int32_t>(attachmentsNode[i]["ChildActorIndex"], 0),
				ValueOr<int32_t>(attachmentsNode[i]["BoneIndex"], 0)
			});
		}
	}

	const GUID animationID = ValueOr<GUID>(node["AnimationID"], GUID::Empty);
	SetCurrentAnimation(GetResourceCache()->CreateHandle<Content::SkeletalAnimationResource>(animationID));
	SetPlayOnStart(ValueOr<bool>(node["PlayOnStart"], true));
	SetLooping(ValueOr<bool>(node["Looping"], true));

	return true;
}
