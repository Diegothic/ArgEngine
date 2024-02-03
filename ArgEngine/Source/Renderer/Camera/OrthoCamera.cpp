#include <arg_pch.hpp>
#include "OrthoCamera.hpp"

auto Arg::Renderer::OrthoCamera::VGetProjection(float aspectRatio) const -> Mat4
{
	const Vec2 halfExtends = Vec2(m_Size, m_Size * 1.0f / aspectRatio) * 0.5f;
	return Math::ortho(
		-halfExtends.x,
		halfExtends.x,
		-halfExtends.y,
		halfExtends.y,
		GetNearPlane(),
		GetFarPlane()
	);
}

auto Arg::Renderer::OrthoCamera::VLinearizeDepth(float depth) const -> float
{
	// Input is [0,1], result is [N,F]
	const float d = depth * 2.0f - 1.0f;
	return (d * (GetFarPlane() - GetNearPlane()) + (GetFarPlane() + GetNearPlane())) * 0.5f;
}

auto Arg::Renderer::OrthoCamera::VUnLinearizeDepth(float depth) const -> float
{
	// Input is [0,1], result is [-1,1]
	const float L = (depth * (GetFarPlane() - GetNearPlane())) + GetNearPlane();
	return ((2.0f * L) - (GetFarPlane() + GetNearPlane())) / (GetFarPlane() - GetNearPlane());
}
