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
			Transform() = default;
			Transform(
				const Vec3& translation,
				const Vec3& rotation,
				const Vec3& scale
			);

			auto GetTranslation() const -> const Vec3& { return m_Translation; }
			void SetTranslation(const Vec3& translation) { m_Translation = translation; }

			auto GetRotation() const -> const Vec3& { return m_Rotation; }
			void SetRotation(const Vec3& rotation) { m_Rotation = rotation; }

			auto GetScale() const -> const Vec3& { return m_Scale; }
			void SetScale(const Vec3& scale) { m_Scale = scale; }

			auto FindTransform() const->Mat4;
			void Reset();

		private:
			Vec3 m_Translation = Vec3(0.0f);
			Vec3 m_Rotation = Vec3(0.0f);
			Vec3 m_Scale = Vec3(1.0f);
		};
	}
}
