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
	return Math::lookAtRH(
		m_Position,
		m_Position + m_Forward,
		m_Up
	);
}

void Arg::Renderer::Camera::Refresh()
{
	m_Forward = Math::ForwardVecFromRotation(
		Math::radians(m_Pitch),
		Math::radians(m_Yaw),
		Math::radians(m_Roll)
	);

	m_Right = Math::RightVecFromRotation(
		Math::radians(m_Pitch),
		Math::radians(m_Yaw),
		Math::radians(m_Roll)
	);

	m_Up = Math::UpVecFromRotation(
		Math::radians(m_Pitch),
		Math::radians(m_Yaw),
		Math::radians(m_Roll)
	);
}
