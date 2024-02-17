#include <arg_pch.hpp>
#include "TriggerVolumeComponent.hpp"

#include "Gameplay/World/GameWorld.hpp"
#include "Physics/PhysicsWorld.hpp"

const Arg::GUID Arg::Gameplay::TriggerVolumeComponent::COMPONENT_ID
	= std::hash<std::string>{}("TriggerVolume");

const std::string Arg::Gameplay::TriggerVolumeComponent::COMPONENT_NAME = "TriggerVolume";

Arg::Gameplay::TriggerVolumeComponent::TriggerVolumeComponent()
{
	m_bCanEverRender = false;
}

auto Arg::Gameplay::TriggerVolumeComponent::VCreateDefault() -> std::shared_ptr<ActorComponent>
{
	return std::make_shared<TriggerVolumeComponent>();
}

auto Arg::Gameplay::TriggerVolumeComponent::VGetDisplayName() const -> const std::string&
{
	static const std::string displayName = "Trigger Volume";
	return displayName;
}

void Arg::Gameplay::TriggerVolumeComponent::VBeginPlay()
{
	ActorComponent::VBeginPlay();

	RefreshPhysics();
}

void Arg::Gameplay::TriggerVolumeComponent::VEndPlay()
{
	ActorComponent::VEndPlay();

	Ev_OnEnter.Clear();
	Ev_OnExit.Clear();
}

void Arg::Gameplay::TriggerVolumeComponent::VTick(const GameTime& gameTime, const GameInput& gameInput)
{
	ActorComponent::VTick(gameTime, gameInput);

	GameWorld* pWorld = GetOwner()->GetWorld();
	const Physics::PhysicsWorld* pPhysicsWorld = pWorld->GetPhysicsWorld();

	if (m_pTriggerVolume->GetFlag(Physics::TriggerVolumeFlag::BodyEntered))
	{
		m_pTriggerVolume->SetFlag(Physics::TriggerVolumeFlag::BodyEntered, false);

		for (size_t i = 0; i < m_pTriggerVolume->GetTriggerEnteredCount(); i++)
		{
			const int32_t& enteredBodyUserIndex = m_pTriggerVolume->GetTriggerEntered(i);
			const Physics::PhysicsBody* pOtherPhysicsBody = pPhysicsWorld->GetPhysicsBody(enteredBodyUserIndex);

			Ev_OnEnter.Invoke(ActorHandle(pWorld, pOtherPhysicsBody->GetActorID()));
		}
	}

	if (m_pTriggerVolume->GetFlag(Physics::TriggerVolumeFlag::BodyExit))
	{
		m_pTriggerVolume->SetFlag(Physics::TriggerVolumeFlag::BodyExit, false);

		for (size_t i = 0; i < m_pTriggerVolume->GetTriggerExitCount(); i++)
		{
			const int32_t& exitBodyUserIndex = m_pTriggerVolume->GetTriggerExit(i);
			if (pPhysicsWorld->HasPhysicsBody(exitBodyUserIndex))
			{
				const Physics::PhysicsBody* pOtherPhysicsBody = pPhysicsWorld->GetPhysicsBody(
					exitBodyUserIndex
				);
				Ev_OnExit.Invoke(ActorHandle(pWorld, pOtherPhysicsBody->GetActorID()));
			}
			else
			{
				Ev_OnExit.Invoke(ActorHandle(pWorld, GUID::Empty));
			}
		}
	}

	if (m_bRefresh)
	{
		RefreshPhysics();
	}
}

void Arg::Gameplay::TriggerVolumeComponent::VDrawDebug(Renderer::RenderContext& context)
{
	ActorComponent::VDrawDebug(context);

	const Vec3& ownerPosition = GetOwner()->GetPosition();
	const Vec3& ownerRotation = GetOwner()->GetRotationEuler();

	const Vec3& size = m_pTriggerVolume->GetSize();

	switch (m_pTriggerVolume->GetShape())
	{
	case Physics::TriggerVolumeShape::TBox:
		context.DrawDebugBox(
			ownerPosition,
			ownerRotation,
			size,
			Renderer::DEBUG_COLOR_PHYSICS_TRIGGER
		);
		break;
	case Physics::TriggerVolumeShape::TSphere:
		context.DrawDebugSphere(
			ownerPosition,
			ownerRotation,
			size.x,
			Renderer::DEBUG_COLOR_PHYSICS_TRIGGER
		);
		break;
	}
}

void Arg::Gameplay::TriggerVolumeComponent::VOnComponentAdded()
{
	if (m_pTriggerVolume == nullptr)
	{
		m_pTriggerVolume = std::make_unique<Physics::TriggerVolume>(GetOwner()->GetID());
	}

	RefreshPhysics();
}

void Arg::Gameplay::TriggerVolumeComponent::VOnComponentRemoved()
{
	const GameWorld* pWorld = GetOwner()->GetWorld();
	Physics::PhysicsWorld* pPhysicsWorld = pWorld->GetPhysicsWorld();
	if (pPhysicsWorld != nullptr
		&& pPhysicsWorld->HasTriggerVolume(m_pTriggerVolume.get()))
	{
		pPhysicsWorld->RemoveTriggerVolume(m_pTriggerVolume.get());
	}
}

void Arg::Gameplay::TriggerVolumeComponent::VClone(const ActorComponent* pActorComponent)
{
	const TriggerVolumeComponent* pTriggerVolumeComponent = dynamic_cast<const TriggerVolumeComponent*>(
		pActorComponent
	);
	ARG_ASSERT(pTriggerVolumeComponent != nullptr);
	m_pTriggerVolume = std::make_unique<Physics::TriggerVolume>(GetOwner()->GetID());
	SetPhysicsShape(pTriggerVolumeComponent->GetPhysicsShape());
	SetSize(pTriggerVolumeComponent->GetSize());
}

void Arg::Gameplay::TriggerVolumeComponent::Clear() const
{
	m_pTriggerVolume->Clear();
}

auto Arg::Gameplay::TriggerVolumeComponent::GetPhysicsShape() const -> Physics::TriggerVolumeShape
{
	return m_pTriggerVolume->GetShape();
}

void Arg::Gameplay::TriggerVolumeComponent::SetPhysicsShape(Physics::TriggerVolumeShape shape)
{
	m_pTriggerVolume->SetShape(shape);

	m_bRefresh = true;
}

auto Arg::Gameplay::TriggerVolumeComponent::GetSize() const -> const Vec3&
{
	return m_pTriggerVolume->GetSize();
}

void Arg::Gameplay::TriggerVolumeComponent::SetSize(const Vec3& size)
{
	m_pTriggerVolume->SetSize(size);

	m_bRefresh = true;
}

bool Arg::Gameplay::TriggerVolumeComponent::VOnSerialize(YAML::Node& node) const
{
	const bool bIsSuccess = ActorComponent::VOnSerialize(node);
	if (!bIsSuccess)
	{
		return false;
	}

	node["Shape"] = static_cast<int32_t>(GetPhysicsShape());
	node["Size"] = GetSize();

	return true;
}

bool Arg::Gameplay::TriggerVolumeComponent::VOnDeserialize(const YAML::Node& node)
{
	const bool bIsSuccess = ActorComponent::VOnDeserialize(node);
	if (!bIsSuccess)
	{
		return false;
	}

	m_pTriggerVolume = std::make_unique<Physics::TriggerVolume>(GetOwner()->GetID());

	SetPhysicsShape(static_cast<Physics::TriggerVolumeShape>(ValueOr<int32_t>(node["Shape"], 0)));
	SetSize(ValueOr<Vec3>(node["Size"], Vec3(1.0f)));

	return true;
}

void Arg::Gameplay::TriggerVolumeComponent::RefreshPhysics()
{
	m_bRefresh = false;
	const GameWorld* pWorld = GetOwner()->GetWorld();
	Physics::PhysicsWorld* pPhysicsWorld = pWorld->GetPhysicsWorld();

	if (pPhysicsWorld != nullptr)
	{
		if (pPhysicsWorld->HasTriggerVolume(m_pTriggerVolume.get()))
		{
			pPhysicsWorld->RemoveTriggerVolume(m_pTriggerVolume.get());
		}

		pPhysicsWorld->AddTriggerVolume(m_pTriggerVolume.get());
	}
}
