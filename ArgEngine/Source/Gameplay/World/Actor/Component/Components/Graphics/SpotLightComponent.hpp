#pragma once

#include <arg_pch.hpp>

#include "Gameplay/World/Actor/Component/ActorComponent.hpp"

namespace Arg
{
	namespace Gameplay
	{
		class SpotLightComponent : public ActorComponent
		{
		public:
			static const GUID COMPONENT_ID;
			static const std::string COMPONENT_NAME;

		public:
			SpotLightComponent();

			auto VCreateDefault() -> std::shared_ptr<ActorComponent> override;

			auto VGetID() const -> GUID override { return COMPONENT_ID; }
			auto VGetName() const -> const std::string& override { return COMPONENT_NAME; }
			auto VGetDisplayName() const -> const std::string& override;

			void VRender(Renderer::RenderContext& context) override;
			void VDrawDebug(Renderer::RenderContext& context) override;

			void VOnComponentAdded() override;
			void VOnComponentRemoved() override;

			void VClone(const ActorComponent* pActorComponent) override;

		public:
			auto GetLightColor() const -> const Vec3& { return m_SpotLight.GetColor(); }
			void SetLightColor(const Vec3& color) { m_SpotLight.SetColor(color); }

			auto GetLightRange() const -> float
			{
				return Math::clamp(m_SpotLight.GetRange() / 1000.0f, 0.0f, 1.0f);
			}

			void SetLightRange(float range)
			{
				m_SpotLight.SetRange(Math::clamp(range * 993.0f + 8.0f, 8.0f, 1000.0f));
			}

			auto GetLightIntensity() const -> float { return m_SpotLight.GetIntensity(); }
			void SetLightIntensity(float intensity) { m_SpotLight.SetIntensity(intensity); }

			auto GetInnerConeAngle() const -> float { return m_SpotLight.GetInnerConeAngle(); }
			void SetInnerConeAngle(float angle) { m_SpotLight.SetInnerConeAngle(angle); }

			auto GetOuterConeAngle() const -> float { return m_SpotLight.GetOuterConeAngle(); }
			void SetOuterConeAngle(float angle) { m_SpotLight.SetOuterConeAngle(angle); }

		protected:
			auto VOnSerialize(YAML::Node& node) const -> bool override;
			auto VOnDeserialize(const YAML::Node& node) -> bool override;

		private:
			Renderer::SpotLight m_SpotLight;
		};
	}
}
