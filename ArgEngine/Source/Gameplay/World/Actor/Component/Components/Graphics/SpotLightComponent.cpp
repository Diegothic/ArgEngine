#include <arg_pch.hpp>
#include "SpotLightComponent.hpp"

#include "Gameplay/World/Actor/Actor.hpp"

const Arg::GUID Arg::Gameplay::SpotLightComponent::COMPONENT_ID
	= std::hash<std::string>{}("SpotLight");

const std::string Arg::Gameplay::SpotLightComponent::COMPONENT_NAME = "SpotLight";

Arg::Gameplay::SpotLightComponent::SpotLightComponent()
{
	m_bCanEverTick = false;
}

auto Arg::Gameplay::SpotLightComponent::VCreateDefault() -> std::shared_ptr<ActorComponent>
{
	return std::make_shared<SpotLightComponent>();
}

auto Arg::Gameplay::SpotLightComponent::VGetDisplayName() const -> const std::string&
{
	static const std::string displayName = "Spot Light";
	return displayName;
}

void Arg::Gameplay::SpotLightComponent::VRender(Renderer::RenderContext& context)
{
	ActorComponent::VRender(context);

	m_SpotLight.SetPosition(GetOwner()->GetPosition());
	m_SpotLight.SetDirection(GetOwner()->GetForwardVec());
	context.AddSpotLight(m_SpotLight);
}

void Arg::Gameplay::SpotLightComponent::VDrawDebug(Renderer::RenderContext& context)
{
	ActorComponent::VDrawDebug(context);

	const Vec3& ownerPosition = GetOwner()->GetPosition();
	const Vec3& ownerRotation = GetOwner()->GetRotationEuler();

	constexpr float radius = 0.3f;
	context.DrawDebugSphere(
		ownerPosition,
		ownerRotation,
		radius,
		Renderer::DEBUG_COLOR_LIGHT
	);

	context.DrawDebugCylinder(
		ownerPosition + (GetOwner()->GetForwardVec() * radius),
		Vec3(0.0f, ownerRotation.y - Math::radians(90.0f), ownerRotation.z),
		radius,
		radius * 2.0f,
		Renderer::DEBUG_COLOR_LIGHT
	);
}

void Arg::Gameplay::SpotLightComponent::VOnComponentAdded()
{
}

void Arg::Gameplay::SpotLightComponent::VOnComponentRemoved()
{
}

void Arg::Gameplay::SpotLightComponent::VClone(const ActorComponent* pActorComponent)
{
	const SpotLightComponent* pSpotLightComponent = dynamic_cast<const SpotLightComponent*>(
		pActorComponent
	);
	ARG_ASSERT(pSpotLightComponent != nullptr);
	SetLightColor(pSpotLightComponent->GetLightColor());
	SetLightRange(pSpotLightComponent->GetLightRange());
	SetLightIntensity(pSpotLightComponent->GetLightIntensity());
	SetInnerConeAngle(pSpotLightComponent->GetInnerConeAngle());
	SetOuterConeAngle(pSpotLightComponent->GetOuterConeAngle());
}

bool Arg::Gameplay::SpotLightComponent::VOnSerialize(YAML::Node& node) const
{
	const bool bIsSuccess = ActorComponent::VOnSerialize(node);
	if (!bIsSuccess)
	{
		return false;
	}

	node["Color"] = GetLightColor();
	node["Range"] = GetLightRange();
	node["Intensity"] = GetLightIntensity();
	node["InnerConeAngle"] = GetInnerConeAngle();
	node["OuterConeAngle"] = GetOuterConeAngle();

	return true;
}

bool Arg::Gameplay::SpotLightComponent::VOnDeserialize(const YAML::Node& node)
{
	const bool bIsSuccess = ActorComponent::VOnDeserialize(node);
	if (!bIsSuccess)
	{
		return false;
	}

	SetLightColor(ValueOr<Vec3>(node["Color"], Vec3(1.0f)));
	SetLightRange(ValueOr<float>(node["Range"], 0.0f));
	SetLightIntensity(ValueOr<float>(node["Intensity"], 1.0f));
	SetInnerConeAngle(ValueOr<float>(node["InnerConeAngle"], 75.0f));
	SetOuterConeAngle(ValueOr<float>(node["OuterConeAngle"], 90.0f));

	return true;
}
