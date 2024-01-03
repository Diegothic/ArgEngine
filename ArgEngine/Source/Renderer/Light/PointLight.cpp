#include <arg_pch.hpp>
#include "PointLight.hpp"
#include <Debug/Assert.hpp>

Arg::Renderer::PointLight::PointLight(const PointLightSpec& spec)
	: m_Position(spec.Position),
	  m_Color(spec.Color),
	  m_Range(Math::clamp(spec.Range, 0.0f, 1000.0f)),
	  m_Intensity(spec.Intensity)
{
}

void Arg::Renderer::PointLight::Apply(
	ShaderProgram* shader,
	const Camera* camera,
	int32_t lightIndex
) const
{
	ARG_ASSERT(lightIndex >= 0 && lightIndex <= POINT_LIGHTS_MAX, "Index out of range!");
	const std::string lightName = std::format("u_PointLights[{}]", lightIndex);
	shader->SetUniform(
		std::format("{}.properties.ambient", lightName),
		m_Color * m_Intensity * 0.2f
	);
	shader->SetUniform(
		std::format("{}.properties.diffuse", lightName),
		m_Color * m_Intensity
	);
	shader->SetUniform(
		std::format("{}.properties.specular", lightName),
		m_Color * m_Intensity
	);

	ARG_ASSERT(m_Range >= 0.0f && m_Range <= 1000.0f, "Invalid point light range!");
	int32_t lightRangeIndex = 0;
	for (auto i = 1; i < LIGHT_RANGE_STEPS_COUNT; i++)
	{
		if (m_Range <= LIGHT_RANGE_STEPS[i])
		{
			lightRangeIndex = i - 1;
			break;
		}
	}

	const float rangeA = LIGHT_RANGE_STEPS[lightRangeIndex];
	const float rangeB = LIGHT_RANGE_STEPS[lightRangeIndex + 1];
	const float diff = rangeB - rangeA;
	const float percentB = 1.0f - ((rangeB - m_Range) / diff);
	const Vec3 rangeValuesA = LIGHT_RANGE_VALUES[lightRangeIndex];
	const Vec3 rangeValuesB = LIGHT_RANGE_VALUES[lightRangeIndex + 1];
	const Vec3 rangeValues = percentB * rangeValuesB + (1.0f - percentB) * rangeValuesA;

	shader->SetUniform(std::format("{}.attenuation.constant", lightName), rangeValues.x);
	shader->SetUniform(std::format("{}.attenuation.linear", lightName), rangeValues.y);
	shader->SetUniform(std::format("{}.attenuation.quadratic", lightName), rangeValues.z);

	const Vec3 lightPositionView = Vec3(camera->GetView() * Vec4(m_Position, 1.0f));
	shader->SetUniform(std::format("{}.position", lightName), lightPositionView);
}
