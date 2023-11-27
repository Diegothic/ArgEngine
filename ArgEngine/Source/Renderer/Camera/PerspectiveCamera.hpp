#pragma once

#include <arg_pch.hpp>

#include "Camera.hpp"

namespace Arg
{
	namespace Renderer
	{
		class PerspectiveCamera : public Camera
		{
		public:
			PerspectiveCamera() = default;
			~PerspectiveCamera() override = default;

			auto GetFOVAngle() const -> const float& { return m_FOVAngle; }
			void SetFOVAngle(float angle) { m_FOVAngle = angle; }

			auto VGetProjection(float aspectRatio) const->Mat4 override;

		private:
			float m_FOVAngle = 45.0f;
		};
	}
}
