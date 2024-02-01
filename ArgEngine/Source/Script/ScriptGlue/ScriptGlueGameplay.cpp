#include <arg_pch.hpp>
#include "ScriptGlueGameplay.hpp"

#include "Gameplay/World/Actor/ActorHandle.hpp"
#include "Gameplay/World/Actor/Component/ActorComponentHandle.hpp"
#include "Gameplay/World/Actor/Component/Components/Graphics/PointLightComponent.hpp"
#include "Gameplay/World/Actor/Component/Components/Graphics/SpotLightComponent.hpp"

void Arg::Script::ScriptExport_Actor(const ScriptEngine& scriptEngine)
{
	auto& scriptState = scriptEngine.GetState();

	scriptState.new_usertype<Gameplay::ActorHandle>(
		"Actor",
		sol::meta_function::equal_to, sol::overload([](
			const Gameplay::ActorHandle& lhs,
			const Gameplay::ActorHandle& rhs
		) -> bool
			{
				return lhs == rhs;
			}),
		"IsValid",
		[](Gameplay::ActorHandle& self) -> bool
		{
			return self.IsValid();
		},
		"Name",
		[](Gameplay::ActorHandle& self) -> std::string
		{
			const Gameplay::Actor& actor = self.Get();
			return actor.GetName();
		},
		"Parent",
		[](Gameplay::ActorHandle& self) -> Gameplay::ActorHandle
		{
			const Gameplay::Actor& actor = self.Get();
			return Gameplay::ActorHandle(actor.GetWorld(), actor.GetParentActor()->GetID());
		},
		"Position",
		[](Gameplay::ActorHandle& self) -> Vec3
		{
			const Gameplay::Actor& actor = self.Get();
			return actor.GetPosition();
		},
		"SetPosition",
		[](Gameplay::ActorHandle& self, const Vec3& position)
		{
			Gameplay::Actor& actor = self.Get();
			return actor.SetPosition(position);
		},
		"Rotation",
		[](Gameplay::ActorHandle& self) -> Vec3
		{
			const Gameplay::Actor& actor = self.Get();
			return Math::degrees(actor.GetRotationEuler());
		},
		"SetRotation",
		[](Gameplay::ActorHandle& self, const Vec3& rotation)
		{
			Gameplay::Actor& actor = self.Get();
			return actor.SetRotationEuler(Math::radians(rotation));
		},
		"Scale",
		[](Gameplay::ActorHandle& self) -> Vec3
		{
			const Gameplay::Actor& actor = self.Get();
			return actor.GetScale();
		},
		"SetScale",
		[](Gameplay::ActorHandle& self, const Vec3& scale)
		{
			Gameplay::Actor& actor = self.Get();
			return actor.SetScale(scale);
		},
		"Forward",
		[](Gameplay::ActorHandle& self) -> Vec3
		{
			const Gameplay::Actor& actor = self.Get();
			return actor.GetForwardVec();
		},
		"Right",
		[](Gameplay::ActorHandle& self) -> Vec3
		{
			const Gameplay::Actor& actor = self.Get();
			return actor.GetRightVec();
		},
		"Up",
		[](Gameplay::ActorHandle& self) -> Vec3
		{
			const Gameplay::Actor& actor = self.Get();
			return actor.GetUpVec();
		},
		"GetComponent",
		[&](
		Gameplay::ActorHandle& self,
		const std::string& componentID,
		sol::this_state s
	) -> sol::object
		{
			const sol::state_view lua(s);

			const Gameplay::Actor& actor = self.Get();
			const GUID ownerID = actor.GetID();
			const GUID ID = std::hash<std::string>{}(componentID);
			if (!actor.HasComponent(ID))
			{
				return sol::nil;
			}

			if (componentID == Gameplay::PointLightComponent::COMPONENT_NAME)
			{
				return sol::make_object(
					lua,
					Gameplay::ActorComponentHandle<Gameplay::PointLightComponent>(
						actor.GetWorld(), ownerID, ID
					));
			}

			if (componentID == Gameplay::SpotLightComponent::COMPONENT_NAME)
			{
				return sol::make_object(
					lua,
					Gameplay::ActorComponentHandle<Gameplay::SpotLightComponent>(
						actor.GetWorld(), ownerID, ID
					));
			}

			if (scriptState[componentID].valid()
				&& scriptState[componentID]["_instances"].valid())
			{
				const std::string ownerIDStr = std::to_string(ownerID);
				return scriptState[componentID]["_instances"][ownerIDStr].get<sol::table>();
			}

			return sol::nil;
		}
	);
}

void Arg::Script::ScriptExport_ActorComponents_Graphics(const ScriptEngine& scriptEngine)
{
	auto& scriptState = scriptEngine.GetState();

	scriptState.new_usertype<Gameplay::ActorComponentHandle<Gameplay::PointLightComponent>>(
		"PointLight",
		sol::meta_function::equal_to, sol::overload([](
			const Gameplay::ActorComponentHandle<Gameplay::PointLightComponent>& lhs,
			const Gameplay::ActorComponentHandle<Gameplay::PointLightComponent>& rhs
		) -> bool
			{
				return lhs == rhs;
			}),
		"Color",
		[](Gameplay::ActorComponentHandle<Gameplay::PointLightComponent>& self) -> Vec3
		{
			const Gameplay::PointLightComponent& component = self.Get();
			return component.GetLightColor();
		},
		"SetColor",
		[](Gameplay::ActorComponentHandle<Gameplay::PointLightComponent>& self, const Vec3& color)
		{
			Gameplay::PointLightComponent& component = self.Get();
			return component.SetLightColor(color);
		},
		"Range",
		[](Gameplay::ActorComponentHandle<Gameplay::PointLightComponent>& self) -> float
		{
			const Gameplay::PointLightComponent& component = self.Get();
			return component.GetLightRange();
		},
		"SetRange",
		[](Gameplay::ActorComponentHandle<Gameplay::PointLightComponent>& self, float range)
		{
			Gameplay::PointLightComponent& component = self.Get();
			return component.SetLightRange(range);
		},
		"Intensity",
		[](Gameplay::ActorComponentHandle<Gameplay::PointLightComponent>& self) -> float
		{
			const Gameplay::PointLightComponent& component = self.Get();
			return component.GetLightIntensity();
		},
		"SetIntensity",
		[](Gameplay::ActorComponentHandle<Gameplay::PointLightComponent>& self, float intensity)
		{
			Gameplay::PointLightComponent& component = self.Get();
			return component.SetLightIntensity(intensity);
		}
	);

	scriptState.new_usertype<Gameplay::ActorComponentHandle<Gameplay::SpotLightComponent>>(
		"SpotLight",
		sol::meta_function::equal_to, sol::overload([](
			const Gameplay::ActorComponentHandle<Gameplay::SpotLightComponent>& lhs,
			const Gameplay::ActorComponentHandle<Gameplay::SpotLightComponent>& rhs
		) -> bool
			{
				return lhs == rhs;
			}),
		"Color",
		[](Gameplay::ActorComponentHandle<Gameplay::SpotLightComponent>& self) -> Vec3
		{
			const Gameplay::SpotLightComponent& component = self.Get();
			return component.GetLightColor();
		},
		"SetColor",
		[](Gameplay::ActorComponentHandle<Gameplay::SpotLightComponent>& self, const Vec3& color)
		{
			Gameplay::SpotLightComponent& component = self.Get();
			return component.SetLightColor(color);
		},
		"Range",
		[](Gameplay::ActorComponentHandle<Gameplay::SpotLightComponent>& self) -> float
		{
			const Gameplay::SpotLightComponent& component = self.Get();
			return component.GetLightRange();
		},
		"SetRange",
		[](Gameplay::ActorComponentHandle<Gameplay::SpotLightComponent>& self, float range)
		{
			Gameplay::SpotLightComponent& component = self.Get();
			return component.SetLightRange(range);
		},
		"Intensity",
		[](Gameplay::ActorComponentHandle<Gameplay::SpotLightComponent>& self) -> float
		{
			const Gameplay::SpotLightComponent& component = self.Get();
			return component.GetLightIntensity();
		},
		"SetIntensity",
		[](Gameplay::ActorComponentHandle<Gameplay::SpotLightComponent>& self, float intensity)
		{
			Gameplay::SpotLightComponent& component = self.Get();
			return component.SetLightIntensity(intensity);
		},
		"InnerCone",
		[](Gameplay::ActorComponentHandle<Gameplay::SpotLightComponent>& self) -> float
		{
			const Gameplay::SpotLightComponent& component = self.Get();
			return component.GetInnerConeAngle();
		},
		"SetInnerCone",
		[](Gameplay::ActorComponentHandle<Gameplay::SpotLightComponent>& self, float angle)
		{
			Gameplay::SpotLightComponent& component = self.Get();
			return component.SetInnerConeAngle(angle);
		},
		"OuterCone",
		[](Gameplay::ActorComponentHandle<Gameplay::SpotLightComponent>& self) -> float
		{
			const Gameplay::SpotLightComponent& component = self.Get();
			return component.GetOuterConeAngle();
		},
		"SetOuterCone",
		[](Gameplay::ActorComponentHandle<Gameplay::SpotLightComponent>& self, float angle)
		{
			Gameplay::SpotLightComponent& component = self.Get();
			return component.SetOuterConeAngle(angle);
		}
	);
}

void Arg::Script::ScriptExport_World(const ScriptEngine& scriptEngine)
{
}
