#include <arg_pch.hpp>
#include "CameraComponent.hpp"

#include "Gameplay/World/Actor/Actor.hpp"
#include "Renderer/Camera/OrthoCamera.hpp"
#include "Renderer/Camera/PerspectiveCamera.hpp"

const Arg::GUID Arg::Gameplay::CameraComponent::COMPONENT_ID
	= std::hash<std::string>{}("CameraComponent");

const std::string Arg::Gameplay::CameraComponent::COMPONENT_NAME = "CameraComponent";

Arg::Gameplay::CameraComponent::CameraComponent()
{
	m_bCanEverRender = false;
}

auto Arg::Gameplay::CameraComponent::VCreateDefault() -> std::shared_ptr<ActorComponent>
{
	return std::make_shared<CameraComponent>();
}

void Arg::Gameplay::CameraComponent::VTick(const GameTime& gameTime)
{
	ActorComponent::VTick(gameTime);

	const Actor* owner = GetOwner();
	const Vec3 position = owner->GetPosition();
	const Vec3 rotation = owner->GetRotationEuler();

	m_pCamera->SetPosition(position);
	m_pCamera->SetPitch(rotation.y);
	m_pCamera->SetYaw(rotation.z);
	m_pCamera->SetRoll(rotation.x);
}

void Arg::Gameplay::CameraComponent::VDrawDebug(Renderer::RenderContext& context)
{
	ActorComponent::VDrawDebug(context);

	const Vec3& ownerPosition = GetOwner()->GetPosition();
	const Vec3& ownerRotation = GetOwner()->GetRotationEuler();
	context.DrawDebugCamera(
		ownerPosition,
		ownerRotation,
		Renderer::DEBUG_COLOR_SPECIAL
	);
}

void Arg::Gameplay::CameraComponent::VOnComponentAdded()
{
	SetMode(m_Mode);
}

void Arg::Gameplay::CameraComponent::VOnComponentRemoved()
{
}

void Arg::Gameplay::CameraComponent::SetMode(const CameraMode& mode)
{
	m_Mode = mode;
	switch (mode)
	{
	case CameraMode::Perspective:
		{
			m_pCamera = std::make_unique<Renderer::PerspectiveCamera>();
			dynamic_cast<Renderer::PerspectiveCamera*>(m_pCamera.get())->SetFOVAngle(m_FOVAngle);
			break;
		}
	case CameraMode::Orthographic:
		{
			m_pCamera = std::make_unique<Renderer::OrthoCamera>();
			dynamic_cast<Renderer::OrthoCamera*>(m_pCamera.get())->SetSize(m_Size);
			break;
		}
	}
}

void Arg::Gameplay::CameraComponent::SetFOVAngle(float angle)
{
	m_FOVAngle = angle;
	if (m_Mode == CameraMode::Perspective)
	{
		dynamic_cast<Renderer::PerspectiveCamera*>(m_pCamera.get())->SetFOVAngle(m_FOVAngle);
	}
}

void Arg::Gameplay::CameraComponent::SetSize(float size)
{
	m_Size = size;
	if (m_Mode == CameraMode::Orthographic)
	{
		dynamic_cast<Renderer::OrthoCamera*>(m_pCamera.get())->SetSize(m_Size);
	}
}

auto Arg::Gameplay::CameraComponent::ScreenToWorldPoint(const Vec2& screenPoint) const -> Vec3
{
	const Mat4 view = m_pCamera->GetView();
	const Mat4 proj = m_pCamera->VGetProjection(1.0f);

	const Mat4 projView = view * proj;
	const Mat4 inverse = Math::inverse(projView);

	const Vec4 worldPoint = Vec4(
		(screenPoint.x - 0.5f) / 0.5f,
		-1.0f * (screenPoint.y - 0.5f) / 0.5f,
		-1.0f,
		1.0f
	);
	Vec4 result = inverse * worldPoint;
	result /= result.w;
	return {result.x, result.y, result.z};
}

auto Arg::Gameplay::CameraComponent::WorldToScreenPoint(const Vec3& worldPoint) const -> Vec2
{
	const Mat4 view = m_pCamera->GetView();
	const Mat4 proj = m_pCamera->VGetProjection(1.0f);

	const Mat4 projView = view * proj;

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
		(result.y * 0.5f) + 0.5f
	);

	return screenPoint;
}

bool Arg::Gameplay::CameraComponent::VOnSerialize(YAML::Node& node) const
{
	const bool bIsSuccess = ActorComponent::VOnSerialize(node);
	if (!bIsSuccess)
	{
		return false;
	}

	node["Mode"] = static_cast<int32_t>(m_Mode);
	node["P_FOVAngle"] = m_FOVAngle;
	node["O_Size"] = m_Size;

	return true;
}

bool Arg::Gameplay::CameraComponent::VOnDeserialize(const YAML::Node& node)
{
	const bool bIsSuccess = ActorComponent::VOnDeserialize(node);
	if (!bIsSuccess)
	{
		return false;
	}

	SetMode(static_cast<CameraMode>(ValueOr<int32_t>(node["Mode"], 0)));
	SetFOVAngle(ValueOr<float>(node["P_FOVAngle"], 45.0f));
	SetSize(ValueOr<float>(node["O_Size"], 1.0f));

	return true;
}
