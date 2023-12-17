#include <arg_pch.hpp>
#include "ComponentRegistry.hpp"

#include "Debug/Assert.hpp"
#include "Components/StaticModelComponent.hpp"

auto Arg::Gameplay::ComponentRegistry::CreateComponent(
	const std::string& componentID
) const -> std::shared_ptr<ActorComponent>
{
	const GUID ID = std::hash<std::string>{}(componentID);
	ARG_ASSERT(m_ComponentRegistry.contains(ID), "Invalid component ID!");
	auto& componentPrototype = m_ComponentRegistry.at(ID);
	return componentPrototype->VCreateDefault();
}

auto Arg::Gameplay::ComponentRegistry::CreateComponent(const GUID& componentID) const -> std::shared_ptr<ActorComponent>
{
	ARG_ASSERT(m_ComponentRegistry.contains(componentID), "Invalid component ID!");
	auto& componentPrototype = m_ComponentRegistry.at(componentID);
	return componentPrototype->VCreateDefault();
}

void Arg::Gameplay::ComponentRegistry::Clear()
{
	m_ComponentRegistry.clear();
}

void Arg::Gameplay::ComponentRegistry::RegisterComponents()
{
	m_ComponentRegistry.clear();

	m_ComponentRegistry[StaticModelComponent::COMPONENT_ID] = std::make_unique<StaticModelComponent>();
}
