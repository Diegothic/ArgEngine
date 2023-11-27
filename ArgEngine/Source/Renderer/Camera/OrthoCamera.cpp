#include <arg_pch.hpp>
#include "OrthoCamera.hpp"

auto Arg::Renderer::OrthoCamera::VGetProjection(float aspectRatio) const -> Mat4
{
	const Vec2 halfExtends = Vec2(aspectRatio * m_Size, m_Size) * 0.5f;
	return Math::ortho(
		-halfExtends.x,
		halfExtends.x,
		-halfExtends.y,
		halfExtends.y,
		GetNearPlane(),
		GetFarPlane()
	);
}
