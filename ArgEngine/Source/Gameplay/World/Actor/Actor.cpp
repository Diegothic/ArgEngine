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
	ARG_ASSERT(m_ComponentsRegistry.contains(componentID), "Actor does not have a component of type specified!");
	return m_ComponentsRegistry.at(componentID);
}

auto Arg::Gameplay::Actor::GetComponentByIndex(const size_t index) -> std::shared_ptr<ActorComponent>&
{
	ARG_ASSERT(index < m_Components.size(), "Index out of range!");
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
	const auto it = std::ranges::find(m_Components, component);
	if (it != m_Components.end())
	{
		m_Components.erase(it);
	}
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
	ARG_ASSERT(std::ranges::find(m_pChildActors, actor) == m_pChildActors.end(), "Actor is already a child actor!");
	m_pChildActors.push_back(actor);
}

void Arg::Gameplay::Actor::RemoveChildActor(const Actor* actor)
{
	const auto it = std::ranges::find(m_pChildActors, actor);
	ARG_ASSERT(it != m_pChildActors.end(), "Child actor not found!");
	*it = nullptr;
	m_pChildActors.erase(it);
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
	Vec3 translation, rotation, scale;
	const Mat4 newLocalTransform = Math::inverse(newParentTransform) * m_GlobalTransform;
	Math::Decompose(newLocalTransform, translation, rotation, scale);
	SetLocalPosition(translation);
	SetLocalRotation(Math::degrees(rotation));
	SetLocalScale(scale);
}

void Arg::Gameplay::Actor::SetLocalPosition(const Vec3& position)
{
	m_bRefreshTransform = true;
	m_Transform.SetTranslation(position);
}

void Arg::Gameplay::Actor::SetLocalRotation(const Vec3& rotation)
{
	m_bRefreshTransform = true;
	m_Transform.SetRotation(rotation);
}

void Arg::Gameplay::Actor::SetLocalScale(const Vec3& scale)
{
	m_bRefreshTransform = true;
	m_Transform.SetScale(scale);
}

auto Arg::Gameplay::Actor::GetPosition() const -> Vec3
{
	Vec3 translation;
	Vec3 rotation;
	Vec3 scale;
	Math::Decompose(m_GlobalTransform, translation, rotation, scale);
	return translation;
}

void Arg::Gameplay::Actor::SetPosition(const Vec3& position)
{
	m_bRefreshTransform = true;
	Mat4 localTransform = m_Transform.FindTransform();
	const Mat4 parentTransform = m_GlobalTransform * Math::inverse(localTransform);
	Vec3 dTranslation, dRotation, dScale;
	Math::Decompose(m_GlobalTransform, dTranslation, dRotation, dScale);

	m_GlobalTransform = Mat4(1.0f);
	m_GlobalTransform = Math::translate(m_GlobalTransform, position);
	m_GlobalTransform = Math::rotate(m_GlobalTransform, dRotation.z, Vec3(0.0f, 0.0f, 1.0f));
	m_GlobalTransform = Math::rotate(m_GlobalTransform, dRotation.y, Vec3(0.0f, 1.0f, 0.0f));
	m_GlobalTransform = Math::rotate(m_GlobalTransform, dRotation.x, Vec3(1.0f, 0.0f, 0.0f));
	m_GlobalTransform = Math::scale(m_GlobalTransform, dScale);

	localTransform = Math::inverse(parentTransform) * m_GlobalTransform;
	Math::Decompose(localTransform, dTranslation, dRotation, dScale);

	SetLocalPosition(dTranslation);
}

auto Arg::Gameplay::Actor::GetRotation() const -> Vec3
{
	Vec3 translation;
	Vec3 rotation;
	Vec3 scale;
	Math::Decompose(m_GlobalTransform, translation, rotation, scale);
	return Math::degrees(rotation);
}

void Arg::Gameplay::Actor::SetRotation(const Vec3& rotation)
{
	m_bRefreshTransform = true;
	Mat4 localTransform = m_Transform.FindTransform();
	const Mat4 parentTransform = m_GlobalTransform * Math::inverse(localTransform);
	Vec3 dTranslation, dRotation, dScale;
	Math::Decompose(m_GlobalTransform, dTranslation, dRotation, dScale);

	m_GlobalTransform = Mat4(1.0f);
	m_GlobalTransform = Math::translate(m_GlobalTransform, dTranslation);
	m_GlobalTransform = Math::rotate(m_GlobalTransform, Math::radians(rotation.z), Vec3(0.0f, 0.0f, 1.0f));
	m_GlobalTransform = Math::rotate(m_GlobalTransform, Math::radians(rotation.y), Vec3(0.0f, 1.0f, 0.0f));
	m_GlobalTransform = Math::rotate(m_GlobalTransform, Math::radians(rotation.x), Vec3(1.0f, 0.0f, 0.0f));
	m_GlobalTransform = Math::scale(m_GlobalTransform, dScale);

	localTransform = Math::inverse(parentTransform) * m_GlobalTransform;
	Math::Decompose(localTransform, dTranslation, dRotation, dScale);

	SetLocalRotation(Math::degrees(dRotation));
}

auto Arg::Gameplay::Actor::GetScale() const -> Vec3
{
	Vec3 translation;
	Vec3 rotation;
	Vec3 scale;
	Math::Decompose(m_GlobalTransform, translation, rotation, scale);
	return scale;
}

void Arg::Gameplay::Actor::SetScale(const Vec3& scale)
{
	Mat4 localTransform = m_Transform.FindTransform();
	const Mat4 parentTransform = m_GlobalTransform * Math::inverse(localTransform);
	Vec3 dTranslation, dRotation, dScale;
	Math::Decompose(m_GlobalTransform, dTranslation, dRotation, dScale);

	m_GlobalTransform = Mat4(1.0f);
	m_GlobalTransform = Math::translate(m_GlobalTransform, dTranslation);
	m_GlobalTransform = Math::rotate(m_GlobalTransform, dRotation.z, Vec3(0.0f, 0.0f, 1.0f));
	m_GlobalTransform = Math::rotate(m_GlobalTransform, dRotation.y, Vec3(0.0f, 1.0f, 0.0f));
	m_GlobalTransform = Math::rotate(m_GlobalTransform, dRotation.x, Vec3(1.0f, 0.0f, 0.0f));
	m_GlobalTransform = Math::scale(m_GlobalTransform, scale);

	localTransform = Math::inverse(parentTransform) * m_GlobalTransform;
	Math::Decompose(localTransform, dTranslation, dRotation, dScale);

	SetLocalScale(dScale);
}

auto Arg::Gameplay::Actor::GetForwardVec() const -> Vec3
{
	const Vec3 rotation = GetRotation();
	return Math::normalize(Vec3(
		Math::cos(Math::radians(rotation.z)) * Math::cos(Math::radians(rotation.y)),
		Math::sin(Math::radians(rotation.z)) * Math::cos(Math::radians(rotation.y)),
		Math::sin(Math::radians(rotation.y))
	));
}

auto Arg::Gameplay::Actor::GetRightVec() const -> Vec3
{
	const Vec3 rotation = GetRotation();
	return Math::normalize(Vec3(
		-Math::cos(Math::radians(rotation.z))
		* Math::sin(Math::radians(rotation.y))
		* Math::sin(Math::radians(rotation.x))
		- Math::sin(Math::radians(rotation.z))
		* Math::sin(Math::radians(rotation.x)),

		-Math::sin(Math::radians(rotation.z))
		* Math::sin(Math::radians(rotation.y))
		* Math::sin(Math::radians(rotation.x))
		+ Math::cos(Math::radians(rotation.z))
		* Math::cos(Math::radians(rotation.x)),

		Math::cos(Math::radians(rotation.y))
		* Math::sin(Math::radians(rotation.x))
	));
}

auto Arg::Gameplay::Actor::GetUpVec() const -> Vec3
{
	const Vec3 rotation = GetRotation();
	return Math::normalize(Vec3(
		-Math::cos(Math::radians(rotation.z))
		* Math::sin(Math::radians(rotation.y))
		* Math::cos(Math::radians(rotation.x))
		+ Math::sin(Math::radians(rotation.z))
		* Math::sin(Math::radians(rotation.x)),

		-Math::sin(Math::radians(rotation.z))
		* Math::sin(Math::radians(rotation.y))
		* Math::cos(Math::radians(rotation.x))
		- Math::cos(Math::radians(rotation.z))
		* Math::sin(Math::radians(rotation.x)),

		Math::cos(Math::radians(rotation.y))
		* Math::sin(Math::radians(rotation.x))
	));
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
	node["Transform"]["Rotation"] = GetLocalRotation();
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
		const Vec3 localPosition = ValueOr<Vec3>(node["Transform"]["Position"], Vec3(0.0f));
		const Vec3 localRotation = ValueOr<Vec3>(node["Transform"]["Rotation"], Vec3(0.0f));
		const Vec3 localScale = ValueOr<Vec3>(node["Transform"]["Scale"], Vec3(1.0f));
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
