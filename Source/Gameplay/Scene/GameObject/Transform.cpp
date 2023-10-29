#include "Transform.h"

Arg::Vec3 Arg::Transform::FindPosition() const
{
	const DecomposedTransform decomposed = Decompose(m_GlobalTransform);
	return decomposed.Position;
}

Arg::Quat Arg::Transform::FindRotation() const
{
	const DecomposedTransform decomposed = Decompose(m_GlobalTransform);
	return decomposed.Rotation;
}

Arg::Vec3 Arg::Transform::FindRotationEuler() const
{
	const DecomposedTransform decomposed = Decompose(m_GlobalTransform);
	return Math::degrees(Math::eulerAngles(decomposed.Rotation));
}

Arg::Vec3 Arg::Transform::FindScale() const
{
	const DecomposedTransform decomposed = Decompose(m_GlobalTransform);
	return decomposed.Scale;
}

void Arg::Transform::SetPosition(const Vec3& position)
{
	const Mat4 parentTransform = m_GlobalTransform * Math::inverse(m_LocalTransform);
	const DecomposedTransform decomposedGlobal = Decompose(m_GlobalTransform);
	const Mat4 rotationMat = Math::mat4_cast(decomposedGlobal.Rotation);
	m_GlobalTransform = Mat4(1.0f);
	m_GlobalTransform = Math::translate(m_GlobalTransform, position);
	m_GlobalTransform = m_GlobalTransform * rotationMat;
	m_GlobalTransform = Math::scale(m_GlobalTransform, decomposedGlobal.Scale);

	m_LocalTransform = Math::inverse(parentTransform) * m_GlobalTransform;

	const DecomposedTransform decomposedLocal = Decompose(m_LocalTransform);
	SetLocalPosition(decomposedLocal.Position);
	SetLocalRotation(decomposedLocal.Rotation);
	SetLocalScale(decomposedLocal.Scale);
	m_IsDirty = true;
}

void Arg::Transform::SetRotation(const Quat& rotation)
{
	const Mat4 parentTransform = m_GlobalTransform * Math::inverse(m_LocalTransform);
	const DecomposedTransform decomposedGlobal = Decompose(m_GlobalTransform);
	const Mat4 rotationMat = Math::mat4_cast(rotation);
	m_GlobalTransform = Mat4(1.0f);
	m_GlobalTransform = Math::translate(m_GlobalTransform, decomposedGlobal.Position);
	m_GlobalTransform = m_GlobalTransform * rotationMat;
	m_GlobalTransform = Math::scale(m_GlobalTransform, decomposedGlobal.Scale);

	m_LocalTransform = Math::inverse(parentTransform) * m_GlobalTransform;

	const DecomposedTransform decomposedLocal = Decompose(m_LocalTransform);
	SetLocalPosition(decomposedLocal.Position);
	SetLocalRotation(decomposedLocal.Rotation);
	SetLocalScale(decomposedLocal.Scale);
	m_IsDirty = true;
}

void Arg::Transform::SetRotation(const Vec3& rotationEuler)
{
	SetRotation(Quat(Math::radians(rotationEuler)));
}

void Arg::Transform::SetScale(const Vec3& scale)
{
	const Mat4 parentTransform = m_GlobalTransform * Math::inverse(m_LocalTransform);
	const DecomposedTransform decomposedGlobal = Decompose(m_GlobalTransform);
	const Mat4 rotationMat = Math::mat4_cast(decomposedGlobal.Rotation);
	m_GlobalTransform = Mat4(1.0f);
	m_GlobalTransform = Math::translate(m_GlobalTransform, decomposedGlobal.Position);
	m_GlobalTransform = m_GlobalTransform * rotationMat;
	m_GlobalTransform = Math::scale(m_GlobalTransform, scale);

	m_LocalTransform = Math::inverse(parentTransform) * m_GlobalTransform;

	const DecomposedTransform decomposedLocal = Decompose(m_LocalTransform);
	SetLocalPosition(decomposedLocal.Position);
	SetLocalRotation(decomposedLocal.Rotation);
	SetLocalScale(decomposedLocal.Scale);
	m_IsDirty = true;
}

void Arg::Transform::SetLocalRotation(const Quat& rotation)
{
	m_LocalRotation = rotation;
	m_LocalRotationEuler = Math::degrees(Math::eulerAngles(m_LocalRotation));
	m_IsDirty = true;
}

void Arg::Transform::SetLocalRotation(const Vec3& rotationEuler)
{
	m_LocalRotationEuler = rotationEuler;
	m_LocalRotation = Quat(Math::radians(m_LocalRotationEuler));
	m_IsDirty = true;
}

Arg::Vec3 Arg::Transform::FindForward() const
{
	return m_LocalRotation * Transform::FORWARD;
}

Arg::Vec3 Arg::Transform::FindRight() const
{
	return m_LocalRotation * Transform::RIGHT;
}

Arg::Vec3 Arg::Transform::FindUp() const
{
	return m_LocalRotation * Transform::UP;
}

void Arg::Transform::Recalculate(const Mat4& parentTransform)
{
	m_IsDirty = false;
	const Mat4 rotationMat = Math::mat4_cast(m_LocalRotation);

	m_LocalTransform = Mat4(1.0f);
	m_LocalTransform = Math::translate(m_LocalTransform, m_LocalPosition);
	m_LocalTransform = m_LocalTransform * rotationMat;
	m_LocalTransform = Math::scale(m_LocalTransform, m_LocalScale);

	m_GlobalTransform = parentTransform * m_LocalTransform;
}

void Arg::Transform::ChangeParent(const Mat4& newParentTransform)
{
	m_LocalTransform = Math::inverse(newParentTransform) * m_GlobalTransform;

	const DecomposedTransform decomposed = Decompose(m_LocalTransform);
	SetLocalPosition(decomposed.Position);
	SetLocalRotation(decomposed.Rotation);
	SetLocalScale(decomposed.Scale);
}

Arg::DecomposedTransform Arg::Transform::Decompose(const Mat4& transform)
{
	Vec3 position;
	Quat rotation;
	Vec3 scale;
	Vec3 skew;
	Vec4 perspective;
	Math::decompose(transform, scale, rotation, position, skew, perspective);

	return { position, rotation, scale };
}
