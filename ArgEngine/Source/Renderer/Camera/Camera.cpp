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
