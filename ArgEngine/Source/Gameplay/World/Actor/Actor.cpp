#include <arg_pch.hpp>
#include "Actor.hpp"

#include "Debug/Assert.hpp"

#include "Gameplay/World/GameWorld.hpp"

Arg::Gameplay::Actor::Actor(GUID ID, GameWorld* world)
	: m_ID(ID), m_pWorld(world)
{
}

void Arg::Gameplay::Actor::SetName(const std::string& name)
{
	m_Name = name;
}

auto Arg::Gameplay::Actor::GetComponentCount() const -> size_t
{
	return m_Components.size();
}

auto Arg::Gameplay::Actor::HasComponent(const GUID& componentID) const -> bool
{
	return m_ComponentsRegistry.contains(componentID);
}

auto Arg::Gameplay::Actor::GetComponent(const GUID& componentID) -> std::shared_ptr<ActorComponent>&
{
	ARG_ASSERT(m_ComponentsRegistry.contains(componentID));
	return m_ComponentsRegistry.at(componentID);
}

auto Arg::Gameplay::Actor::GetComponentByIndex(const size_t index) -> std::shared_ptr<ActorComponent>&
{
	ARG_ASSERT(index < m_Components.size());
	return m_Components[index];
}

auto Arg::Gameplay::Actor::AddComponent(const std::shared_ptr<ActorComponent>& component) -> bool
{
	const GUID componentID = component->VGetID();
	if (m_ComponentsRegistry.contains(componentID))
	{
		ARG_CONSOLE_LOG_WARN(
			"Actor already has component with ID: %llu",
			static_cast<uint64_t>(componentID)
		);
		return false;
	}

	m_Components.push_back(component);
	m_ComponentsRegistry[componentID] = component;
	component->SetOwner(this);
	component->VOnComponentAdded();
	return true;
}

auto Arg::Gameplay::Actor::RemoveComponent(const GUID& componentID) -> bool
{
	if (!m_ComponentsRegistry.contains(componentID))
	{
		ARG_CONSOLE_LOG_WARN(
			"Actor has no component with ID: %llu",
			static_cast<uint64_t>(componentID)
		);
		return false;
	}

	const auto& component = m_ComponentsRegistry.at(componentID);

	component->VOnComponentRemoved();
	std::erase(m_Components, component);
	m_ComponentsRegistry[componentID]->SetOwner(nullptr);
	m_ComponentsRegistry[componentID] = nullptr;
	m_ComponentsRegistry.erase(componentID);
	return true;
}

void Arg::Gameplay::Actor::SetParentActor(Actor* actor)
{
	m_pParentActor = actor;
}

void Arg::Gameplay::Actor::AddChildActor(Actor* actor)
{
	ARG_ASSERT(std::ranges::find(m_pChildActors, actor) == m_pChildActors.end());
	m_pChildActors.push_back(actor);
}

void Arg::Gameplay::Actor::RemoveChildActor(const Actor* actor)
{
	std::erase(m_pChildActors, actor);
}

void Arg::Gameplay::Actor::ClearChildActors()
{
	for (size_t i = 0; i < GetChildActorsCount(); i++)
	{
		m_pChildActors[i] = nullptr;
	}

	m_pChildActors.clear();
}

void Arg::Gameplay::Actor::UpdateTransform(const Mat4& parentTransform)
{
	if (m_bRefreshTransform)
	{
		RefreshTransform(parentTransform);
		return;
	}

	for (size_t i = 0; i < GetChildActorsCount(); i++)
	{
		const auto childActor = GetChildActor(i);
		childActor->UpdateTransform(m_GlobalTransform);
	}
}

void Arg::Gameplay::Actor::ReparentTransform(const Actor& newParentActor)
{
	const Mat4 newParentTransform = newParentActor.m_GlobalTransform;
	Vec3 position;
	Quat rotation;
	Vec3 scale;
	const Mat4 newLocalTransform = Math::inverse(newParentTransform) * m_GlobalTransform;
	Math::Decompose(newLocalTransform, position, rotation, scale);
	SetLocalPosition(position);
	SetLocalRotation(rotation);
	SetLocalScale(scale);
}

void Arg::Gameplay::Actor::SetLocalPosition(const Vec3& position)
{
	m_bRefreshTransform = true;
	m_Transform.SetPosition(position);
}

void Arg::Gameplay::Actor::SetLocalRotation(const Quat& rotation)
{
	m_bRefreshTransform = true;
	m_Transform.SetRotation(rotation);
}

void Arg::Gameplay::Actor::SetLocalRotationEuler(const Vec3& rotationEuler)
{
	m_bRefreshTransform = true;
	m_Transform.SetRotationEuler(rotationEuler);
}

void Arg::Gameplay::Actor::SetLocalScale(const Vec3& scale)
{
	m_bRefreshTransform = true;
	m_Transform.SetScale(scale);
}

auto Arg::Gameplay::Actor::GetPosition() const -> Vec3
{
	Vec3 position;
	Quat rotation;
	Vec3 scale;
	Math::Decompose(m_GlobalTransform, position, rotation, scale);
	return position;
}

void Arg::Gameplay::Actor::SetPosition(const Vec3& position)
{
	m_bRefreshTransform = true;
	Mat4 localTransform = m_Transform.FindTransform();
	const Mat4 parentTransform = m_GlobalTransform * Math::inverse(localTransform);
	Vec3 dPosition;
	Quat dRotation;
	Vec3 dScale;
	Math::Decompose(m_GlobalTransform, dPosition, dRotation, dScale);
	m_GlobalTransform = Math::CalculateTransform(position, dRotation, dScale);
	localTransform = Math::inverse(parentTransform) * m_GlobalTransform;
	Math::Decompose(localTransform, dPosition, dRotation, dScale);
	SetLocalPosition(dPosition);
}

auto Arg::Gameplay::Actor::GetRotation() const -> Quat
{
	Vec3 position;
	Quat rotation;
	Vec3 scale;
	Math::Decompose(m_GlobalTransform, position, rotation, scale);
	return rotation;
}

void Arg::Gameplay::Actor::SetRotation(const Quat& rotation)
{
	m_bRefreshTransform = true;
	Mat4 localTransform = m_Transform.FindTransform();
	const Mat4 parentTransform = m_GlobalTransform * Math::inverse(localTransform);
	Vec3 dPosition;
	Quat dRotation;
	Vec3 dScale;
	Math::Decompose(m_GlobalTransform, dPosition, dRotation, dScale);
	m_GlobalTransform = Math::CalculateTransform(dPosition, rotation, dScale);
	localTransform = Math::inverse(parentTransform) * m_GlobalTransform;
	Math::Decompose(localTransform, dPosition, dRotation, dScale);
	SetLocalRotation(dRotation);
}

auto Arg::Gameplay::Actor::GetRotationEuler() const -> Vec3
{
	Vec3 position;
	Vec3 rotation;
	Vec3 scale;
	Math::Decompose(m_GlobalTransform, position, rotation, scale);
	return rotation;
}

void Arg::Gameplay::Actor::SetRotationEuler(const Vec3& rotationEuler)
{
	const Quat rotation = Math::ToQuat(rotationEuler);
	SetRotation(rotation);
}

auto Arg::Gameplay::Actor::GetScale() const -> Vec3
{
	Vec3 position;
	Quat rotation;
	Vec3 scale;
	Math::Decompose(m_GlobalTransform, position, rotation, scale);
	return scale;
}

void Arg::Gameplay::Actor::SetScale(const Vec3& scale)
{
	m_bRefreshTransform = true;
	Mat4 localTransform = m_Transform.FindTransform();
	const Mat4 parentTransform = m_GlobalTransform * Math::inverse(localTransform);
	Vec3 dPosition;
	Quat dRotation;
	Vec3 dScale;
	Math::Decompose(m_GlobalTransform, dPosition, dRotation, dScale);
	m_GlobalTransform = Math::CalculateTransform(dPosition, dRotation, scale);
	localTransform = Math::inverse(parentTransform) * m_GlobalTransform;
	Math::Decompose(localTransform, dPosition, dRotation, dScale);
	SetLocalScale(dScale);
}

auto Arg::Gameplay::Actor::GetForwardVec() const -> Vec3
{
	const Vec3 rotation = GetRotationEuler();
	return Math::ForwardVecFromRotation(rotation);
}

auto Arg::Gameplay::Actor::GetRightVec() const -> Vec3
{
	const Vec3 rotation = GetRotationEuler();
	return Math::RightVecFromRotation(rotation);
}

auto Arg::Gameplay::Actor::GetUpVec() const -> Vec3
{
	const Vec3 rotation = GetRotationEuler();
	return Math::UpVecFromRotation(rotation);
}

void Arg::Gameplay::Actor::BeginPlay()
{
	if (m_bIsDestroyed)
	{
		return;
	}

	for (const auto& component : m_Components)
	{
		component->VBeginPlay();
	}
}

void Arg::Gameplay::Actor::Tick(const GameTime& gameTime)
{
	if (m_bIsDestroyed)
	{
		return;
	}

	for (const auto& component : m_Components)
	{
		if (!component->CanEverTick())
		{
			continue;
		}

		component->VTick(gameTime);
	}
}

void Arg::Gameplay::Actor::Render(Renderer::RenderContext& context)
{
	if (m_bIsDestroyed)
	{
		return;
	}

	for (const auto& component : m_Components)
	{
		if (!component->CanEverRender())
		{
			continue;
		}

		component->VRender(context);
	}
}

void Arg::Gameplay::Actor::DrawDebug(Renderer::RenderContext& context)
{
	if (m_bIsDestroyed)
	{
		return;
	}

	for (const auto& component : m_Components)
	{
		component->VDrawDebug(context);
	}
}

void Arg::Gameplay::Actor::Destroy()
{
	std::vector<GUID> componentsToRemove;
	componentsToRemove.reserve(m_Components.size());
	for (const auto& component : m_Components)
	{
		componentsToRemove.push_back(component->VGetID());
	}

	for (const auto& componentID : componentsToRemove)
	{
		RemoveComponent(componentID);
	}
}

auto Arg::Gameplay::Actor::VOnSerialize(YAML::Node& node) const -> bool
{
	node["ID"] = m_ID;
	node["Name"] = m_Name;

	node["Transform"]["Position"] = GetLocalPosition();
	node["Transform"]["Rotation"] = Math::degrees(GetLocalRotationEuler());
	node["Transform"]["Scale"] = GetLocalScale();

	auto componentsNode = node["Components"];
	componentsNode.reset();
	for (const auto& component : m_Components)
	{
		YAML::Node componentNode;
		componentNode["ID"] = component->VGetName();
		if (component->Serialize(componentNode))
		{
			componentsNode.push_back(componentNode);
		}
	}

	node["Components"] = componentsNode;

	return true;
}

auto Arg::Gameplay::Actor::VOnDeserialize(const YAML::Node& node) -> bool
{
	m_ID = ValueOr<GUID>(node["ID"], GUID::Empty);
	m_Name = ValueOr<std::string>(node["Name"], "Actor");
	if (node["Transform"])
	{
		const Vec3 localPosition = ValueOr<Vec3>(
			node["Transform"]["Position"],
			Transform::POSITION_DEFAULT
		);
		const Vec3 localRotation = Math::radians(ValueOr<Vec3>(
			node["Transform"]["Rotation"],
			Transform::ROTATION_EULER_DEFAULT
		));
		const Vec3 localScale = ValueOr<Vec3>(
			node["Transform"]["Scale"],
			Transform::SCALE_DEFAULT
		);
		m_Transform = Transform(localPosition, localRotation, localScale);
	}

	if (node["Components"])
	{
		const auto componentRegistry = GetComponentRegistry();
		for (size_t i = 0; i < node["Components"].size(); i++)
		{
			const auto& componentNode = node["Components"][i];
			if (!componentNode["ID"])
			{
				continue;
			}

			const std::string componentID = ValueOr<std::string>(componentNode["ID"], "");
			const GUID ID = std::hash<std::string>{}(componentID);
			if (!componentRegistry->ContainsComponent(ID))
			{
				continue;
			}

			auto component = componentRegistry->CreateComponent(componentID);

			m_Components.push_back(component);
			m_ComponentsRegistry[component->VGetID()] = component;

			component->SetOwner(this);
			component->Deserialize(componentNode);
			component->VOnComponentAdded();
		}
	}

	m_bRefreshTransform = true;

	return true;
}

void Arg::Gameplay::Actor::RefreshTransform(const Mat4& parentTransform)
{
	m_bRefreshTransform = false;
	const Mat4 localTransform = m_Transform.FindTransform();
	m_GlobalTransform = parentTransform * localTransform;
	for (size_t i = 0; i < GetChildActorsCount(); i++)
	{
		const auto childActor = GetChildActor(i);
		childActor->RefreshTransform(m_GlobalTransform);
	}
}

auto Arg::Gameplay::Actor::GetResourceCache() const -> Content::ResourceCache*
{
	return GetWorld()->GetResourceCache();
}

auto Arg::Gameplay::Actor::GetComponentRegistry() const -> ComponentRegistry*
{
	return GetWorld()->GetComponentRegistry();
}
