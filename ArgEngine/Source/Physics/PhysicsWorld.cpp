#include <arg_pch.hpp>
#include "PhysicsWorld.hpp"

#include "bullet/btBulletDynamicsCommon.h"
#include "Gameplay/World/GameWorld.hpp"

btScalar Arg::Physics::CollisionCallback::addSingleResult(
	btManifoldPoint& cp,
	const btCollisionObjectWrapper* colObj0Wrap, int partId0, int index0,
	const btCollisionObjectWrapper* colObj1Wrap, int partId1, int index1
)
{
	m_Collisions.emplace_back(
		colObj1Wrap->m_collisionObject->getUserIndex(),
		PhysicsWorld::Convert(cp.getPositionWorldOnB()),
		PhysicsWorld::Convert(cp.m_normalWorldOnB)
	);

	return 0.0f;
}

auto Arg::Physics::CollisionCallback::GetCollisionCount() const -> size_t
{
	return m_Collisions.size();
}

auto Arg::Physics::CollisionCallback::GetCollision(size_t index) const -> const PhysicsCollisionData&
{
	ARG_ASSERT(index < m_Collisions.size());
	return m_Collisions[index];
}

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
	m_pDynamicsWorld->setGravity(btVector3(m_Gravity.x, m_Gravity.y, m_Gravity.z));
}

void Arg::Physics::PhysicsWorld::Tick(float deltaTime)
{
	// Transfer transform updates to physics world
	for (int32_t i = m_pDynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
	{
		btCollisionObject* pCollisionObject = m_pDynamicsWorld->getCollisionObjectArray()[i];
		btRigidBody* pRigidBody = btRigidBody::upcast(pCollisionObject);

		const int32_t userIndex = pCollisionObject->getUserIndex();
		const PhysicsBody* pPhysicsBody = m_PhysicsBodyLookup.at(userIndex);

		const Gameplay::Actor& actor = m_pWorld->GetActor(pPhysicsBody->GetActorID());
		const Mat4 globalTransform = actor.GetTransform();
		Vec3 position;
		Quat rotation;
		Vec3 scale;
		Math::Decompose(globalTransform, position, rotation, scale);
		btVector3 simPosition = Convert(position);
		btQuaternion simRotation = Convert(rotation);

		btTransform transform;
		transform.setIdentity();
		transform.setOrigin(simPosition);
		transform.setRotation(simRotation);

		pCollisionObject->setWorldTransform(transform);
	}

	for (int32_t i = m_pDynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
	{
		btCollisionObject* pCollisionObject = m_pDynamicsWorld->getCollisionObjectArray()[i];
		const int32_t userIndex = pCollisionObject->getUserIndex();
		PhysicsBody* pPhysicsBody = m_PhysicsBodyLookup.at(userIndex);

		btRigidBody* pRigidBody = btRigidBody::upcast(pCollisionObject);
		if (pRigidBody == nullptr)
		{
			continue;
		}

		// Handle velocity change
		if (pPhysicsBody->GetFlag(PhysicsBodyFlag::VelocityChanged))
		{
			pPhysicsBody->SetFlag(PhysicsBodyFlag::VelocityChanged, false);
			pRigidBody->setLinearVelocity(Convert(pPhysicsBody->GetVelocity()));
			pRigidBody->activate();
		}

		// Handle wake requests
		if (pPhysicsBody->GetFlag(PhysicsBodyFlag::NeedsWake))
		{
			pPhysicsBody->SetFlag(PhysicsBodyFlag::NeedsWake, false);
			pRigidBody->activate();
		}
	}

	// Step simulation
	m_pDynamicsWorld->updateAabbs();
	m_pDynamicsWorld->stepSimulation(deltaTime, 0);

	// Transfer updated physics world transforms to actors
	for (int32_t i = m_pDynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
	{
		btCollisionObject* pCollisionObject = m_pDynamicsWorld->getCollisionObjectArray()[i];
		btRigidBody* pRigidBody = btRigidBody::upcast(pCollisionObject);
		btTransform transform;
		if (pRigidBody != nullptr)
		{
			transform = pRigidBody->getWorldTransform();
			//pRigidBody->getMotionState()->getWorldTransform(transform);
		}
		else
		{
			transform = pCollisionObject->getWorldTransform();
		}

		const btVector3 simPosition = transform.getOrigin();
		const btQuaternion simRotation = transform.getRotation();
		const Vec3 position = Convert(simPosition);
		const Quat rotation = Convert(simRotation);

		const int32_t userIndex = pCollisionObject->getUserIndex();
		PhysicsBody* pPhysicsBody = m_PhysicsBodyLookup.at(userIndex);

		Gameplay::Actor& actor = m_pWorld->GetActor(pPhysicsBody->GetActorID());
		actor.SetPosition(position);
		actor.SetRotation(rotation);

		if (pRigidBody != nullptr)
		{
			pPhysicsBody->SetVelocity(Convert(pRigidBody->getLinearVelocity()));
		}
	}

	// Handle collision events
	for (int32_t i = m_pDynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
	{
		btCollisionObject* pCollisionObject = m_pDynamicsWorld->getCollisionObjectArray()[i];
		const int32_t userIndex = pCollisionObject->getUserIndex();
		PhysicsBody* pPhysicsBody = m_PhysicsBodyLookup.at(userIndex);
		if (!pPhysicsBody->GetGenerateOnCollisionEvents())
		{
			continue;
		}

		btRigidBody* pRigidBody = btRigidBody::upcast(pCollisionObject);
		if (pRigidBody == nullptr)
		{
			continue;
		}

		CollisionCallback collisionCallback;
		m_pDynamicsWorld->contactTest(pRigidBody, collisionCallback);
		pPhysicsBody->BeginCollisions();
		for (size_t j = 0; j < collisionCallback.GetCollisionCount(); j++)
		{
			const PhysicsCollisionData& collisionData = collisionCallback.GetCollision(j);
			pPhysicsBody->AddCollision(collisionData);
			if (!pPhysicsBody->HasCollidedLastWith(collisionData.OtherUserIndex))
			{
				pPhysicsBody->AddCollisionHit(j);
				pPhysicsBody->SetFlag(PhysicsBodyFlag::HasCollided, true);
			}
		}
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

	for (int32_t i = m_CustomTriangleMeshes.size() - 1; i >= 0; i--)
	{
		delete m_CustomTriangleMeshes[i];
		m_CustomTriangleMeshes[i] = nullptr;
	}
	m_CustomTriangleMeshes.clear();

	m_LastUserIndex = 0;
	m_PhysicsBodyLookup.clear();
	m_PhysicsBodies.clear();

	m_pDynamicsWorld = nullptr;
	m_pConstraintSolver = nullptr;
	m_pBroadphaseInterface = nullptr;
	m_pDispatcher = nullptr;
	m_pCollisionConfig = nullptr;
}

void Arg::Physics::PhysicsWorld::AddPhysicsBody(PhysicsBody* pPhysicsBody)
{
	const int32_t userIndex = m_LastUserIndex + 1;
	m_LastUserIndex = userIndex;
	AddPhysicsBody(pPhysicsBody, userIndex);

	btCollisionShape* pShape = nullptr;
	{
		const Vec3 size = pPhysicsBody->GetSize();
		switch (pPhysicsBody->GetShape())
		{
		case PhysicsBodyShape::Box:
			pShape = new btBoxShape(btVector3(
				size.x * 0.5f,
				size.y * 0.5f,
				size.z * 0.5f
			));
			break;
		case PhysicsBodyShape::Sphere:
			pShape = new btSphereShape(size.x * 0.5f);
			break;
		case PhysicsBodyShape::Capsule:
			pShape = new btCapsuleShapeZ(
				size.x * 0.5f,
				size.z
			);
			break;
		case PhysicsBodyShape::Mesh:
			const StaticModelHandle& model = pPhysicsBody->GetShapeMesh();
			btTriangleMesh* pTriangleMesh = nullptr;
			if (model.IsValid())
			{
				pTriangleMesh = new btTriangleMesh();
				std::vector<Vec3> vertices = model.Get()->FindMeshData(0);
				for (size_t i = 0; i < vertices.size(); i += 3)
				{
					pTriangleMesh->addTriangle(
						Convert(vertices[i]),
						Convert(vertices[i + 1]),
						Convert(vertices[i + 2])
					);
				}
				m_CustomTriangleMeshes[userIndex] = pTriangleMesh;
			}

			if (pTriangleMesh != nullptr)
			{
				pShape = new btBvhTriangleMeshShape(pTriangleMesh, true, true);
				pShape->setLocalScaling(Convert(size));
			}
			else
			{
				pShape = new btBoxShape(btVector3(
					0.001f,
					0.001f,
					0.001f
				));
			}
			break;
		}
	}

	pShape->setMargin(0.05f);
	m_CollisionShapes.push_back(pShape);

	float mass = Math::max(pPhysicsBody->GetMass(), 0.0001f);
	btVector3 localInertia(0.0f, 0.0f, 0.0f);
	if (pPhysicsBody->GetIsDynamic()
		&& pPhysicsBody->GetShape() != PhysicsBodyShape::Mesh)
	{
		pShape->calculateLocalInertia(mass, localInertia);
	}
	else
	{
		mass = 0.0f;
	}

	const Gameplay::Actor& actor = m_pWorld->GetActor(pPhysicsBody->GetActorID());
	const Mat4 globalTransform = actor.GetTransform();
	Vec3 position;
	Quat rotation;
	Vec3 scale;
	Math::Decompose(globalTransform, position, rotation, scale);
	btVector3 simPosition = Convert(position);
	btQuaternion simRotation = Convert(rotation);

	btTransform startTransform;
	startTransform.setIdentity();
	startTransform.setOrigin(simPosition);
	startTransform.setRotation(simRotation);

	btDefaultMotionState* pMotionState = new btDefaultMotionState(startTransform);
	const btRigidBody::btRigidBodyConstructionInfo cInfo(
		mass,
		pMotionState,
		pShape,
		localInertia
	);
	btRigidBody* pRigidBody = new btRigidBody(cInfo);
	pRigidBody->setUserIndex(userIndex);

	pRigidBody->setLinearFactor(btVector3(
		pPhysicsBody->GetLockMovement(0) ? 0.0f : 1.0f,
		pPhysicsBody->GetLockMovement(1) ? 0.0f : 1.0f,
		pPhysicsBody->GetLockMovement(2) ? 0.0f : 1.0f
	));
	pRigidBody->setAngularFactor(btVector3(
		pPhysicsBody->GetLockRotation(0) ? 0.0f : 1.0f,
		pPhysicsBody->GetLockRotation(1) ? 0.0f : 1.0f,
		pPhysicsBody->GetLockRotation(2) ? 0.0f : 1.0f
	));

	pRigidBody->setRestitution(pPhysicsBody->GetBounciness());
	pRigidBody->setDamping(pPhysicsBody->GetDamping(), pPhysicsBody->GetAngularDamping());

	m_pDynamicsWorld->addRigidBody(pRigidBody);
	pRigidBody->activate();
}

void Arg::Physics::PhysicsWorld::RemovePhysicsBody(PhysicsBody* pPhysicsBody)
{
	for (int32_t i = m_pDynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
	{
		btCollisionObject* pCollisionObject = m_pDynamicsWorld->getCollisionObjectArray()[i];
		btRigidBody* pRigidBody = btRigidBody::upcast(pCollisionObject);
		const int32_t userIndex = pRigidBody->getUserIndex();
		if (userIndex == pPhysicsBody->GetUserIndex())
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

			if (m_CustomTriangleMeshes.contains(userIndex))
			{
				delete m_CustomTriangleMeshes.at(userIndex);
				m_CustomTriangleMeshes.erase(userIndex);
			}

			delete pCollisionObject;
			break;
		}
	}

	RemovePhysicsBody(pPhysicsBody->GetUserIndex());
}

auto Arg::Physics::PhysicsWorld::HasPhysicsBody(const PhysicsBody* pPhysicsBody) const -> bool
{
	return std::ranges::find(m_PhysicsBodies, pPhysicsBody) != m_PhysicsBodies.end();
}

auto Arg::Physics::PhysicsWorld::GetPhysicsBody(const int32_t& userIndex) const -> PhysicsBody*
{
	ARG_ASSERT(m_PhysicsBodyLookup.contains(userIndex));
	return m_PhysicsBodyLookup.at(userIndex);
}

void Arg::Physics::PhysicsWorld::SetGravity(const Vec3& gravity)
{
	m_Gravity = gravity;
	m_pDynamicsWorld->setGravity(btVector3(m_Gravity.x, m_Gravity.y, m_Gravity.z));
}

auto Arg::Physics::PhysicsWorld::Raycast(
	const Vec3& source,
	const Vec3& direction,
	float distance
) const -> PhysicsRaycastResult
{
	const btVector3 from = Convert(source);
	const btVector3 to = Convert(source + (Math::normalize(direction) * distance));
	btCollisionWorld::AllHitsRayResultCallback result(from, to);

	m_pDynamicsWorld->rayTest(from, to, result);
	if (!result.hasHit())
	{
		return {false, 0, Math::VEC3_ZERO, Math::VEC3_ZERO};
	}

	int32_t closestIndex = 0;
	float minFraction = result.m_hitFractions[0];
	for (int32_t i = 1; i < result.m_hitFractions.size(); i++)
	{
		if (result.m_hitFractions[i] < minFraction)
		{
			minFraction = result.m_hitFractions[i];
			closestIndex = i;
		}
	}

	return {
		true,
		result.m_collisionObjects[closestIndex]->getUserIndex(),
		Convert(result.m_hitPointWorld[closestIndex]),
		Convert(result.m_hitNormalWorld[closestIndex])
	};
}

auto Arg::Physics::PhysicsWorld::Raycast(
	const Vec3& source,
	const Vec3& direction,
	float distance,
	const std::vector<GUID>& ignoreIDs
) const -> PhysicsRaycastResult
{
	const btVector3 from = Convert(source);
	const btVector3 to = Convert(source + (Math::normalize(direction) * distance));
	btCollisionWorld::AllHitsRayResultCallback result(from, to);

	m_pDynamicsWorld->rayTest(from, to, result);
	if (!result.hasHit())
	{
		return {false, 0, Math::VEC3_ZERO, Math::VEC3_ZERO};
	}

	std::unordered_set<int32_t> ignoreUserIndices;
	ignoreUserIndices.reserve(ignoreIDs.size());
	for (const GUID& ID : ignoreIDs)
	{
		for (const PhysicsBody* pPhysicsBody : m_PhysicsBodies)
		{
			if (pPhysicsBody->GetActorID() == ID)
			{
				ignoreUserIndices.emplace(pPhysicsBody->GetUserIndex());
			}
		}
	}

	int32_t closestIndex = -1;
	float minFraction = distance;
	for (int32_t i = 0; i < result.m_hitFractions.size(); i++)
	{
		if (ignoreUserIndices.contains(result.m_collisionObjects[i]->getUserIndex()))
		{
			continue;
		}

		if (result.m_hitFractions[i] < minFraction)
		{
			minFraction = result.m_hitFractions[i];
			closestIndex = i;
		}
	}

	if (closestIndex == -1)
	{
		return {false, 0, Math::VEC3_ZERO, Math::VEC3_ZERO};
	}

	return {
		true,
		result.m_collisionObjects[closestIndex]->getUserIndex(),
		Convert(result.m_hitPointWorld[closestIndex]),
		Convert(result.m_hitNormalWorld[closestIndex])
	};
}

auto Arg::Physics::PhysicsWorld::RaycastAll(
	const Vec3& source,
	const Vec3& direction,
	float distance
) const -> std::vector<PhysicsRaycastResult>
{
	std::vector<PhysicsRaycastResult> results;
	const btVector3 from = Convert(source);
	const btVector3 to = Convert(source + (Math::normalize(direction) * distance));
	btCollisionWorld::AllHitsRayResultCallback result(from, to);

	m_pDynamicsWorld->rayTest(from, to, result);
	if (!result.hasHit())
	{
		return results;
	}


	for (int32_t i = 0; i < result.m_hitFractions.size(); i++)
	{
		results.emplace_back(
			true,
			result.m_collisionObjects[i]->getUserIndex(),
			Convert(result.m_hitPointWorld[i]),
			Convert(result.m_hitNormalWorld[i])
		);
	}

	return results;
}

auto Arg::Physics::PhysicsWorld::RaycastAll(
	const Vec3& source,
	const Vec3& direction,
	float distance,
	const std::vector<GUID>& ignoreIDs
) const -> std::vector<PhysicsRaycastResult>
{
	std::vector<PhysicsRaycastResult> results;
	const btVector3 from = Convert(source);
	const btVector3 to = Convert(source + (Math::normalize(direction) * distance));
	btCollisionWorld::AllHitsRayResultCallback result(from, to);

	m_pDynamicsWorld->rayTest(from, to, result);
	if (!result.hasHit())
	{
		return results;
	}

	std::unordered_set<int32_t> ignoreUserIndices;
	ignoreUserIndices.reserve(ignoreIDs.size());
	for (const GUID& ID : ignoreIDs)
	{
		for (const PhysicsBody* pPhysicsBody : m_PhysicsBodies)
		{
			if (pPhysicsBody->GetActorID() == ID)
			{
				ignoreUserIndices.emplace(pPhysicsBody->GetUserIndex());
			}
		}
	}

	for (int32_t i = 0; i < result.m_hitFractions.size(); i++)
	{
		if (ignoreUserIndices.contains(result.m_collisionObjects[i]->getUserIndex()))
		{
			continue;
		}

		results.emplace_back(
			true,
			result.m_collisionObjects[i]->getUserIndex(),
			Convert(result.m_hitPointWorld[i]),
			Convert(result.m_hitNormalWorld[i])
		);
	}

	return results;
}

auto Arg::Physics::PhysicsWorld::CheckSphere(
	const Vec3& point,
	float radius
) const -> std::vector<int32_t>
{
	std::vector<int32_t> result;

	btCollisionShape* shape = new btSphereShape(radius);
	btCollisionObject sphereObject;
	sphereObject.setCollisionShape(shape);
	btTransform transform;
	transform.setIdentity();
	transform.setOrigin(Convert(point));
	sphereObject.setWorldTransform(transform);

	CollisionCallback collisionCallback;
	m_pDynamicsWorld->contactTest(&sphereObject, collisionCallback);
	for (size_t j = 0; j < collisionCallback.GetCollisionCount(); j++)
	{
		const PhysicsCollisionData& collisionData = collisionCallback.GetCollision(j);
		result.push_back(collisionData.OtherUserIndex);
	}

	delete shape;

	return result;
}

auto Arg::Physics::PhysicsWorld::Convert(const Vec3& position) -> btVector3
{
	return btVector3(
		position.x,
		position.y,
		position.z
	);
}

auto Arg::Physics::PhysicsWorld::Convert(const btVector3& simPosition) -> Vec3
{
	return Vec3(
		simPosition.getX(),
		simPosition.getY(),
		simPosition.getZ()
	);
}

auto Arg::Physics::PhysicsWorld::Convert(const Quat& rotation) -> btQuaternion
{
	return btQuaternion(
		rotation.x,
		rotation.y,
		rotation.z,
		rotation.w
	);
}

auto Arg::Physics::PhysicsWorld::Convert(const btQuaternion& simRotation) -> Quat
{
	return Quat(
		simRotation.getW(),
		simRotation.getX(),
		simRotation.getY(),
		simRotation.getZ()
	);
}

void Arg::Physics::PhysicsWorld::AddPhysicsBody(PhysicsBody* pPhysicsBody, int32_t userIndex)
{
	ARG_ASSERT(!m_PhysicsBodyLookup.contains(userIndex));
	pPhysicsBody->Initialize(userIndex);
	m_PhysicsBodies.push_back(pPhysicsBody);
	m_PhysicsBodyLookup[userIndex] = pPhysicsBody;
}

void Arg::Physics::PhysicsWorld::RemovePhysicsBody(int32_t userIndex)
{
	ARG_ASSERT(m_PhysicsBodyLookup.contains(userIndex));
	PhysicsBody* pPhysicsBody = m_PhysicsBodyLookup.at(userIndex);
	std::erase(m_PhysicsBodies, pPhysicsBody);
	m_PhysicsBodyLookup.erase(userIndex);
}
