#pragma once

#include <arg_pch.hpp>

#include "Gameplay/World/Actor/Component/ActorComponent.hpp"
#include "Physics/PhysicsBody.hpp"

namespace Arg
{
	namespace Gameplay
	{
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
			void VTick(const GameTime& gameTime) override;
			void VDrawDebug(Renderer::RenderContext& context) override;

			void VOnComponentAdded() override;
			void VOnComponentRemoved() override;

		public:
			auto GetPhysicsShape() const -> Physics::PhysicsBodyShape;
			void SetPhysicsShape(Physics::PhysicsBodyShape shape);

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

			void Wake() const;

		protected:
			auto VOnSerialize(YAML::Node& node) const -> bool override;
			auto VOnDeserialize(const YAML::Node& node) -> bool override;

		private:
			void RefreshPhysics() const;

		private:
			Physics::PhysicsBody m_PhysicsBody;
		};
	}
}
