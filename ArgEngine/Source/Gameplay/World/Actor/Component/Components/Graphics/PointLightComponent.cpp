#include <arg_pch.hpp>
#include "PointLightComponent.hpp"

#include "Gameplay/World/Actor/Actor.hpp"

const Arg::GUID Arg::Gameplay::PointLightComponent::COMPONENT_ID
	= std::hash<std::string>{}("PointLightComponent");

const std::string Arg::Gameplay::PointLightComponent::COMPONENT_NAME = "PointLightComponent";

Arg::Gameplay::PointLightComponent::PointLightComponent()
{
	m_bCanEverTick = false;
}

auto Arg::Gameplay::PointLightComponent::VCreateDefault() -> std::shared_ptr<ActorComponent>
{
	return std::make_shared<PointLightComponent>();
}

void Arg::Gameplay::PointLightComponent::VRender(Renderer::RenderContext& context)
{
	ActorComponent::VRender(context);

	m_PointLight.SetPosition(GetOwner()->GetPosition());
	context.AddPointLight(m_PointLight);
}

void Arg::Gameplay::PointLightComponent::VDrawDebug(Renderer::RenderContext& context)
{
	ActorComponent::VDrawDebug(context);

	const Vec3& ownerPosition = GetOwner()->GetPosition();
	const Vec3& ownerRotation = GetOwner()->GetRotationEuler();

	context.DrawDebugSphere(
		ownerPosition,
		ownerRotation,
		0.3f,
		Renderer::DEBUG_COLOR_LIGHT
	);
}

void Arg::Gameplay::PointLightComponent::VOnComponentAdded()
{
}

void Arg::Gameplay::PointLightComponent::VOnComponentRemoved()
{
}

bool Arg::Gameplay::PointLightComponent::VOnSerialize(YAML::Node& node) const
{
	const bool bIsSuccess = ActorComponent::VOnSerialize(node);
	if (!bIsSuccess)
	{
		return false;
	}

	node["Color"] = GetLightColor();
	node["Range"] = GetLightRange();
	node["Intensity"] = GetLightIntensity();

	return true;
}

bool Arg::Gameplay::PointLightComponent::VOnDeserialize(const YAML::Node& node)
{
	const bool bIsSuccess = ActorComponent::VOnDeserialize(node);
	if (!bIsSuccess)
	{
		return false;
	}

	SetLightColor(ValueOr<Vec3>(node["Color"], Vec3(1.0f)));
	SetLightRange(ValueOr<float>(node["Range"], 0.0f));
	SetLightIntensity(ValueOr<float>(node["Intensity"], 1.0f));

	return true;
}
