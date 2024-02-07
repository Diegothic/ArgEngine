#pragma once

#include <arg_pch.hpp>

#include "Gameplay/World/Actor/Component/ActorComponent.hpp"

namespace Arg
{
	namespace Gameplay
	{
		class PointLightComponent : public ActorComponent
		{
		public:
			static const GUID COMPONENT_ID;
			static const std::string COMPONENT_NAME;

		public:
			PointLightComponent();

			auto VCreateDefault() -> std::shared_ptr<ActorComponent> override;

			auto VGetID() const -> GUID override { return COMPONENT_ID; }
			auto VGetName() const -> const std::string& override { return COMPONENT_NAME; }

			void VRender(Renderer::RenderContext& context) override;
			void VDrawDebug(Renderer::RenderContext& context) override;

			void VOnComponentAdded() override;
			void VOnComponentRemoved() override;

			void VClone(const ActorComponent* pActorComponent) override;
			
		public:
			auto GetLightColor() const -> const Vec3& { return m_PointLight.GetColor(); }
			void SetLightColor(const Vec3& color) { m_PointLight.SetColor(color); }

			auto GetLightRange() const -> float
			{
				return Math::clamp((m_PointLight.GetRange() - 8.0f) / 992.0f, 0.0f, 1.0f);
			}

			void SetLightRange(float range)
			{
				m_PointLight.SetRange(Math::clamp(range * 992.0f + 8.0f, 8.0f, 1000.0f));
			}

			auto GetLightIntensity() const -> float { return m_PointLight.GetIntensity(); }
			void SetLightIntensity(float intensity) { m_PointLight.SetIntensity(intensity); }

		protected:
			auto VOnSerialize(YAML::Node& node) const -> bool override;
			auto VOnDeserialize(const YAML::Node& node) -> bool override;

		private:
			Renderer::PointLight m_PointLight;
		};
	}
}
