#include <arg_pch.hpp>
#include "Camera.hpp"


void Arg::Renderer::Camera::SetPosition(const Vec3& position)
{
	m_Position = position;
	Refresh();
}

void Arg::Renderer::Camera::SetPitch(float angle)
{
	m_Pitch = angle;
	Refresh();
}

void Arg::Renderer::Camera::SetYaw(float angle)
{
	m_Yaw = angle;
	Refresh();
}

void Arg::Renderer::Camera::SetRoll(float angle)
{
	m_Roll = angle;
	Refresh();
}

auto Arg::Renderer::Camera::GetView() const -> Mat4
{
	return Math::lookAt(
		m_Position,
		m_Position + m_Forward,
		m_Up
	);
}

auto Arg::Renderer::Camera::ScreenToWorldPoint(const Vec2& screenPoint, float distance) const -> Vec3
{
	const Mat4 view = GetView();
	const Mat4 proj = VGetProjection(m_AspectRatio);

	const Mat4 projView = proj * view;
	const Mat4 inverse = Math::inverse(projView);

	distance = Math::clamp(distance, 0.0f, (m_FarPlane - m_NearPlane));
	float z = distance / (m_FarPlane - m_NearPlane);
	z = VUnLinearizeDepth(z);

	const Vec4 worldPoint = Vec4(
		((screenPoint.x * m_AspectRatio) - (0.5f * m_AspectRatio)) / (0.5f * m_AspectRatio),
		-(screenPoint.y - 0.5f) / 0.5f,
		z,
		1.0f
	);
	Vec4 result = inverse * worldPoint;
	result /= result.w;
	return {result.x, result.y, result.z};
}

auto Arg::Renderer::Camera::WorldToScreenPoint(const Vec3& worldPoint) const -> Vec2
{
	const Mat4 view = GetView();
	const Mat4 proj = VGetProjection(m_AspectRatio);

	const Mat4 projView = proj * view;

	const Vec4 point = Vec4(
		worldPoint.x,
		worldPoint.y,
		worldPoint.z,
		1.0f
	);

	Vec4 result = projView * point;
	result /= result.w;
	const Vec2 screenPoint(
		(result.x * 0.5f) + 0.5f,
		-(result.y * 0.5f) + 0.5f
	);

	return screenPoint;
}

void Arg::Renderer::Camera::Refresh()
{
	m_Forward = Math::ForwardVecFromRotation(
		m_Pitch,
		m_Yaw,
		m_Roll
	);

	m_Right = Math::RightVecFromRotation(
		m_Pitch,
		m_Yaw,
		m_Roll
	);

	m_Up = Math::UpVecFromRotation(
		m_Pitch,
		m_Yaw,
		m_Roll
	);
}
