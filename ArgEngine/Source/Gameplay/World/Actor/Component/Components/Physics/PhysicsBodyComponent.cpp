#include <arg_pch.hpp>
#include "PhysicsBodyComponent.hpp"

#include "Gameplay/World/GameWorld.hpp"
#include "Gameplay/World/Actor/Actor.hpp"
#include "Physics/PhysicsWorld.hpp"

const Arg::GUID Arg::Gameplay::PhysicsBodyComponent::COMPONENT_ID
	= std::hash<std::string>{}("PhysicsBodyComponent");

const std::string Arg::Gameplay::PhysicsBodyComponent::COMPONENT_NAME = "PhysicsBodyComponent";

Arg::Gameplay::PhysicsBodyComponent::PhysicsBodyComponent()
{
	m_bCanEverRender = false;
}

auto Arg::Gameplay::PhysicsBodyComponent::VCreateDefault() -> std::shared_ptr<ActorComponent>
{
	return std::make_shared<PhysicsBodyComponent>();
}

void Arg::Gameplay::PhysicsBodyComponent::VBeginPlay()
{
	ActorComponent::VBeginPlay();
	RefreshPhysics();
}

void Arg::Gameplay::PhysicsBodyComponent::VTick(const GameTime& gameTime)
{
	ActorComponent::VTick(gameTime);
}

void Arg::Gameplay::PhysicsBodyComponent::VOnComponentAdded()
{
	m_PhysicsBody.ID = GetOwner()->GetID();
	RefreshPhysics();
}

void Arg::Gameplay::PhysicsBodyComponent::VOnComponentRemoved()
{
	const GameWorld* pWorld = GetOwner()->GetWorld();
	Physics::PhysicsWorld* pPhysicsWorld = pWorld->GetPhysicsWorld();

	if (pPhysicsWorld != nullptr && pPhysicsWorld->HasPhysicsBody(m_PhysicsBody.ID))
	{
		pPhysicsWorld->RemovePhysicsBody(m_PhysicsBody.ID);
	}
}

auto Arg::Gameplay::PhysicsBodyComponent::GetPhysicsShape() const -> Physics::PhysicsBodyShape
{
	return m_PhysicsBody.Shape;
}

void Arg::Gameplay::PhysicsBodyComponent::SetPhysicsShape(Physics::PhysicsBodyShape shape)
{
	m_PhysicsBody.Shape = shape;
	RefreshPhysics();
}

auto Arg::Gameplay::PhysicsBodyComponent::GetSize() const -> const Vec3&
{
	return m_PhysicsBody.Size;
}

void Arg::Gameplay::PhysicsBodyComponent::SetSize(const Vec3& size)
{
	m_PhysicsBody.Size = size;
	RefreshPhysics();
}

auto Arg::Gameplay::PhysicsBodyComponent::GetIsDynamic() const -> bool
{
	return m_PhysicsBody.bIsDynamic;
}

void Arg::Gameplay::PhysicsBodyComponent::SetIsDynamic(bool bIsDynamic)
{
	m_PhysicsBody.bIsDynamic = bIsDynamic;
	RefreshPhysics();
}

auto Arg::Gameplay::PhysicsBodyComponent::GetMass() const -> float
{
	return m_PhysicsBody.Mass;
}

void Arg::Gameplay::PhysicsBodyComponent::SetMass(float mass)
{
	m_PhysicsBody.Mass = mass;
	RefreshPhysics();
}

auto Arg::Gameplay::PhysicsBodyComponent::GetDamping() const -> float
{
	return m_PhysicsBody.Damping;
}

void Arg::Gameplay::PhysicsBodyComponent::SetDamping(float damping)
{
	m_PhysicsBody.Damping = damping;
	RefreshPhysics();
}

auto Arg::Gameplay::PhysicsBodyComponent::GetAngularDamping() const -> float
{
	return m_PhysicsBody.AngularDamping;
}

void Arg::Gameplay::PhysicsBodyComponent::SetAngularDamping(float angularDamping)
{
	m_PhysicsBody.AngularDamping = angularDamping;
	RefreshPhysics();
}

auto Arg::Gameplay::PhysicsBodyComponent::GetBounciness() const -> float
{
	return m_PhysicsBody.Bounciness;
}

void Arg::Gameplay::PhysicsBodyComponent::SetBounciness(float bounciness)
{
	m_PhysicsBody.Bounciness = bounciness;
	RefreshPhysics();
}

auto Arg::Gameplay::PhysicsBodyComponent::GetMovementLockX() const -> bool
{
	return m_PhysicsBody.LockMovement[0];
}

void Arg::Gameplay::PhysicsBodyComponent::SetMovementLockX(bool bLockMovement)
{
	m_PhysicsBody.LockMovement[0] = bLockMovement;
	RefreshPhysics();
}

auto Arg::Gameplay::PhysicsBodyComponent::GetMovementLockY() const -> bool
{
	return m_PhysicsBody.LockMovement[1];
}

void Arg::Gameplay::PhysicsBodyComponent::SetMovementLockY(bool bLockMovement)
{
	m_PhysicsBody.LockMovement[1] = bLockMovement;
	RefreshPhysics();
}

auto Arg::Gameplay::PhysicsBodyComponent::GetMovementLockZ() const -> bool
{
	return m_PhysicsBody.LockMovement[2];
}

void Arg::Gameplay::PhysicsBodyComponent::SetMovementLockZ(bool bLockMovement)
{
	m_PhysicsBody.LockMovement[2] = bLockMovement;
	RefreshPhysics();
}

auto Arg::Gameplay::PhysicsBodyComponent::GetRotationLockX() const -> bool
{
	return m_PhysicsBody.LockRotation[0];
}

void Arg::Gameplay::PhysicsBodyComponent::SetRotationLockX(bool bLockRotation)
{
	m_PhysicsBody.LockRotation[0] = bLockRotation;
	RefreshPhysics();
}

auto Arg::Gameplay::PhysicsBodyComponent::GetRotationLockY() const -> bool
{
	return m_PhysicsBody.LockRotation[1];
}

void Arg::Gameplay::PhysicsBodyComponent::SetRotationLockY(bool bLockRotation)
{
	m_PhysicsBody.LockRotation[1] = bLockRotation;
	RefreshPhysics();
}

auto Arg::Gameplay::PhysicsBodyComponent::GetRotationLockZ() const -> bool
{
	return m_PhysicsBody.LockRotation[2];
}

void Arg::Gameplay::PhysicsBodyComponent::SetRotationLockZ(bool bLockRotation)
{
	m_PhysicsBody.LockRotation[2] = bLockRotation;
	RefreshPhysics();
}

void Arg::Gameplay::PhysicsBodyComponent::Wake() const
{
	const GameWorld* pWorld = GetOwner()->GetWorld();
	Physics::PhysicsWorld* pPhysicsWorld = pWorld->GetPhysicsWorld();

	if (pPhysicsWorld != nullptr && pPhysicsWorld->HasPhysicsBody(m_PhysicsBody.ID))
	{
		pPhysicsWorld->WakePhysicsBody(m_PhysicsBody.ID);
	}
}

bool Arg::Gameplay::PhysicsBodyComponent::VOnSerialize(YAML::Node& node) const
{
	const bool bIsSuccess = ActorComponent::VOnSerialize(node);
	if (!bIsSuccess)
	{
		return false;
	}

	node["IsDynamic"] = m_PhysicsBody.bIsDynamic;
	node["Mass"] = m_PhysicsBody.Mass;
	node["Damping"] = m_PhysicsBody.Damping;
	node["AngularDamping"] = m_PhysicsBody.AngularDamping;
	node["Bounciness"] = m_PhysicsBody.Bounciness;

	node["Shape"] = static_cast<int32_t>(m_PhysicsBody.Shape);
	node["Size"] = m_PhysicsBody.Size;

	auto lockMovementNode = node["LockMovement"];
	lockMovementNode["X"] = m_PhysicsBody.LockMovement[0];
	lockMovementNode["Y"] = m_PhysicsBody.LockMovement[1];
	lockMovementNode["Z"] = m_PhysicsBody.LockMovement[2];
	node["LockMovement"] = lockMovementNode;

	auto lockRotationNode = node["LockRotation"];
	lockRotationNode["X"] = m_PhysicsBody.LockRotation[0];
	lockRotationNode["Y"] = m_PhysicsBody.LockRotation[1];
	lockRotationNode["Z"] = m_PhysicsBody.LockRotation[2];
	node["LockRotation"] = lockRotationNode;

	return true;
}

bool Arg::Gameplay::PhysicsBodyComponent::VOnDeserialize(const YAML::Node& node)
{
	const bool bIsSuccess = ActorComponent::VOnDeserialize(node);
	if (!bIsSuccess)
	{
		return false;
	}

	m_PhysicsBody.bIsDynamic = ValueOr<bool>(node["IsDynamic"], true);
	m_PhysicsBody.Mass = ValueOr<float>(node["Mass"], 1.0f);
	m_PhysicsBody.Damping = ValueOr<float>(node["Damping"], 0.005f);
	m_PhysicsBody.AngularDamping = ValueOr<float>(node["AngularDamping"], 0.005f);
	m_PhysicsBody.Bounciness = ValueOr<float>(node["Bounciness"], 0.0f);

	m_PhysicsBody.Shape = static_cast<Physics::PhysicsBodyShape>(ValueOr<int32_t>(node["Shape"], 0.0f));
	m_PhysicsBody.Size = ValueOr<Vec3>(node["Size"], Vec3(1.0f));

	const auto& lockMovementNode = node["LockMovement"];
	if (lockMovementNode)
	{
		m_PhysicsBody.LockMovement[0] = ValueOr<bool>(lockMovementNode["X"], false);
		m_PhysicsBody.LockMovement[1] = ValueOr<bool>(lockMovementNode["Y"], false);
		m_PhysicsBody.LockMovement[2] = ValueOr<bool>(lockMovementNode["Z"], false);
	}

	const auto& lockRotationNode = node["LockRotation"];
	if (lockRotationNode)
	{
		m_PhysicsBody.LockRotation[0] = ValueOr<bool>(lockRotationNode["X"], false);
		m_PhysicsBody.LockRotation[1] = ValueOr<bool>(lockRotationNode["Y"], false);
		m_PhysicsBody.LockRotation[2] = ValueOr<bool>(lockRotationNode["Z"], false);
	}

	return true;
}

void Arg::Gameplay::PhysicsBodyComponent::RefreshPhysics() const
{
	const GameWorld* pWorld = GetOwner()->GetWorld();
	Physics::PhysicsWorld* pPhysicsWorld = pWorld->GetPhysicsWorld();

	if (pPhysicsWorld != nullptr)
	{
		if (pPhysicsWorld->HasPhysicsBody(m_PhysicsBody.ID))
		{
			pPhysicsWorld->RemovePhysicsBody(m_PhysicsBody.ID);
		}

		pPhysicsWorld->AddPhysicsBody(m_PhysicsBody, GetOwner()->GetTransform());
	}
}
