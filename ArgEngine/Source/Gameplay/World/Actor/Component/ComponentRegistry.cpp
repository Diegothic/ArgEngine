#include <arg_pch.hpp>
#include "ComponentRegistry.hpp"

#include "Debug/Assert.hpp"
#include "Components/PointLightComponent.hpp"
#include "Components/SkeletalModelComponent.hpp"
#include "Components/SpotLightComponent.hpp"
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
	m_ComponentIDs.clear();
}

auto Arg::Gameplay::ComponentRegistry::GetComponent(const GUID& componentID) const -> ActorComponent*
{
	ARG_ASSERT(m_ComponentRegistry.contains(componentID), "");
	return m_ComponentRegistry.at(componentID).get();
}

auto Arg::Gameplay::ComponentRegistry::GetComponentCount() const -> size_t
{
	return m_ComponentIDs.size();
}

auto Arg::Gameplay::ComponentRegistry::GetComponentID(size_t index) const -> GUID
{
	ARG_ASSERT(index < m_ComponentIDs.size(), "Index out of range!");
	return m_ComponentIDs[index];
}

auto Arg::Gameplay::ComponentRegistry::GetComponentName(size_t index) const -> const std::string&
{
	const GUID componentID = GetComponentID(index);
	ARG_ASSERT(m_ComponentRegistry.contains(componentID), "Invalid component ID!");
	return m_ComponentRegistry.at(componentID)->VGetName();
}

void Arg::Gameplay::ComponentRegistry::RegisterComponents()
{
	Clear();

	RegisterComponent(new StaticModelComponent());
	RegisterComponent(new SkeletalModelComponent());
	RegisterComponent(new PointLightComponent());
	RegisterComponent(new SpotLightComponent());
}

void Arg::Gameplay::ComponentRegistry::RegisterComponent(ActorComponent* prototype)
{
	const GUID componentID = prototype->VGetID();
	ARG_ASSERT(!m_ComponentRegistry.contains(componentID), "Component already registered!");
	m_ComponentRegistry[componentID] = std::unique_ptr<ActorComponent>(prototype);
	m_ComponentIDs.push_back(componentID);
}
