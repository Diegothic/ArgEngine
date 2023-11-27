#pragma once

#include <arg_pch.hpp>

#include "Core/Math/Math.hpp"

namespace Arg
{
	namespace Renderer
	{
		class Camera
		{
		public:
			Camera() = default;
			virtual ~Camera() = default;

			auto GetPosition() const -> const Vec3& { return m_Position; }
			void SetPosition(const Vec3& position);

			auto GetPitch() const -> const float& { return m_Pitch; }
			void SetPitch(float angle);
			auto GetYaw() const -> const float& { return m_Yaw; }
			void SetYaw(float angle);

			auto GetForwardVector() const -> const Vec3& { return m_Forward; }
			auto GetUpVector() const -> const Vec3& { return m_Up; }
			auto GetRightVector() const -> const Vec3& { return m_Right; }

			auto GetNearPlane() const -> const float& { return m_NearPlane; }
			void SetNearPlane(float nearPlane) { m_NearPlane = nearPlane; }
			auto GetFarPlane()const -> const float& { return m_FarPlane; }
			void SetFarPlane(float farPlane) { m_FarPlane = farPlane; }

			auto GetView() const->Mat4;
			virtual auto VGetProjection(float aspectRatio) const->Mat4 = 0;

		private:
			void Refresh();

		private:
			Vec3 m_Position = Vec3(0.0f);
			Vec3 m_Forward = Vec3(1.0f, 0.0f, 0.0f);
			Vec3 m_Up = Vec3(0.0f, 0.0f, 1.0f);
			Vec3 m_Right = Vec3(0.0f, 1.0f, 0.0f);
			Vec3 m_WorldUp = Vec3(0.0f, 0.0f, 1.0f);

			float m_Pitch = 0.0f;
			float m_Yaw = 0.0f;

			float m_NearPlane = 0.1f;
			float m_FarPlane = 1000.0f;
		};
	}
}
