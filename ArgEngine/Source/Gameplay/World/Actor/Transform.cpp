#include <arg_pch.hpp>
#include "Transform.hpp"

Arg::Gameplay::Transform::Transform(
	const Vec3& translation,
	const Vec3& rotation,
	const Vec3& scale
)
	: m_Position(translation),
	  m_Rotation(Math::ToQuat(rotation)),
	  m_RotationEuler(rotation),
	  m_Scale(scale)
{
}

Arg::Gameplay::Transform::Transform(
	const Vec3& translation,
	const Quat& rotation,
	const Vec3& scale
)
: m_Position(translation),
	  m_Rotation(rotation),
	  m_RotationEuler(Math::ToEuler(rotation)),
	  m_Scale(scale)
{
}

void Arg::Gameplay::Transform::SetRotation(const Quat& rotation)
{
	m_Rotation = rotation;
	m_RotationEuler = Math::ToEuler(m_Rotation);
}

void Arg::Gameplay::Transform::SetRotationEuler(const Vec3& rotation)
{
	m_RotationEuler = rotation;
	m_Rotation = Math::ToQuat(m_RotationEuler);
}

auto Arg::Gameplay::Transform::FindTransform() const -> Mat4
{
	return Math::CalculateTransform(m_Position, m_Rotation, m_Scale);
}

void Arg::Gameplay::Transform::Reset()
{
	m_Position = POSITION_DEFAULT;
	m_Rotation = ROTATION_DEFAULT;
	m_RotationEuler = ROTATION_EULER_DEFAULT;
	m_Scale = SCALE_DEFAULT;
}
