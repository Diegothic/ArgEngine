#include <arg_pch.hpp>
#include "PhysicsBody.hpp"

Arg::Physics::PhysicsBody::PhysicsBody(const GUID& actorID)
	: m_ActorID(actorID)
{
}

void Arg::Physics::PhysicsBody::Initialize(int32_t userIndex)
{
	m_UserIndex = userIndex;
}

void Arg::Physics::PhysicsBody::SetGenerateOnCollisionEvents(bool bGenerateCollisionEvents)
{
	m_bGenerateCollisionEvents = bGenerateCollisionEvents;
}

auto Arg::Physics::PhysicsBody::GetFlag(const PhysicsBodyFlag& flag) const -> bool
{
	return m_Flags & static_cast<int32_t>(flag);
}

void Arg::Physics::PhysicsBody::SetFlag(const PhysicsBodyFlag& flag, bool value)
{
	if (value)
	{
		m_Flags |= static_cast<int32_t>(flag);
	}
	else
	{
		m_Flags &= ~static_cast<int32_t>(flag);
	}
}

void Arg::Physics::PhysicsBody::SetVelocity(const Vec3& velocity)
{
	m_Velocity = velocity;
}

void Arg::Physics::PhysicsBody::BeginCollisions()
{
	m_CollisionHits.clear();
	m_LastCollisions = m_Collisions;
	m_Collisions.clear();
}

void Arg::Physics::PhysicsBody::AddCollision(const PhysicsCollisionData& data)
{
	m_Collisions.push_back(data);
}

void Arg::Physics::PhysicsBody::AddCollisionHit(size_t index)
{
	m_CollisionHits.push_back(index);
}

auto Arg::Physics::PhysicsBody::HasCollidedLastWith(const int32_t& userIndex) const -> bool
{
	for (const auto& collision : m_LastCollisions)
	{
		if (collision.OtherUserIndex == userIndex)
		{
			return true;
		}
	}

	return false;
}

auto Arg::Physics::PhysicsBody::GetCollisionHitCount() const -> size_t
{
	return m_CollisionHits.size();
}

auto Arg::Physics::PhysicsBody::GetCollisionHit(size_t index) const -> const PhysicsCollisionData&
{
	ARG_ASSERT(index < m_CollisionHits.size());
	return m_Collisions[m_CollisionHits[index]];
}

void Arg::Physics::PhysicsBody::SetShape(const PhysicsBodyShape& shape)
{
	m_Shape = shape;
}

void Arg::Physics::PhysicsBody::SetShapeMesh(const StaticModelHandle& staticModel)
{
	m_StaticMesh = staticModel;
}

void Arg::Physics::PhysicsBody::SetSize(const Vec3& size)
{
	m_Size = size;
}

void Arg::Physics::PhysicsBody::SetIsDynamic(bool bIsDynamic)
{
	m_bIsDynamic = bIsDynamic;
}

void Arg::Physics::PhysicsBody::SetMass(float mass)
{
	m_Mass = mass;
}

void Arg::Physics::PhysicsBody::SetDamping(float damping)
{
	m_Damping = damping;
}

void Arg::Physics::PhysicsBody::SetAngularDamping(float damping)
{
	m_AngularDamping = damping;
}

void Arg::Physics::PhysicsBody::SetBounciness(float bounciness)
{
	m_Bounciness = bounciness;
}

void Arg::Physics::PhysicsBody::SetLockMovement(size_t index, bool value)
{
	ARG_ASSERT(index < 3);
	m_LockMovement[index] = value;
}

void Arg::Physics::PhysicsBody::SetLockRotation(size_t index, bool value)
{
	ARG_ASSERT(index < 3);
	m_LockRotation[index] = value;
}
