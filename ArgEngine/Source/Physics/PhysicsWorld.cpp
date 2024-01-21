#include <arg_pch.hpp>
#include "PhysicsWorld.hpp"

#include "bullet/btBulletDynamicsCommon.h"
#include "Gameplay/World/GameWorld.hpp"

Arg::Physics::PhysicsWorld::PhysicsWorld(Gameplay::GameWorld* pWorld)
	: m_pWorld(pWorld)
{
}

void Arg::Physics::PhysicsWorld::Initialize()
{
	m_pCollisionConfig = std::make_unique<btDefaultCollisionConfiguration>();
	m_pDispatcher = std::make_unique<btCollisionDispatcher>(m_pCollisionConfig.get());
	m_pBroadphaseInterface = std::make_unique<btDbvtBroadphase>();
	m_pConstraintSolver = std::make_unique<btSequentialImpulseConstraintSolver>();

	m_pDynamicsWorld = std::make_unique<btDiscreteDynamicsWorld>(
		m_pDispatcher.get(),
		m_pBroadphaseInterface.get(),
		m_pConstraintSolver.get(),
		m_pCollisionConfig.get()
	);

	m_pDynamicsWorld->getSolverInfo().m_solverMode |= SOLVER_USE_2_FRICTION_DIRECTIONS;
	m_pDynamicsWorld->getSolverInfo().m_numIterations = 10;
	m_pDynamicsWorld->setGravity(btVector3(m_Gravity.x, m_Gravity.z, -m_Gravity.y));
}

void Arg::Physics::PhysicsWorld::Tick(float deltaTime)
{
	// Transfer transform updates to physics world
	for (int32_t i = m_pDynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
	{
		btCollisionObject* pCollisionObject = m_pDynamicsWorld->getCollisionObjectArray()[i];
		btRigidBody* pRigidBody = btRigidBody::upcast(pCollisionObject);

		const int32_t userIndex = pCollisionObject->getUserIndex();
		const GUID actorID = m_ActorIDLookup.at(userIndex);
		auto& actor = m_pWorld->GetActor(actorID);

		const Mat4& globalTransform = actor.GetTransform();
		Vec3 position, rotationEuler, scale;
		Math::Decompose(globalTransform, position, rotationEuler, scale);
		const Quat rotation = Quat(rotationEuler);
		Quat newRotation = Math::angleAxis(Math::radians(180.0f), Vec3(0.0f, 0.0f, 1.0f))
			* Math::angleAxis(Math::radians(90.0f), Vec3(1.0f, 0.0f, 0.0f))
			* rotation;

		const btVector3 simPosition(-position.x, position.z, position.y);
		btQuaternion simRotation = btQuaternion(newRotation.x, newRotation.y, newRotation.z, newRotation.w);

		if (pRigidBody && pRigidBody->getMotionState())
		{
			btTransform transform;
			pRigidBody->getMotionState()->getWorldTransform(transform);
			transform.setIdentity();
			transform.setOrigin(simPosition);
			transform.setRotation(simRotation);
			pRigidBody->setWorldTransform(transform);
			pRigidBody->getMotionState()->setWorldTransform(transform);
		}
		else
		{
			btTransform transform = pCollisionObject->getWorldTransform();
			transform.setIdentity();
			transform.setOrigin(simPosition);
			transform.setRotation(simRotation);
			pCollisionObject->setWorldTransform(transform);
		}
	}

	m_pDynamicsWorld->stepSimulation(deltaTime, 10);

	// Stabilize transforms
	for (int32_t i = m_pDynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
	{
		btCollisionObject* pCollisionObject = m_pDynamicsWorld->getCollisionObjectArray()[i];
		btRigidBody* pRigidBody = btRigidBody::upcast(pCollisionObject);
		btTransform transform;
		if (pRigidBody && pRigidBody->getMotionState())
		{
			pRigidBody->getMotionState()->getWorldTransform(transform);
		}
		else
		{
			transform = pCollisionObject->getWorldTransform();
		}

		btQuaternion simRotation = transform.getRotation();
		float simRotX, simRotY, simRotZ;
		simRotation.getEulerZYX(simRotZ, simRotY, simRotX);
		if (Math::abs(Math::degrees(simRotZ) - 90.0f) < 0.1f)
		{
			simRotZ = Math::radians(89.9f);
		}
		if (Math::abs(Math::degrees(simRotZ) + 90.0f) < 0.1f)
		{
			simRotZ = Math::radians(-89.9f);
		}
		if (Math::abs(Math::degrees(simRotY) - 90.0f) < 0.1f)
		{
			simRotY = Math::radians(89.9f);
		}
		if (Math::abs(Math::degrees(simRotY) + 90.0f) < 0.1f)
		{
			simRotY = Math::radians(-89.9f);
		}
		if (Math::abs(Math::degrees(simRotX) - 90.0f) < 0.1f)
		{
			simRotX = Math::radians(89.9f);
		}
		if (Math::abs(Math::degrees(simRotX) + 90.0f) < 0.1f)
		{
			simRotX = Math::radians(-89.9f);
		}
		simRotation.setEulerZYX(simRotZ, simRotY, simRotX);
		transform.setRotation(simRotation);
		if (pRigidBody && pRigidBody->getMotionState())
		{
			pRigidBody->getMotionState()->setWorldTransform(transform);
		}
		else
		{
			pCollisionObject->setWorldTransform(transform);
		}
	}

	// Transfer updated physics world transforms to actors
	for (int32_t i = m_pDynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
	{
		btCollisionObject* pCollisionObject = m_pDynamicsWorld->getCollisionObjectArray()[i];
		btRigidBody* pRigidBody = btRigidBody::upcast(pCollisionObject);
		btTransform transform;
		if (pRigidBody && pRigidBody->getMotionState())
		{
			pRigidBody->getMotionState()->getWorldTransform(transform);
		}
		else
		{
			transform = pCollisionObject->getWorldTransform();
		}

		const btVector3 simPosition = transform.getOrigin();
		const btQuaternion simRotation = transform.getRotation();
		const Quat rotation = Quat(
			simRotation.getW(),
			simRotation.getX(),
			simRotation.getY(),
			simRotation.getZ()
		);
		Quat newRotation = Math::angleAxis(Math::radians(-90.0f), Vec3(1.0f, 0.0f, 0.0f))
			* Math::angleAxis(Math::radians(-180.0f), Vec3(0.0f, 0.0f, 1.0f))
			* rotation;

		const Vec3 position(
			-simPosition.getX(),
			simPosition.getZ(),
			simPosition.getY()
		);
		const Vec3 rotationEuler = Math::degrees(Math::eulerAngles(newRotation));

		const int32_t userIndex = pCollisionObject->getUserIndex();
		const GUID actorID = m_ActorIDLookup.at(userIndex);
		auto& actor = m_pWorld->GetActor(actorID);
		actor.SetPosition(position);
		actor.SetRotation(rotationEuler);
	}
}

void Arg::Physics::PhysicsWorld::CleanUp()
{
	for (int32_t i = m_pDynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
	{
		btCollisionObject* pCollisionObject = m_pDynamicsWorld->getCollisionObjectArray()[i];
		btRigidBody* pRigidBody = btRigidBody::upcast(pCollisionObject);
		if (pRigidBody && pRigidBody->getMotionState())
		{
			delete pRigidBody->getMotionState();
		}

		m_pDynamicsWorld->removeCollisionObject(pCollisionObject);
		delete pCollisionObject;
	}

	for (int32_t i = m_CollisionShapes.size() - 1; i >= 0; i--)
	{
		delete m_CollisionShapes[i];
		m_CollisionShapes[i] = nullptr;
	}

	m_CollisionShapes.clear();

	m_LastUserIndex = 0;
	m_UserIndexLookup.clear();
	m_ActorIDLookup.clear();

	m_pDynamicsWorld = nullptr;
	m_pConstraintSolver = nullptr;
	m_pBroadphaseInterface = nullptr;
	m_pDispatcher = nullptr;
	m_pCollisionConfig = nullptr;
}

void Arg::Physics::PhysicsWorld::AddPhysicsBody(const PhysicsBody& physicsBody, const Mat4& transform)
{
	ARG_ASSERT(!m_UserIndexLookup.contains(physicsBody.ID), "");

	btCollisionShape* pShape = nullptr;
	switch (physicsBody.Shape)
	{
	case PhysicsBodyShape::Box:
		pShape = new btBoxShape(btVector3(
			physicsBody.Size.x * 0.5f,
			physicsBody.Size.y * 0.5f,
			physicsBody.Size.z * 0.5f
		));
		break;
	case PhysicsBodyShape::Sphere:
		pShape = new btSphereShape(physicsBody.Size.x * 0.5f);
		break;
	case PhysicsBodyShape::Capsule:
		pShape = new btCapsuleShapeZ(
			physicsBody.Size.x * 0.5f,
			physicsBody.Size.z
		);
		break;
	}

	m_CollisionShapes.push_back(pShape);

	float mass = Math::max(physicsBody.Mass, 0.0001f);
	btVector3 localInertia(0.0f, 0.0f, 0.0f);
	if (physicsBody.bIsDynamic)
	{
		pShape->calculateLocalInertia(mass, localInertia);
	}
	else
	{
		mass = 0.0f;
	}

	Vec3 position, rotationEuler, scale;
	Math::Decompose(transform, position, rotationEuler, scale);
	Quat rotation = Quat(rotationEuler);
	const Quat newRotation = Math::angleAxis(Math::radians(180.0f), Vec3(0.0f, 0.0f, 1.0f))
		* Math::angleAxis(Math::radians(90.0f), Vec3(1.0f, 0.0f, 0.0f))
		* rotation;

	const btVector3 simPosition(-position.x, position.z, position.y);
	btQuaternion simRotation = btQuaternion(newRotation.x, newRotation.y, newRotation.z, newRotation.w);

	btTransform startTransform;
	startTransform.setIdentity();
	startTransform.setOrigin(simPosition);
	startTransform.setRotation(simRotation);

	btDefaultMotionState* pMotionState = new btDefaultMotionState(startTransform);
	const btRigidBody::btRigidBodyConstructionInfo cInfo(mass, pMotionState, pShape, localInertia);
	btRigidBody* pRigidBody = new btRigidBody(cInfo);

	const int32_t userIndex = m_LastUserIndex + 1;
	m_LastUserIndex = userIndex;
	m_UserIndexLookup[physicsBody.ID] = userIndex;
	m_ActorIDLookup[userIndex] = physicsBody.ID;
	pRigidBody->setUserIndex(userIndex);

	pRigidBody->setAngularFactor(btVector3(
		physicsBody.LockRotation[0] ? 0.0f : 1.0f,
		physicsBody.LockRotation[2] ? 0.0f : 1.0f,
		physicsBody.LockRotation[1] ? 0.0f : 1.0f
	));
	pRigidBody->setLinearFactor(btVector3(
		physicsBody.LockMovement[0] ? 0.0f : 1.0f,
		physicsBody.LockMovement[2] ? 0.0f : 1.0f,
		physicsBody.LockMovement[1] ? 0.0f : 1.0f
	));

	pRigidBody->setRestitution(physicsBody.Bounciness);

	pRigidBody->setDamping(physicsBody.Damping, physicsBody.AngularDamping);

	m_pDynamicsWorld->addRigidBody(pRigidBody);
	pRigidBody->activate();
}

void Arg::Physics::PhysicsWorld::RemovePhysicsBody(GUID ID)
{
	ARG_ASSERT(m_UserIndexLookup.contains(ID), "");

	const int32_t userIndex = m_UserIndexLookup.at(ID);
	for (int32_t i = m_pDynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
	{
		btCollisionObject* pCollisionObject = m_pDynamicsWorld->getCollisionObjectArray()[i];
		btRigidBody* pRigidBody = btRigidBody::upcast(pCollisionObject);
		if (pRigidBody->getUserIndex() == userIndex)
		{
			if (pRigidBody && pRigidBody->getMotionState())
			{
				delete pRigidBody->getMotionState();
			}

			m_pDynamicsWorld->removeCollisionObject(pCollisionObject);

			btCollisionShape* pCollisionShape = pCollisionObject->getCollisionShape();
			if (pCollisionShape)
			{
				m_CollisionShapes.remove(pCollisionShape);
			}

			delete pCollisionObject;
			break;
		}
	}

	m_UserIndexLookup.erase(ID);
	m_ActorIDLookup.erase(userIndex);
}

auto Arg::Physics::PhysicsWorld::HasPhysicsBody(GUID ID) const -> bool
{
	return m_UserIndexLookup.contains(ID);
}

void Arg::Physics::PhysicsWorld::WakePhysicsBody(GUID ID) const
{
	ARG_ASSERT(m_UserIndexLookup.contains(ID), "");
	const int32_t userIndex = m_UserIndexLookup.at(ID);
	for (int32_t i = m_pDynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
	{
		btCollisionObject* pCollisionObject = m_pDynamicsWorld->getCollisionObjectArray()[i];
		btRigidBody* pRigidBody = btRigidBody::upcast(pCollisionObject);
		if (pRigidBody->getUserIndex() == userIndex)
		{
			pRigidBody->activate();
			break;
		}
	}
}

void Arg::Physics::PhysicsWorld::SetGravity(const Vec3& gravity)
{
	m_Gravity = gravity;
	m_pDynamicsWorld->setGravity(btVector3(m_Gravity.x, m_Gravity.z, -m_Gravity.y));
}
