#pragma once

#include <arg_pch.hpp>

#include "Core/Math/Math.hpp"

namespace Arg
{
	namespace Gameplay
	{
		class Transform
		{
		public:
			static constexpr Vec3 POSITION_DEFAULT = Math::VEC3_ZERO;
			static constexpr Quat ROTATION_DEFAULT = Math::QUAT_ZERO;
			static constexpr Vec3 ROTATION_EULER_DEFAULT = Math::VEC3_ZERO;
			static constexpr Vec3 SCALE_DEFAULT = Math::VEC3_ONE;

		public:
			Transform() = default;
			Transform(
				const Vec3& translation,
				const Vec3& rotation,
				const Vec3& scale
			);
			Transform(
				const Vec3& translation,
				const Quat& rotation,
				const Vec3& scale
			);

			auto GetPosition() const -> const Vec3& { return m_Position; }
			void SetPosition(const Vec3& translation) { m_Position = translation; }

			auto GetRotation() const -> const Quat& { return m_Rotation; }
			void SetRotation(const Quat& rotation);

			auto GetRotationEuler() const -> const Vec3& { return m_RotationEuler; }
			void SetRotationEuler(const Vec3& rotation);

			auto GetScale() const -> const Vec3& { return m_Scale; }
			void SetScale(const Vec3& scale) { m_Scale = scale; }

			auto FindTransform() const -> Mat4;
			void Reset();

		private:
			Vec3 m_Position = POSITION_DEFAULT;
			Quat m_Rotation = ROTATION_DEFAULT;
			Vec3 m_RotationEuler = ROTATION_EULER_DEFAULT;
			Vec3 m_Scale = SCALE_DEFAULT;
		};
	}
}
