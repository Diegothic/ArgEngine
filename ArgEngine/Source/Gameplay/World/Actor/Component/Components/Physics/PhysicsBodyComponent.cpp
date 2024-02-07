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

	Ev_OnCollision.Clear();
	RefreshPhysics();
}

void Arg::Gameplay::PhysicsBodyComponent::VEndPlay()
{
	ActorComponent::VEndPlay();

	Ev_OnCollision.Clear();
}

void Arg::Gameplay::PhysicsBodyComponent::VTick(const GameTime& gameTime, const GameInput& gameInput)
{
	ActorComponent::VTick(gameTime, gameInput);

	GameWorld* pWorld = GetOwner()->GetWorld();
	const Physics::PhysicsWorld* pPhysicsWorld = pWorld->GetPhysicsWorld();

	if (m_pPhysicsBody->GetFlag(Physics::PhysicsBodyFlag::HasCollided))
	{
		m_pPhysicsBody->SetFlag(Physics::PhysicsBodyFlag::HasCollided, false);

		for (size_t i = 0; i < m_pPhysicsBody->GetCollisionHitCount(); i++)
		{
			const Physics::PhysicsCollisionData& collisionData = m_pPhysicsBody->GetCollisionHit(i);
			const Physics::PhysicsBody* pOtherPhysicsBody = pPhysicsWorld->GetPhysicsBody(collisionData.OtherUserIndex);
			const CollisionData eventData{
				.OtherActor = ActorHandle(pWorld, pOtherPhysicsBody->GetActorID()),
				.HitPoint = collisionData.HitPoint,
				.HitNormal = collisionData.HitNormal
			};

			Ev_OnCollision.Invoke(eventData);
		}
	}

	if (m_bRefresh)
	{
		RefreshPhysics();
	}
}

void Arg::Gameplay::PhysicsBodyComponent::VDrawDebug(Renderer::RenderContext& context)
{
	ActorComponent::VDrawDebug(context);

	const Vec3& ownerPosition = GetOwner()->GetPosition();
	const Vec3& ownerRotation = GetOwner()->GetRotationEuler();

	const Vec3& size = m_pPhysicsBody->GetSize();

	switch (m_pPhysicsBody->GetShape())
	{
	case Physics::PhysicsBodyShape::Box:
		context.DrawDebugBox(
			ownerPosition,
			ownerRotation,
			size,
			Renderer::DEBUG_COLOR_PHYSICS
		);
		break;
	case Physics::PhysicsBodyShape::Sphere:
		context.DrawDebugSphere(
			ownerPosition,
			ownerRotation,
			size.x,
			Renderer::DEBUG_COLOR_PHYSICS
		);
		break;
	case Physics::PhysicsBodyShape::Capsule:
		context.DrawDebugCapsule(
			ownerPosition,
			ownerRotation,
			size.x,
			size.z,
			Renderer::DEBUG_COLOR_PHYSICS
		);
		break;
	case Physics::PhysicsBodyShape::Mesh:
		if (m_pPhysicsBody->GetShapeMesh().IsValid())
		{
			context.DrawDebugCustomMesh(
				m_pPhysicsBody->GetShapeMesh().Get()->GetStaticModel(),
				ownerPosition,
				ownerRotation,
				m_pPhysicsBody->GetSize(),
				Renderer::DEBUG_COLOR_PHYSICS
			);
		}
		break;
	}

	context.DrawDebugLine(ownerPosition, ownerPosition + GetOwner()->GetForwardVec(), Vec3(1.0f, 0.0f, 0.0f));
	context.DrawDebugLine(ownerPosition, ownerPosition + GetOwner()->GetUpVec(), Vec3(0.0f, 0.0f, 1.0f));
	context.DrawDebugLine(ownerPosition, ownerPosition + GetOwner()->GetRightVec(), Vec3(0.0f, 1.0f, 0.0f));
}

void Arg::Gameplay::PhysicsBodyComponent::VOnComponentAdded()
{
	if (m_pPhysicsBody == nullptr)
	{
		m_pPhysicsBody = std::make_unique<Physics::PhysicsBody>(GetOwner()->GetID());
	}

	if (m_pPhysicsBody->GetShapeMesh().IsValid())
	{
		m_pPhysicsBody->GetShapeMesh().Get()->AddRef();
	}

	RefreshPhysics();
}

void Arg::Gameplay::PhysicsBodyComponent::VOnComponentRemoved()
{
	if (m_pPhysicsBody->GetShapeMesh().IsValid())
	{
		m_pPhysicsBody->GetShapeMesh().Get()->FreeRef();
	}

	const GameWorld* pWorld = GetOwner()->GetWorld();
	Physics::PhysicsWorld* pPhysicsWorld = pWorld->GetPhysicsWorld();
	if (pPhysicsWorld != nullptr
		&& pPhysicsWorld->HasPhysicsBody(m_pPhysicsBody.get()))
	{
		pPhysicsWorld->RemovePhysicsBody(m_pPhysicsBody.get());
	}
}

void Arg::Gameplay::PhysicsBodyComponent::VClone(const ActorComponent* pActorComponent)
{
	const PhysicsBodyComponent* pPhysicsBodyComponent = dynamic_cast<const PhysicsBodyComponent*>(
		pActorComponent
	);
	ARG_ASSERT(pPhysicsBodyComponent != nullptr);
	m_pPhysicsBody = std::make_unique<Physics::PhysicsBody>(GetOwner()->GetID());
	SetGeneratesOnCollisionEvents(pPhysicsBodyComponent->GetGeneratesOnCollisionEvents());
	SetIsDynamic(pPhysicsBodyComponent->GetIsDynamic());
	SetMass(pPhysicsBodyComponent->GetMass());
	SetDamping(pPhysicsBodyComponent->GetDamping());
	SetAngularDamping(pPhysicsBodyComponent->GetAngularDamping());
	SetBounciness(pPhysicsBodyComponent->GetBounciness());
	SetPhysicsShape(pPhysicsBodyComponent->GetPhysicsShape());
	SetPhysicsShapeMesh(pPhysicsBodyComponent->GetPhysicsShapeMesh());
	SetSize(pPhysicsBodyComponent->GetSize());
	SetMovementLockX(pPhysicsBodyComponent->GetMovementLockX());
	SetMovementLockY(pPhysicsBodyComponent->GetMovementLockY());
	SetMovementLockZ(pPhysicsBodyComponent->GetMovementLockZ());
	SetRotationLockX(pPhysicsBodyComponent->GetRotationLockX());
	SetRotationLockY(pPhysicsBodyComponent->GetRotationLockY());
	SetRotationLockZ(pPhysicsBodyComponent->GetRotationLockZ());
}

auto Arg::Gameplay::PhysicsBodyComponent::GetGeneratesOnCollisionEvents() const -> bool
{
	return m_pPhysicsBody->GetGenerateOnCollisionEvents();
}

void Arg::Gameplay::PhysicsBodyComponent::SetGeneratesOnCollisionEvents(bool bGenerates)
{
	m_pPhysicsBody->SetGenerateOnCollisionEvents(bGenerates);
}

auto Arg::Gameplay::PhysicsBodyComponent::GetPhysicsShape() const -> Physics::PhysicsBodyShape
{
	return m_pPhysicsBody->GetShape();
}

void Arg::Gameplay::PhysicsBodyComponent::SetPhysicsShape(Physics::PhysicsBodyShape shape)
{
	m_pPhysicsBody->SetShape(shape);
	if (shape != Physics::Mesh)
	{
		if (m_pPhysicsBody->GetShapeMesh().IsValid())
		{
			m_pPhysicsBody->GetShapeMesh().Get()->FreeRef();
		}

		m_pPhysicsBody->SetShapeMesh(
			GetResourceCache()->CreateHandle<Content::StaticModelResource>(GUID::Empty)
		);
	}
	else
	{
		SetIsDynamic(false);
	}

	m_bRefresh = true;
}

auto Arg::Gameplay::PhysicsBodyComponent::GetPhysicsShapeMesh() const -> const StaticModelHandle&
{
	return m_pPhysicsBody->GetShapeMesh();
}

void Arg::Gameplay::PhysicsBodyComponent::SetPhysicsShapeMesh(const StaticModelHandle& staticModel)
{
	if (m_pPhysicsBody->GetShapeMesh().IsValid())
	{
		m_pPhysicsBody->GetShapeMesh().Get()->FreeRef();
	}

	m_pPhysicsBody->SetShapeMesh(staticModel);

	if (m_pPhysicsBody->GetShapeMesh().IsValid())
	{
		m_pPhysicsBody->GetShapeMesh().Get()->AddRef();
	}

	m_bRefresh = true;
}

auto Arg::Gameplay::PhysicsBodyComponent::GetSize() const -> const Vec3&
{
	return m_pPhysicsBody->GetSize();
}

void Arg::Gameplay::PhysicsBodyComponent::SetSize(const Vec3& size)
{
	m_pPhysicsBody->SetSize(size);
	m_bRefresh = true;
}

auto Arg::Gameplay::PhysicsBodyComponent::GetIsDynamic() const -> bool
{
	return m_pPhysicsBody->GetIsDynamic();
}

void Arg::Gameplay::PhysicsBodyComponent::SetIsDynamic(bool bIsDynamic)
{
	m_pPhysicsBody->SetIsDynamic(bIsDynamic);
	m_bRefresh = true;
}

auto Arg::Gameplay::PhysicsBodyComponent::GetMass() const -> float
{
	return m_pPhysicsBody->GetMass();
}

void Arg::Gameplay::PhysicsBodyComponent::SetMass(float mass)
{
	m_pPhysicsBody->SetMass(mass);
	m_bRefresh = true;
}

auto Arg::Gameplay::PhysicsBodyComponent::GetDamping() const -> float
{
	return m_pPhysicsBody->GetDamping();
}

void Arg::Gameplay::PhysicsBodyComponent::SetDamping(float damping)
{
	m_pPhysicsBody->SetDamping(damping);
	m_bRefresh = true;
}

auto Arg::Gameplay::PhysicsBodyComponent::GetAngularDamping() const -> float
{
	return m_pPhysicsBody->GetAngularDamping();
}

void Arg::Gameplay::PhysicsBodyComponent::SetAngularDamping(float angularDamping)
{
	m_pPhysicsBody->SetAngularDamping(angularDamping);
	m_bRefresh = true;
}

auto Arg::Gameplay::PhysicsBodyComponent::GetBounciness() const -> float
{
	return m_pPhysicsBody->GetBounciness();
}

void Arg::Gameplay::PhysicsBodyComponent::SetBounciness(float bounciness)
{
	m_pPhysicsBody->SetBounciness(bounciness);
	m_bRefresh = true;
}

auto Arg::Gameplay::PhysicsBodyComponent::GetMovementLockX() const -> bool
{
	return m_pPhysicsBody->GetLockMovement(0);
}

void Arg::Gameplay::PhysicsBodyComponent::SetMovementLockX(bool bLockMovement)
{
	m_pPhysicsBody->SetLockMovement(0, bLockMovement);
	m_bRefresh = true;
}

auto Arg::Gameplay::PhysicsBodyComponent::GetMovementLockY() const -> bool
{
	return m_pPhysicsBody->GetLockMovement(1);
}

void Arg::Gameplay::PhysicsBodyComponent::SetMovementLockY(bool bLockMovement)
{
	m_pPhysicsBody->SetLockMovement(1, bLockMovement);
	m_bRefresh = true;
}

auto Arg::Gameplay::PhysicsBodyComponent::GetMovementLockZ() const -> bool
{
	return m_pPhysicsBody->GetLockMovement(2);
}

void Arg::Gameplay::PhysicsBodyComponent::SetMovementLockZ(bool bLockMovement)
{
	m_pPhysicsBody->SetLockMovement(2, bLockMovement);
	m_bRefresh = true;
}

auto Arg::Gameplay::PhysicsBodyComponent::GetRotationLockX() const -> bool
{
	return m_pPhysicsBody->GetLockRotation(0);
}

void Arg::Gameplay::PhysicsBodyComponent::SetRotationLockX(bool bLockRotation)
{
	m_pPhysicsBody->SetLockRotation(0, bLockRotation);
	m_bRefresh = true;
}

auto Arg::Gameplay::PhysicsBodyComponent::GetRotationLockY() const -> bool
{
	return m_pPhysicsBody->GetLockRotation(1);
}

void Arg::Gameplay::PhysicsBodyComponent::SetRotationLockY(bool bLockRotation)
{
	m_pPhysicsBody->SetLockRotation(1, bLockRotation);
	m_bRefresh = true;
}

auto Arg::Gameplay::PhysicsBodyComponent::GetRotationLockZ() const -> bool
{
	return m_pPhysicsBody->GetLockRotation(2);
}

void Arg::Gameplay::PhysicsBodyComponent::SetRotationLockZ(bool bLockRotation)
{
	m_pPhysicsBody->SetLockRotation(2, bLockRotation);
	m_bRefresh = true;
}

void Arg::Gameplay::PhysicsBodyComponent::Wake() const
{
	m_pPhysicsBody->SetFlag(Physics::PhysicsBodyFlag::NeedsWake, true);
}

auto Arg::Gameplay::PhysicsBodyComponent::GetVelocity() const -> const Vec3&
{
	return m_pPhysicsBody->GetVelocity();
}

void Arg::Gameplay::PhysicsBodyComponent::SetVelocity(const Vec3& velocity)
{
	m_pPhysicsBody->SetVelocity(velocity);
	m_pPhysicsBody->SetFlag(Physics::PhysicsBodyFlag::VelocityChanged, true);
}

bool Arg::Gameplay::PhysicsBodyComponent::VOnSerialize(YAML::Node& node) const
{
	const bool bIsSuccess = ActorComponent::VOnSerialize(node);
	if (!bIsSuccess)
	{
		return false;
	}

	node["GeneratesCollisionEvents"] = GetGeneratesOnCollisionEvents();

	node["IsDynamic"] = GetIsDynamic();
	node["Mass"] = GetMass();
	node["Damping"] = GetDamping();
	node["AngularDamping"] = GetAngularDamping();
	node["Bounciness"] = GetBounciness();

	node["Shape"] = static_cast<int32_t>(GetPhysicsShape());
	node["ShapeModelID"] = GetPhysicsShapeMesh().GetID();
	node["Size"] = GetSize();

	auto lockMovementNode = node["LockMovement"];
	lockMovementNode["X"] = GetMovementLockX();
	lockMovementNode["Y"] = GetMovementLockY();
	lockMovementNode["Z"] = GetMovementLockZ();
	node["LockMovement"] = lockMovementNode;

	auto lockRotationNode = node["LockRotation"];
	lockRotationNode["X"] = GetRotationLockX();
	lockRotationNode["Y"] = GetRotationLockY();
	lockRotationNode["Z"] = GetRotationLockZ();
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

	m_pPhysicsBody = std::make_unique<Physics::PhysicsBody>(GetOwner()->GetID());

	SetGeneratesOnCollisionEvents(ValueOr<bool>(node["GeneratesCollisionEvents"], false));

	SetIsDynamic(ValueOr<bool>(node["IsDynamic"], true));
	SetMass(ValueOr<float>(node["Mass"], 1.0f));
	SetDamping(ValueOr<float>(node["Damping"], 0.005f));
	SetAngularDamping(ValueOr<float>(node["AngularDamping"], 0.005f));
	SetBounciness(ValueOr<float>(node["Bounciness"], 0.0f));

	SetPhysicsShape(static_cast<Physics::PhysicsBodyShape>(ValueOr<int32_t>(node["Shape"], 0)));
	const GUID modelID = ValueOr<GUID>(node["ShapeModelID"], GUID::Empty);
	SetPhysicsShapeMesh(GetResourceCache()->CreateHandle<Content::StaticModelResource>(modelID));
	SetSize(ValueOr<Vec3>(node["Size"], Vec3(1.0f)));

	const auto& lockMovementNode = node["LockMovement"];
	if (lockMovementNode)
	{
		SetMovementLockX(ValueOr<bool>(lockMovementNode["X"], false));
		SetMovementLockY(ValueOr<bool>(lockMovementNode["Y"], false));
		SetMovementLockZ(ValueOr<bool>(lockMovementNode["Z"], false));
	}

	const auto& lockRotationNode = node["LockRotation"];
	if (lockRotationNode)
	{
		SetRotationLockX(ValueOr<bool>(lockRotationNode["X"], false));
		SetRotationLockY(ValueOr<bool>(lockRotationNode["Y"], false));
		SetRotationLockZ(ValueOr<bool>(lockRotationNode["Z"], false));
	}

	return true;
}

void Arg::Gameplay::PhysicsBodyComponent::RefreshPhysics()
{
	m_bRefresh = false;
	const GameWorld* pWorld = GetOwner()->GetWorld();
	Physics::PhysicsWorld* pPhysicsWorld = pWorld->GetPhysicsWorld();

	if (pPhysicsWorld != nullptr)
	{
		if (pPhysicsWorld->HasPhysicsBody(m_pPhysicsBody.get()))
		{
			pPhysicsWorld->RemovePhysicsBody(m_pPhysicsBody.get());
		}

		pPhysicsWorld->AddPhysicsBody(m_pPhysicsBody.get());
	}
}
