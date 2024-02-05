#pragma once

#include <arg_pch.hpp>

#include "Core/Event/Event.hpp"
#include "Gameplay/World/Actor/ActorHandle.hpp"
#include "Gameplay/World/Actor/Component/ActorComponent.hpp"
#include "Physics/PhysicsBody.hpp"

namespace Arg
{
	namespace Gameplay
	{
		struct CollisionData
		{
			ActorHandle OtherActor;
			Vec3 HitPoint;
			Vec3 HitNormal;
		};

		class PhysicsBodyComponent : public ActorComponent
		{
		public:
			static const GUID COMPONENT_ID;
			static const std::string COMPONENT_NAME;

		public:
			PhysicsBodyComponent();

			auto VCreateDefault() -> std::shared_ptr<ActorComponent> override;

			auto VGetID() const -> GUID override { return COMPONENT_ID; }
			auto VGetName() const -> const std::string& override { return COMPONENT_NAME; }

			void VBeginPlay() override;
			void VEndPlay() override;
			void VTick(const GameTime& gameTime, const GameInput& gameInput) override;
			void VDrawDebug(Renderer::RenderContext& context) override;

			void VOnComponentAdded() override;
			void VOnComponentRemoved() override;

		public:
			Event<void(CollisionData)> Ev_OnCollision;

		public:
			void Wake() const;

			auto GetVelocity() const -> const Vec3&;
			void SetVelocity(const Vec3& velocity);

		public:
			auto GetGeneratesOnCollisionEvents() const -> bool;
			void SetGeneratesOnCollisionEvents(bool bGenerates);

		public:
			auto GetPhysicsShape() const -> Physics::PhysicsBodyShape;
			void SetPhysicsShape(Physics::PhysicsBodyShape shape);

			auto GetPhysicsShapeMesh() const -> const StaticModelHandle&;
			void SetPhysicsShapeMesh(const StaticModelHandle& staticModel);

			auto GetSize() const -> const Vec3&;
			void SetSize(const Vec3& size);

			auto GetIsDynamic() const -> bool;
			void SetIsDynamic(bool bIsDynamic);

			auto GetMass() const -> float;
			void SetMass(float mass);

			auto GetDamping() const -> float;
			void SetDamping(float damping);

			auto GetAngularDamping() const -> float;
			void SetAngularDamping(float angularDamping);

			auto GetBounciness() const -> float;
			void SetBounciness(float bounciness);

			auto GetMovementLockX() const -> bool;
			void SetMovementLockX(bool bLockMovement);

			auto GetMovementLockY() const -> bool;
			void SetMovementLockY(bool bLockMovement);

			auto GetMovementLockZ() const -> bool;
			void SetMovementLockZ(bool bLockMovement);

			auto GetRotationLockX() const -> bool;
			void SetRotationLockX(bool bLockRotation);

			auto GetRotationLockY() const -> bool;
			void SetRotationLockY(bool bLockRotation);

			auto GetRotationLockZ() const -> bool;
			void SetRotationLockZ(bool bLockRotation);

		protected:
			auto VOnSerialize(YAML::Node& node) const -> bool override;
			auto VOnDeserialize(const YAML::Node& node) -> bool override;

		private:
			void RefreshPhysics();

		private:
			std::unique_ptr<Physics::PhysicsBody> m_pPhysicsBody = nullptr;
			bool m_bRefresh = false;
		};
	}
}
