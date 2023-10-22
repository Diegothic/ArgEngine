#pragma once

#include "Arg/Math.h"

namespace Arg
{
	struct DecomposedTransform
	{
		Vec3 Position;
		Quat Rotation;
		Vec3 Scale;
	};

	class Transform
	{
	public:
		const Mat4& GetLocalTransform() const { return m_LocalTransform; }
		const Mat4& GetGlobalTransform() const { return m_GlobalTransform; }

		Vec3 FindPosition() const;
		Quat FindRotation() const;
		Vec3 FindRotationEuler() const;
		Vec3 FindScale() const;

		void SetPosition(const Vec3& position);
		void SetRotation(const Quat& rotation);
		void SetRotation(const Vec3& rotationEuler);
		void SetScale(const Vec3& scale);

		const Vec3& GetLocalPosition() const { return m_LocalPosition; }
		const Quat& GetLocalRotation() const { return m_LocalRotation; }
		const Vec3& GetLocalRotationEuler() const { return m_LocalRotationEuler; }
		const Vec3& GetLocalScale() const { return m_LocalScale; }

		void SetLocalPosition(const Vec3& position) { m_LocalPosition = position; m_IsDirty = true; }
		void SetLocalRotation(const Quat& rotation);
		void SetLocalRotation(const Vec3& rotationEuler);
		void SetLocalScale(const Vec3& scale) { m_LocalScale = scale; m_IsDirty = true; }

		bool IsDirty() const { return m_IsDirty; }
		void SetDirty() { m_IsDirty = true; }
		void Recalculate(const Mat4& parentTransform = Mat4(1.0f));
		void ChangeParent(const Mat4& newParentTransform);

	protected:
		static DecomposedTransform Decompose(const Mat4& transform);

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
