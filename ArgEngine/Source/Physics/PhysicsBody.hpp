#pragma once

#include <arg_pch.hpp>

#include "Content/ResourceCache.hpp"
#include "Content/Resource/GameResources/StaticModelResource.hpp"
#include "Core/GUID.hpp"
#include "Core/Math/Math.hpp"

namespace Arg
{
	using StaticModelHandle = Content::ResourceHandle<Content::StaticModelResource>;

	namespace Physics
	{
		enum PhysicsBodyShape
		{
			Box,
			Sphere,
			Capsule,
			Mesh
		};

		enum PhysicsBodyFlag
		{
			NeedsRefresh = 1 << 0,
			NeedsWake = 1 << 1,
			VelocityChanged = 1 << 2,
			HasCollided = 1 << 3,
		};

		struct PhysicsCollisionData
		{
			int32_t OtherUserIndex;
			Vec3 HitPoint;
			Vec3 HitNormal;
		};

		class PhysicsBody
		{
		public:
			PhysicsBody(const GUID& actorID);

		public:
			void Initialize(int32_t userIndex);

		public:
			auto GetActorID() const -> const GUID& { return m_ActorID; }
			auto GetUserIndex() const -> const int32_t& { return m_UserIndex; }

			auto GetGenerateOnCollisionEvents() const -> bool { return m_bGenerateCollisionEvents; }
			void SetGenerateOnCollisionEvents(bool bGenerateCollisionEvents);

			auto GetFlag(const PhysicsBodyFlag& flag) const -> bool;
			void SetFlag(const PhysicsBodyFlag& flag, bool value);

			auto GetVelocity() const -> const Vec3& { return m_Velocity; }
			void SetVelocity(const Vec3& velocity);

			void BeginCollisions();
			void AddCollision(const PhysicsCollisionData& data);
			void AddCollisionHit(size_t index);
			auto HasCollidedLastWith(const int32_t& userIndex) const -> bool;

			auto GetCollisionHitCount() const -> size_t;
			auto GetCollisionHit(size_t index) const -> const PhysicsCollisionData&;

		public:
			auto GetShape() const -> const PhysicsBodyShape& { return m_Shape; }
			void SetShape(const PhysicsBodyShape& shape);

			auto GetShapeMesh() const -> const StaticModelHandle& { return m_StaticMesh; }
			void SetShapeMesh(const StaticModelHandle& staticModel);

			auto GetSize() const -> const Vec3& { return m_Size; }
			void SetSize(const Vec3& size);

			auto GetIsDynamic() const -> bool { return m_bIsDynamic; }
			void SetIsDynamic(bool bIsDynamic);

			auto GetMass() const -> float { return m_Mass; }
			void SetMass(float mass);

			auto GetDamping() const -> float { return m_Damping; }
			void SetDamping(float damping);

			auto GetAngularDamping() const -> float { return m_AngularDamping; }
			void SetAngularDamping(float damping);

			auto GetBounciness() const -> float { return m_Bounciness; }
			void SetBounciness(float bounciness);

			auto GetLockMovement(size_t index) const -> bool { return m_LockMovement[index]; }
			void SetLockMovement(size_t index, bool value);

			auto GetLockRotation(size_t index) const -> bool { return m_LockRotation[index]; }
			void SetLockRotation(size_t index, bool value);

		private:
			int32_t m_Flags = 0;

			GUID m_ActorID = GUID::Empty;

			int32_t m_UserIndex = -1;
			Vec3 m_Velocity = Math::VEC3_ZERO;

			bool m_bGenerateCollisionEvents = false;
			std::vector<PhysicsCollisionData> m_Collisions;
			std::vector<size_t> m_CollisionHits;
			std::vector<PhysicsCollisionData> m_LastCollisions;

			PhysicsBodyShape m_Shape = PhysicsBodyShape::Box;
			StaticModelHandle m_StaticMesh;
			Vec3 m_Size = Vec3(1.0f, 1.0f, 1.0f);
			bool m_bIsDynamic = true;
			float m_Mass = 1.0f;
			float m_Damping = 0.05f;
			float m_AngularDamping = 0.05f;
			float m_Bounciness = 0.0f;
			bool m_LockMovement[3] = {false, false, false};
			bool m_LockRotation[3] = {false, false, false};
		};
	}
}
