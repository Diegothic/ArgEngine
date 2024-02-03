#include <arg_pch.hpp>
#include "PerspectiveCamera.hpp"

auto Arg::Renderer::PerspectiveCamera::VGetProjection(float aspectRatio) const -> Mat4
{
	return Math::perspectiveRH(
		Math::radians(m_FOVAngle),
		aspectRatio,
		GetNearPlane(),
		GetFarPlane()
	);
}

auto Arg::Renderer::PerspectiveCamera::VLinearizeDepth(float depth) const -> float
{
	// Input is [0,1], result is [N,F]
	const float d = depth * 2.0f - 1.0f;
	return (2.0f * GetNearPlane() * GetFarPlane())
		/ (GetFarPlane() + GetNearPlane() - d * (GetFarPlane() - GetNearPlane()));
}

auto Arg::Renderer::PerspectiveCamera::VUnLinearizeDepth(float depth) const -> float
{
	// Input is [0,1], result is [-1,1]
	const float L = (depth * (GetFarPlane() - GetNearPlane())) + GetNearPlane();
	return ((-2.0f * GetNearPlane() * GetFarPlane()) / (L * (GetFarPlane() - GetNearPlane())))
		+ ((GetFarPlane() + GetNearPlane()) / (GetFarPlane() - GetNearPlane()));
}
