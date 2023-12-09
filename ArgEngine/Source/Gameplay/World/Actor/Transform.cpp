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

auto Arg::Gameplay::Transform::FindTransform() const->Mat4
{
	Mat4 transform = Mat4(1.0f);
	transform = Math::translate(transform, m_Translation);
	transform = Math::rotate(transform, Math::radians(m_Rotation.z), Vec3(0.0f, 0.0f, 1.0f));
	transform = Math::rotate(transform, Math::radians(m_Rotation.y), Vec3(0.0f, 1.0f, 0.0f));
	transform = Math::rotate(transform, Math::radians(m_Rotation.x), Vec3(1.0f, 0.0f, 0.0f));
	transform = Math::scale(transform, m_Scale);
	return transform;
}

void Arg::Gameplay::Transform::Reset()
{
	m_Translation = Vec3(0.0f);
	m_Rotation = Vec3(0.0f);
	m_Scale = Vec3(1.0f);
}
