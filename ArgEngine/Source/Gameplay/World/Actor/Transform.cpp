#include <arg_pch.hpp>
#include "Transform.hpp"

Arg::Gameplay::Transform::Transform(
	const Vec3& translation,
	const Vec3& rotation,
	const Vec3& scale
)
	: m_Translation(translation),
	  m_Rotation(rotation),
	  m_Scale(scale)
{
}

auto Arg::Gameplay::Transform::FindTransform() const -> Mat4
{
	return Math::CalculateTransform(m_Translation, m_Rotation, m_Scale);
}

void Arg::Gameplay::Transform::Reset()
{
	m_Translation = Vec3(0.0f);
	m_Rotation = Vec3(0.0f);
	m_Scale = Vec3(1.0f);
}
