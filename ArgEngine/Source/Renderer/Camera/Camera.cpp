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

auto Arg::Renderer::Camera::GetView() const -> Mat4
{
	return Math::lookAt(
		m_Position,
		m_Position + m_Forward,
		m_WorldUp
	);
}

void Arg::Renderer::Camera::Refresh()
{
	m_Forward = Math::normalize(Vec3(
		Math::cos(Math::radians(m_Yaw)) * Math::cos(Math::radians(m_Pitch)),
		Math::sin(Math::radians(m_Yaw)) * Math::cos(Math::radians(m_Pitch)),
		Math::sin(Math::radians(m_Pitch))
	));

	m_Right = Math::normalize(Math::cross(m_Forward, m_WorldUp));
	m_Up = Math::normalize(Math::cross(m_Right, m_Forward));
}
