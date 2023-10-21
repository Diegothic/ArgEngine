#pragma once

#include "Arg/Math.h"

namespace Arg
{
	class Transform
	{
	public:
		const Mat4& GetLocalTransform() const { return m_LocalTransform; }
		const Mat4& GetGlobalTransform() const { return m_GlobalTransform; }

		const Vec3& GetPosition() const { return m_LocalPosition; }
		const Quat& GetRotation() const { return m_LocalRotation; }
		Vec3 GetRotationEuler() const { return m_LocalRotationEuler; }
		const Vec3& GetScale() const { return m_LocalScale; }

		void SetPosition(const Vec3& position) { m_LocalPosition = position; m_IsDirty = true; }
		void SetRotation(const Quat& rotation);
		void SetRotation(const Vec3& rotationEuler);
		void SetScale(const Vec3& scale) { m_LocalScale = scale; m_IsDirty = true; }

		bool IsDirty() const { return m_IsDirty; }
		void SetDirty() { m_IsDirty = true; }
		void Recalculate(const Mat4& parentTransform = Mat4(1.0f));
		void ChangeParent(const Mat4& newParentTransform);

	private:
		Vec3 m_LocalPosition = Vec3(0.0f);
		Quat m_LocalRotation = Quat(Vec3(0.0f));
		Vec3 m_LocalRotationEuler = Vec3(0.0f);
		Vec3 m_LocalScale = Vec3(1.0f);

		Mat4 m_LocalTransform = Mat4(1.0f);
		Mat4 m_GlobalTransform = Mat4(1.0f);
		bool m_IsDirty = false;
	};
}
