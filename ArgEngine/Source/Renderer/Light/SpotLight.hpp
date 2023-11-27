#pragma once

#include <arg_pch.hpp>

#include "Core/Math/Math.hpp"
#include "Renderer/ShaderProgram.hpp"
#include "Renderer/Camera/Camera.hpp"

namespace Arg
{
	namespace Renderer
	{
		const int32_t SPOT_LIGHTS_MAX = 32;

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
			SpotLight(const SpotLightSpec& spec);
			~SpotLight() = default;

			auto GetPosition() const -> const Vec3& { return m_Position; }
			void SetPosition(const Vec3& position) { m_Position = position; }

			auto GetDirection() const -> const Vec3& { return m_Direction; }
			void SetDirection(const Vec3& direction) { m_Direction = Math::normalize(direction); }

			auto GetColor() const -> const Vec3& { return m_Color; }
			void SetColor(const Vec3& color) { m_Color = color; }

			auto GetRange() const -> const float& { return m_Range; }
			void SetRange(const float range) { m_Range = Math::clamp(range, 0.0f, 1000.0f); }

			auto GetIntensity() const -> const float& { return m_Intensity; }
			void SetIntensity(const float intensity) { m_Intensity = intensity; }

			void Apply(
				const std::shared_ptr<ShaderProgram>& shader,
				const std::shared_ptr<const Camera>& camera,
				const int32_t lightIndex
			) const;

		private:
			Vec3 m_Position;
			Vec3 m_Direction;
			float m_InnerConeAngle;
			float m_OuterConeAngle;
			Vec3 m_Color;
			float m_Range;
			float m_Intensity;
		};
	}
}
