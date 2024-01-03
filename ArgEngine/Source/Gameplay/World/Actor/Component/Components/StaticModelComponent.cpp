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

auto Arg::Gameplay::StaticModelComponent::VCreateDefault()
	-> std::shared_ptr<ActorComponent>
{
	return std::make_shared<StaticModelComponent>();
}

void Arg::Gameplay::StaticModelComponent::VRender(
	Renderer::RenderContext& context
)
{
	ActorComponent::VRender(context);
	
	if (m_StaticModel.IsValid())
	{
		const auto& staticModel = m_StaticModel.Get()->GetStaticModel();
		const auto& transform = GetOwner()->GetTransform();

		std::vector<Renderer::Material*> materials(m_Materials.size());
		for (size_t i = 0; i < m_Materials.size(); i++)
		{
			materials[i] = m_Materials[i].IsValid()
				               ? m_Materials[i].Get()->GetMaterial().get()
				               : nullptr;
		}

		context.DrawModel(
			staticModel,
			transform,
			materials,
			m_bReceiveShadows,
			m_bCastShadows
		);
	}
}

void Arg::Gameplay::StaticModelComponent::VOnComponentAdded()
{
	if (m_StaticModel.IsValid())
	{
		m_StaticModel.AddRef();
		for (auto& material : m_Materials)
		{
			if (material.IsValid())
			{
				material.AddRef();
			}
		}
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

auto Arg::Gameplay::StaticModelComponent::GetMaterial(size_t index) const -> const MaterialHandle&
{
	ARG_ASSERT(index < m_Materials.size(), "Index out of range!");
	return m_Materials[index];
}

void Arg::Gameplay::StaticModelComponent::SetMaterial(size_t index, const MaterialHandle& material)
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

void Arg::Gameplay::StaticModelComponent::SetReceiveShadows(bool bReceiveShadows)
{
	m_bReceiveShadows = bReceiveShadows;
}

void Arg::Gameplay::StaticModelComponent::SetCastShadows(bool bCastShadows)
{
	m_bCastShadows = bCastShadows;
}

auto Arg::Gameplay::StaticModelComponent::VOnSerialize(
	YAML::Node& node
) const -> bool
{
	const bool bIsSuccess = ActorComponent::VOnSerialize(node);
	if (!bIsSuccess)
	{
		return false;
	}

	node["ModelID"] = m_StaticModel.GetID();
	auto materialsNode = node["Materials"];
	materialsNode.reset();
	for (const auto& material : m_Materials)
	{
		YAML::Node materialNode;
		materialNode["ID"] = material.GetID();
		materialsNode.push_back(materialNode);
	}

	node["Materials"] = materialsNode;

	return true;
}

auto Arg::Gameplay::StaticModelComponent::VOnDeserialize(
	const YAML::Node& node
) -> bool
{
	const bool bIsSuccess = ActorComponent::VOnDeserialize(node);
	if (!bIsSuccess)
	{
		return false;
	}

	const GUID modelID = ValueOr<GUID>(node["ModelID"], GUID::Empty);
	m_StaticModel = GetResourceCache()->CreateHandle<Content::StaticModelResource>(modelID);

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

	return true;
}
