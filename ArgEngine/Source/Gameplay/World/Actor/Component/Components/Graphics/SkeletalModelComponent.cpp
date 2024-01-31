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

	if (m_bPlayOnStart)
	{
		Play(m_CurrentAnimation);
	}
}

void Arg::Gameplay::SkeletalModelComponent::VTick(const GameTime& gameTime)
{
	ActorComponent::VTick(gameTime);

	if (m_bIsPlaying)
	{
		m_ElapsedTime += gameTime.GetDeltaTime();
	}
}

void Arg::Gameplay::SkeletalModelComponent::VRender(Renderer::RenderContext& context)
{
	ActorComponent::VRender(context);

	if (m_Model.IsValid() && m_Skeleton.IsValid())
	{
		const auto& skeleton = m_Skeleton.Get()->GetSkeleton();
		const auto& model = m_Model.Get()->GetModel();
		const auto& transform = GetOwner()->GetTransform();

		std::vector<Renderer::Material*> materials(m_Materials.size());
		for (size_t i = 0; i < m_Materials.size(); i++)
		{
			materials[i] = m_Materials[i].IsValid()
				               ? m_Materials[i].Get()->GetMaterial().get()
				               : nullptr;
		}

		if (m_CurrentAnimation.IsValid())
		{
			m_CurrentAnimation.Get()->GetAnimation().CalculateTransforms(
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

void Arg::Gameplay::SkeletalModelComponent::SetSkeleton(const SkeletonHandle& skeleton)
{
	if (m_Skeleton.IsValid())
	{
		m_Skeleton.FreeRef();
	}

	SetModel(SkeletalModelHandle(GUID::Empty, nullptr));

	m_Skeleton = skeleton;
	if (m_Skeleton.IsValid())
	{
		m_Skeleton.AddRef();
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

void Arg::Gameplay::SkeletalModelComponent::Play(const SkeletalAnimationHandle& animation)
{
	SetCurrentAnimation(animation);
	m_ElapsedTime = 0.0f;
	m_bIsPlaying = true;
}

void Arg::Gameplay::SkeletalModelComponent::Stop()
{
	m_bIsPlaying = false;
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
	m_Skeleton = GetResourceCache()->CreateHandle<Content::SkeletonResource>(skeletonID);

	const GUID modelID = ValueOr<GUID>(node["ModelID"], GUID::Empty);
	m_Model = GetResourceCache()->CreateHandle<Content::SkeletalModelResource>(modelID);

	m_bCastShadows = ValueOr<bool>(node["CastShadows"], true);
	m_bReceiveShadows = ValueOr<bool>(node["ReceiveShadows"], true);

	m_Materials.clear();
	const auto& materialsNode = node["Materials"];
	if (materialsNode)
	{
		const size_t materialsCount = materialsNode.size();
		m_Materials = std::vector<MaterialHandle>(materialsCount);
		for (size_t i = 0; i < materialsCount; i++)
		{
			const GUID materialID = ValueOr<GUID>(materialsNode[i]["ID"], GUID::Empty);
			m_Materials[i] = GetResourceCache()->CreateHandle<Content::MaterialResource>(materialID);
		}
	}
	else
	{
		m_Materials = std::vector<MaterialHandle>(1);
		m_Materials[0] = GetResourceCache()->CreateHandle<Content::MaterialResource>(GUID::Empty);
	}

	const GUID animationID = ValueOr<GUID>(node["AnimationID"], GUID::Empty);
	m_CurrentAnimation = GetResourceCache()->CreateHandle<Content::SkeletalAnimationResource>(animationID);
	m_bPlayOnStart = ValueOr<bool>(node["PlayOnStart"], true);
	m_bLooping = ValueOr<bool>(node["Looping"], true);

	return true;
}
