#pragma once

#include <arg_pch.hpp>

#include "Core/Math/Math.hpp"
#include "Renderer/ShaderProgram.hpp"
#include "Renderer/Camera/Camera.hpp"

namespace Arg
{
	namespace Renderer
	{
		const int32_t POINT_LIGHTS_MAX = 64;

		struct PointLightSpec
		{
			Vec3 Position = Vec3(0.0f);
			Vec3 Color = Vec3(1.0f);
			float Range = 1.0f;
			float Intensity = 1.0f;
		};

		const int32_t LIGHT_RANGE_STEPS_COUNT = 12;

		const float LIGHT_RANGE_STEPS[LIGHT_RANGE_STEPS_COUNT] = {
			0.0f, 7.0f, 13.0f, 20.0f,
			32.0f, 50.0f, 65.0f,
			100.0f, 160.0f, 200.0f,
			325.0f, 1000.0f
		};

		const Vec3 LIGHT_RANGE_VALUES[LIGHT_RANGE_STEPS_COUNT] = {
			Vec3(0.0f, 0.0f, 0.0f), Vec3(1.0f, 0.7f, 1.8f), Vec3(1.0f, 0.35f, 0.44f),
			Vec3(1.0f, 0.22f, 0.20f), Vec3(1.0f, 0.14f, 0.07f), Vec3(1.0f, 0.09f, 0.032f),
			Vec3(1.0f, 0.07f, 0.017f), Vec3(1.0f, 0.045f, 0.0075f), Vec3(1.0f, 0.027f, 0.0028f),
			Vec3(1.0f, 0.022f, 0.0019f), Vec3(1.0f, 0.014f, 0.0007f), Vec3(1.0f, 0.005f, 0.0001f)
		};

		class PointLight
		{
		public:
			PointLight(const PointLightSpec& spec);
			~PointLight() = default;

			auto GetPosition() const -> const Vec3& { return m_Position; }
			void SetPosition(const Vec3& position) { m_Position = position; }

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
			Vec3 m_Color;
			float m_Range;
			float m_Intensity;
		};
	}
}
