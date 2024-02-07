#pragma once

#include <arg_pch.hpp>

#include "Core/Event/Event.hpp"
#include "Gameplay/World/Actor/ActorHandle.hpp"
#include "Gameplay/World/Actor/Component/ActorComponent.hpp"
#include "Physics/TriggerVolume.hpp"

namespace Arg
{
	namespace Gameplay
	{
		class TriggerVolumeComponent : public ActorComponent
		{
		public:
			static const GUID COMPONENT_ID;
			static const std::string COMPONENT_NAME;

		public:
			TriggerVolumeComponent();

			auto VCreateDefault() -> std::shared_ptr<ActorComponent> override;

			auto VGetID() const -> GUID override { return COMPONENT_ID; }
			auto VGetName() const -> const std::string& override { return COMPONENT_NAME; }

			void VBeginPlay() override;
			void VEndPlay() override;
			void VTick(const GameTime& gameTime, const GameInput& gameInput) override;
			void VDrawDebug(Renderer::RenderContext& context) override;

			void VOnComponentAdded() override;
			void VOnComponentRemoved() override;

			void VClone(const ActorComponent* pActorComponent) override;

		public:
			Event<void(ActorHandle)> Ev_OnEnter;
			Event<void(ActorHandle)> Ev_OnExit;

		public:
			auto GetPhysicsShape() const -> Physics::TriggerVolumeShape;
			void SetPhysicsShape(Physics::TriggerVolumeShape shape);

			auto GetSize() const -> const Vec3&;
			void SetSize(const Vec3& size);

		protected:
			auto VOnSerialize(YAML::Node& node) const -> bool override;
			auto VOnDeserialize(const YAML::Node& node) -> bool override;

		private:
			void RefreshPhysics();

		private:
			std::unique_ptr<Physics::TriggerVolume> m_pTriggerVolume = nullptr;
			bool m_bRefresh = false;
		};
	}
}
