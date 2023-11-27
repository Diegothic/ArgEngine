#pragma once

#include <arg_pch.hpp>

#include "Camera.hpp"

namespace Arg
{
	namespace Renderer
	{
		class OrthoCamera : public Camera
		{
		public:
			OrthoCamera() = default;
			~OrthoCamera() override = default;

			auto GetSize() const -> const float& { return m_Size; }
			void SetSize(float size) { m_Size = size; }

			auto VGetProjection(float aspectRatio) const->Mat4 override;

		private:
			float m_Size = 1.0f;
		};
	}
}
