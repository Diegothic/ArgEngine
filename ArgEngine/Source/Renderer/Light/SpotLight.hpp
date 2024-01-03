#pragma once

#include <arg_pch.hpp>

#include "Core/Math/Math.hpp"
#include "Renderer/ShaderProgram.hpp"
#include "Renderer/Camera/Camera.hpp"

namespace Arg
{
	namespace Renderer
	{
		constexpr int32_t SPOT_LIGHTS_MAX = 32;

		struct SpotLightSpec
		{
			Vec3 Position = Vec3(0.0f);
			Vec3 Direction = Vec3(1.0f, 0.0f, 0.0f);
			float InnerConeAngle = 75.0f;
			float OuterConeAngle = 90.0f;
			Vec3 Color = Vec3(1.0f);
			float Range = 1.0f;
			float Intensity = 1.0f;
		};

		class SpotLight
		{
		public:
			SpotLight() = default;
			SpotLight(const SpotLightSpec& spec);
			~SpotLight() = default;

			auto GetPosition() const -> const Vec3& { return m_Position; }
			void SetPosition(const Vec3& position) { m_Position = position; }

			auto GetDirection() const -> const Vec3& { return m_Direction; }
			void SetDirection(const Vec3& direction) { m_Direction = Math::normalize(direction); }

			auto GetColor() const -> const Vec3& { return m_Color; }
			void SetColor(const Vec3& color) { m_Color = color; }

			auto GetRange() const -> float { return m_Range; }
			void SetRange(float range) { m_Range = Math::clamp(range, 0.0f, 1000.0f); }

			auto GetIntensity() const -> float { return m_Intensity; }
			void SetIntensity(float intensity) { m_Intensity = intensity; }

			auto GetInnerConeAngle() const -> float { return m_InnerConeAngle; }
			void SetInnerConeAngle(float angle) { m_InnerConeAngle = Math::min(angle, m_OuterConeAngle); }

			auto GetOuterConeAngle() const -> float { return m_OuterConeAngle; }
			void SetOuterConeAngle(float angle) { m_OuterConeAngle = Math::clamp(angle, m_InnerConeAngle, 180.0f); }

			void Apply(
				ShaderProgram* shader,
				const Camera* camera,
				int32_t lightIndex
			) const;

		private:
			Vec3 m_Position = Vec3(0.0f);
			Vec3 m_Direction = Vec3(1.0f, 0.0f, 0.0f);
			float m_InnerConeAngle = 75.0f;
			float m_OuterConeAngle = 90.0f;
			Vec3 m_Color = Vec3(1.0f);
			float m_Range = 1.0f;
			float m_Intensity = 1.0f;
		};
	}
}
