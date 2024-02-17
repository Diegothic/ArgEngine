#pragma once

#include <arg_pch.hpp>

#include <bullet/btBulletDynamicsCommon.h>

#include "PhysicsBody.hpp"
#include "TriggerVolume.hpp"
#include "Gameplay/World/Actor/Actor.hpp"

namespace Arg
{
	namespace Gameplay
	{
		class GameWorld;
	}

	namespace Physics
	{
		struct PhysicsRaycastResult
		{
			bool bHasHit;
			int32_t HitUserIndex;
			Vec3 HitPoint;
			Vec3 HitNormal;
		};

		class PhysicsWorld;

		class CollisionCallback : public btCollisionWorld::ContactResultCallback
		{
		public:
			CollisionCallback(const PhysicsWorld* pPhysicsWorld);

			btScalar addSingleResult(
				btManifoldPoint& cp,
				const btCollisionObjectWrapper* colObj0Wrap, int partId0, int index0,
				const btCollisionObjectWrapper* colObj1Wrap, int partId1, int index1
			) override;

		public:
			auto GetCollisionCount() const -> size_t;
			auto GetCollision(size_t index) const -> const PhysicsCollisionData&;

		private:
			const PhysicsWorld* m_pPhysicsWorld = nullptr;
			std::vector<PhysicsCollisionData> m_Collisions;
		};

		class PhysicsWorld
		{
		public:
			PhysicsWorld(Gameplay::GameWorld* pWorld);

		public:
			void Initialize();
			void Tick(float deltaTime);
			void CleanUp();

		public:
			void AddPhysicsBody(PhysicsBody* pPhysicsBody);
			void RemovePhysicsBody(PhysicsBody* pPhysicsBody);
			auto HasPhysicsBody(const PhysicsBody* pPhysicsBody) const -> bool;
			auto HasPhysicsBody(const int32_t& userIndex) const -> bool;
			auto GetPhysicsBody(const int32_t& userIndex) const -> PhysicsBody*;

			void AddTriggerVolume(TriggerVolume* pTriggerVolume);
			void RemoveTriggerVolume(TriggerVolume* pTriggerVolume);
			auto HasTriggerVolume(const TriggerVolume* pTriggerVolume) const -> bool;
			auto GetTriggerVolume(const int32_t& userIndex) const -> TriggerVolume*;

			auto IsPhysicsBody(const int32_t& userIndex) const -> bool;

			auto GetGravity() const -> const Vec3& { return m_Gravity; }
			void SetGravity(const Vec3& gravity);

		public:
			auto Raycast(
				const Vec3& source,
				const Vec3& direction,
				float distance
			) const -> PhysicsRaycastResult;
			auto Raycast(
				const Vec3& source,
				const Vec3& direction,
				float distance,
				const std::vector<GUID>& ignoreIDs
			) const -> PhysicsRaycastResult;
			auto RaycastAll(
				const Vec3& source,
				const Vec3& direction,
				float distance
			) const -> std::vector<PhysicsRaycastResult>;
			auto RaycastAll(
				const Vec3& source,
				const Vec3& direction,
				float distance,
				const std::vector<GUID>& ignoreIDs
			) const -> std::vector<PhysicsRaycastResult>;

			auto CheckSphere(const Vec3& point, float radius) const -> std::vector<int32_t>;

		public:
			static auto Convert(const Vec3& position) -> btVector3;
			static auto Convert(const btVector3& simPosition) -> Vec3;
			static auto Convert(const Quat& rotation) -> btQuaternion;
			static auto Convert(const btQuaternion& simRotation) -> Quat;

		private:
			void AddPhysicsBody(PhysicsBody* pPhysicsBody, int32_t userIndex);
			void RemovePhysicsBody(int32_t userIndex);

			void AddTriggerVolume(TriggerVolume* pTriggerVolume, int32_t userIndex);
			void RemoveTriggerVolume(int32_t userIndex);

		private:
			Gameplay::GameWorld* m_pWorld = nullptr;

			std::unique_ptr<btDefaultCollisionConfiguration> m_pCollisionConfig = nullptr;
			std::unique_ptr<btCollisionDispatcher> m_pDispatcher = nullptr;
			std::unique_ptr<btBroadphaseInterface> m_pBroadphaseInterface = nullptr;
			std::unique_ptr<btSequentialImpulseConstraintSolver> m_pConstraintSolver = nullptr;
			std::unique_ptr<btDiscreteDynamicsWorld> m_pDynamicsWorld = nullptr;

			btAlignedObjectArray<btCollisionShape*> m_CollisionShapes;
			std::unordered_map<int32_t, btTriangleMesh*> m_CustomTriangleMeshes;

			int32_t m_LastUserIndex = -1;
			std::unordered_map<int32_t, PhysicsBody*> m_PhysicsBodyLookup;
			std::vector<PhysicsBody*> m_PhysicsBodies;
			std::unordered_map<int32_t, TriggerVolume*> m_TriggerVolumeLookup;
			std::vector<TriggerVolume*> m_TriggerVolumes;

			Vec3 m_Gravity = Vec3(0.0f, 0.0f, -9.81f);
		};
	}
}
