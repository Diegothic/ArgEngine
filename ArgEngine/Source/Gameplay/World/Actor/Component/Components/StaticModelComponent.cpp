#include <arg_pch.hpp>
#include "StaticModelComponent.hpp"

#include "Gameplay/World/Actor/Actor.hpp"

const Arg::GUID Arg::Gameplay::StaticModelComponent::COMPONENT_ID
= std::hash<std::string>{}("StaticModelComponent");

const std::string Arg::Gameplay::StaticModelComponent::COMPONENT_NAME = "StaticModelComponent";

Arg::Gameplay::StaticModelComponent::StaticModelComponent()
{
	m_bCanEverTick = false;
}

auto Arg::Gameplay::StaticModelComponent::VCreateDefault() const
-> std::shared_ptr<ActorComponent>
{
	return std::make_shared<StaticModelComponent>();
}

void Arg::Gameplay::StaticModelComponent::VRender(
	Renderer::RenderContext& context
)
{
	if (m_StaticModel.IsValid())
	{
		const auto& staticModel = m_StaticModel.Get()->GetStaticModel();
		const auto& transform = GetOwner()->GetTransform();
		std::shared_ptr<Renderer::Material> material = nullptr;
		if (m_Materials.size() > 0 && m_Materials[0].IsValid())
		{
			material = m_Materials[0].Get()->GetMaterial();
		}

		context.DrawModel(
			staticModel,
			transform,
			material,
			m_bReceiveShadows,
			m_bCastShadows
		);
	}
}

void Arg::Gameplay::StaticModelComponent::VOnComponentAdded()
{
	m_Materials.clear();
	if (m_StaticModel.IsValid())
	{
		m_StaticModel.AddRef();
		m_Materials = std::vector<MaterialHandle>(
			m_StaticModel.Get()
			->GetStaticModel()
			->GetMaterialCount()
		);
	}
}

void Arg::Gameplay::StaticModelComponent::VOnComponentRemoved()
{
	for (const auto& material : m_Materials)
	{
		if (material.IsValid())
		{
			material.Get()->FreeRef();
		}
	}
	m_Materials.clear();
	if (m_StaticModel.IsValid())
	{
		m_StaticModel.FreeRef();
	}
}

void Arg::Gameplay::StaticModelComponent::SetStaticModel(
	const Content::ResourceHandle<Content::StaticModelResource>& staticModel
)
{
	for (const auto& material : m_Materials)
	{
		if (material.IsValid())
		{
			material.Get()->FreeRef();
		}
	}
	m_Materials.clear();

	if (m_StaticModel.IsValid())
	{
		m_StaticModel.FreeRef();
	}

	m_StaticModel = staticModel;
	
	if (m_StaticModel.IsValid())
	{
		m_StaticModel.AddRef();
		m_Materials = std::vector<MaterialHandle>(
			m_StaticModel.Get()
			->GetStaticModel()
			->GetMaterialCount()
		);
	}
}

void Arg::Gameplay::StaticModelComponent::SetMaterial(const size_t index, const MaterialHandle& material)
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

void Arg::Gameplay::StaticModelComponent::SetReceiveShadows(const bool bReceiveShadows)
{
	m_bReceiveShadows = bReceiveShadows;
}

void Arg::Gameplay::StaticModelComponent::SetCastShadows(const bool bCastShadows)
{
	m_bCastShadows = bCastShadows;
}

auto Arg::Gameplay::StaticModelComponent::VOnSerialize(
	YAML::Node& node
) const -> bool
{
	ActorComponent::VOnSerialize(node);

	node["ModelID"] = m_StaticModel.GetID();

	return true;
}

auto Arg::Gameplay::StaticModelComponent::VOnDeserialize(
	const YAML::Node& node
) -> bool
{
	ActorComponent::VOnDeserialize(node);

	const GUID modelID = ValueOr<GUID>(node["ModelID"], GUID::Empty);
	m_StaticModel = GetResourceCache()->CreateHandle<Content::StaticModelResource>(modelID);

	return true;
}
