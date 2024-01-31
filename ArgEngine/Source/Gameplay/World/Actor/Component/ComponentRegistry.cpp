#include <arg_pch.hpp>
#include "ComponentRegistry.hpp"

#include "Components/Graphics/CameraComponent.hpp"
#include "Debug/Assert.hpp"
#include "Components/Graphics/PointLightComponent.hpp"
#include "Components/Graphics/SkeletalModelComponent.hpp"
#include "Components/Graphics/SpotLightComponent.hpp"
#include "Components/Graphics/StaticModelComponent.hpp"
#include "Components/Physics/PhysicsBodyComponent.hpp"
#include "Components/Sound/SoundPlayerComponent.hpp"

auto Arg::Gameplay::ComponentRegistry::CreateComponent(
	const std::string& componentID
) const -> std::shared_ptr<ActorComponent>
{
	const GUID ID = std::hash<std::string>{}(componentID);
	ARG_ASSERT(m_ComponentRegistry.contains(ID));
	auto& componentPrototype = m_ComponentRegistry.at(ID);
	return componentPrototype->VCreateDefault();
}

auto Arg::Gameplay::ComponentRegistry::CreateComponent(const GUID& componentID) const -> std::shared_ptr<ActorComponent>
{
	ARG_ASSERT(m_ComponentRegistry.contains(componentID));
	auto& componentPrototype = m_ComponentRegistry.at(componentID);
	return componentPrototype->VCreateDefault();
}

void Arg::Gameplay::ComponentRegistry::Clear()
{
	m_ComponentRegistry.clear();
	m_ComponentIDs.clear();
}

auto Arg::Gameplay::ComponentRegistry::ContainsComponent(const GUID& componentID) const -> bool
{
	return m_ComponentRegistry.contains(componentID);
}

auto Arg::Gameplay::ComponentRegistry::GetComponent(const GUID& componentID) const -> ActorComponent*
{
	ARG_ASSERT(m_ComponentRegistry.contains(componentID));
	return m_ComponentRegistry.at(componentID).get();
}

auto Arg::Gameplay::ComponentRegistry::GetComponentCount() const -> size_t
{
	return m_ComponentIDs.size();
}

auto Arg::Gameplay::ComponentRegistry::GetComponentID(size_t index) const -> GUID
{
	ARG_ASSERT(index < m_ComponentIDs.size());
	return m_ComponentIDs[index];
}

auto Arg::Gameplay::ComponentRegistry::GetComponentName(size_t index) const -> const std::string&
{
	const GUID componentID = GetComponentID(index);
	ARG_ASSERT(m_ComponentRegistry.contains(componentID));
	return m_ComponentRegistry.at(componentID)->VGetName();
}

void Arg::Gameplay::ComponentRegistry::RegisterComponents()
{
	Clear();

	RegisterComponent(new CameraComponent());
	RegisterComponent(new StaticModelComponent());
	RegisterComponent(new SkeletalModelComponent());
	RegisterComponent(new PointLightComponent());
	RegisterComponent(new SpotLightComponent());
	RegisterComponent(new PhysicsBodyComponent());
	RegisterComponent(new SoundPlayerComponent());
}

void Arg::Gameplay::ComponentRegistry::RegisterComponent(ActorComponent* prototype)
{
	const GUID componentID = prototype->VGetID();
	ARG_ASSERT_M(!m_ComponentRegistry.contains(componentID), "Component already registered!");
	m_ComponentRegistry[componentID] = std::unique_ptr<ActorComponent>(prototype);
	m_ComponentIDs.push_back(componentID);
}
