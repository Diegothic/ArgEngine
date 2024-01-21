#pragma once

#include <arg_pch.hpp>

#include <bullet/btBulletDynamicsCommon.h>

#include "Core/GUID.hpp"
#include "PhysicsBody.hpp"

namespace Arg
{
	namespace Gameplay
	{
		class GameWorld;
	}

	namespace Physics
	{
		class PhysicsWorld
		{
		public:
			PhysicsWorld(Gameplay::GameWorld* pWorld);

		public:
			void Initialize();
			void Tick(float deltaTime);
			void CleanUp();

		public:
			void AddPhysicsBody(const PhysicsBody& physicsBody, const Mat4& transform);
			void RemovePhysicsBody(GUID ID);
			auto HasPhysicsBody(GUID ID) const -> bool;
			void WakePhysicsBody(GUID ID) const;

			auto GetGravity() const -> const Vec3& {return m_Gravity;}
			void SetGravity(const Vec3& gravity);

		private:
			Gameplay::GameWorld* m_pWorld = nullptr;

			std::unique_ptr<btDefaultCollisionConfiguration> m_pCollisionConfig = nullptr;
			std::unique_ptr<btCollisionDispatcher> m_pDispatcher = nullptr;
			std::unique_ptr<btBroadphaseInterface> m_pBroadphaseInterface = nullptr;
			std::unique_ptr<btSequentialImpulseConstraintSolver> m_pConstraintSolver = nullptr;
			std::unique_ptr<btDiscreteDynamicsWorld> m_pDynamicsWorld = nullptr;

			btAlignedObjectArray<btCollisionShape*> m_CollisionShapes;
			int32_t m_LastUserIndex = 0;
			std::unordered_map<GUID, int32_t> m_UserIndexLookup;
			std::unordered_map<int32_t, GUID> m_ActorIDLookup;

			Vec3 m_Gravity = Vec3(0.0f, 0.0f, -9.81f);
		};
	}
}
