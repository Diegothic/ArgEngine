#include <arg_pch.hpp>
#include "PerspectiveCamera.hpp"

auto Arg::Renderer::PerspectiveCamera::VGetProjection(float aspectRatio) const -> Mat4
{
	return Math::perspective(
		Math::radians(m_FOVAngle),
		aspectRatio,
		GetNearPlane(),
		GetFarPlane()
	);
}
