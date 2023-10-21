#include "Transform.h"

void Arg::Transform::SetRotation(const Quat& rotation)
{
	m_LocalRotation = rotation;
	m_LocalRotationEuler = Math::degrees(Math::eulerAngles(m_LocalRotation));
	m_IsDirty = true;
}

void Arg::Transform::SetRotation(const Vec3& rotationEuler)
{
	m_LocalRotationEuler = rotationEuler;
	m_LocalRotation = Quat(Math::radians(m_LocalRotationEuler));
	m_IsDirty = true;
}

void Arg::Transform::Recalculate(const Mat4& parentTransform)
{
	m_IsDirty = false;
	const Mat4 rotation = Math::mat4_cast(m_LocalRotation);

	m_LocalTransform = Mat4(1.0f);
	m_LocalTransform = Math::translate(m_LocalTransform, m_LocalPosition);
	m_LocalTransform = m_LocalTransform * rotation;
	m_LocalTransform = Math::scale(m_LocalTransform, m_LocalScale);

	m_GlobalTransform = parentTransform * m_LocalTransform;
}

void Arg::Transform::ChangeParent(const Mat4& newParentTransform)
{
	m_LocalTransform = Math::inverse(newParentTransform) * m_GlobalTransform;

	Vec3 position;
	Vec3 scale;
	Quat rotation;
	Vec3 skew;
	Vec4 perspective;
	Math::decompose(m_LocalTransform, scale, rotation, position, skew, perspective);
	SetPosition(position);
	SetScale(scale);
	SetRotation(rotation);
}
