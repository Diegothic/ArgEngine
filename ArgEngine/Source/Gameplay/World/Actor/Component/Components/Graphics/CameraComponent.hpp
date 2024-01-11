#pragma once

#include <arg_pch.hpp>

#include "Gameplay/World/Actor/Component/ActorComponent.hpp"

namespace Arg
{
	namespace Gameplay
	{
		enum CameraMode
		{
			Perspective,
			Orthographic
		};

		class CameraComponent : public ActorComponent
		{
		public:
			static const GUID COMPONENT_ID;
			static const std::string COMPONENT_NAME;

		public:
			CameraComponent();

			auto VCreateDefault() -> std::shared_ptr<ActorComponent> override;

			auto VGetID() const -> GUID override { return COMPONENT_ID; }
			auto VGetName() const -> const std::string& override { return COMPONENT_NAME; }

			void VTick(const GameTime& gameTime) override;

			void VOnComponentAdded() override;
			void VOnComponentRemoved() override;

		public:
			auto GetCamera() const -> Renderer::Camera* { return m_pCamera.get(); }

			auto GetMode() const -> const CameraMode& { return m_Mode; }
			void SetMode(const CameraMode& mode);

			auto GetFOVAngle() const -> float { return m_FOVAngle; }
			void SetFOVAngle(float angle);

			auto GetSize() const -> float { return m_Size; }
			void SetSize(float size);

		protected:
			auto VOnSerialize(YAML::Node& node) const -> bool override;
			auto VOnDeserialize(const YAML::Node& node) -> bool override;

		private:
			std::unique_ptr<Renderer::Camera> m_pCamera = nullptr;

			CameraMode m_Mode = CameraMode::Perspective;

			float m_FOVAngle = 45.0f;
			float m_Size = 1.0f;
		};
	}
}
