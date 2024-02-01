#include <arg_pch.hpp>
#include "ScriptGlueGameplay.hpp"

#include "Gameplay/World/Actor/ActorHandle.hpp"
#include "Gameplay/World/Actor/Component/ActorComponentHandle.hpp"
#include "Gameplay/World/Actor/Component/Components/Graphics/PointLightComponent.hpp"
#include "Gameplay/World/Actor/Component/Components/Graphics/SkeletalModelComponent.hpp"
#include "Gameplay/World/Actor/Component/Components/Graphics/SpotLightComponent.hpp"
#include "Gameplay/World/Actor/Component/Components/Graphics/StaticModelComponent.hpp"
#include "Gameplay/World/Actor/Component/Components/Physics/PhysicsBodyComponent.hpp"
#include "Gameplay/World/Actor/Component/Components/Sound/SoundPlayerComponent.hpp"

using CameraComponentHandle = Arg::Gameplay::ActorComponentHandle<Arg::Gameplay::CameraComponent>;
using PointLightComponentHandle = Arg::Gameplay::ActorComponentHandle<Arg::Gameplay::PointLightComponent>;
using SpotLightComponentHandle = Arg::Gameplay::ActorComponentHandle<Arg::Gameplay::SpotLightComponent>;
using StaticModelComponentHandle = Arg::Gameplay::ActorComponentHandle<Arg::Gameplay::StaticModelComponent>;
using SkeletalModelComponentHandle = Arg::Gameplay::ActorComponentHandle<Arg::Gameplay::SkeletalModelComponent>;

using PhysicsBodyComponentHandle = Arg::Gameplay::ActorComponentHandle<Arg::Gameplay::PhysicsBodyComponent>;

using SoundPlayerComponentHandle = Arg::Gameplay::ActorComponentHandle<Arg::Gameplay::SoundPlayerComponent>;

void Arg::Script::ScriptExport_World(const ScriptEngine& scriptEngine)
{
	auto& scriptState = scriptEngine.GetState();

	scriptState.new_usertype<Gameplay::GameWorld>(
		"World",
		"SunlightColor",
		[](Gameplay::GameWorld& self) -> Vec3
		{
			return self.GetSunlightColor();
		},
		"SetSunlightColor",
		[](Gameplay::GameWorld& self, const Vec3& color)
		{
			self.SetSunlightColor(color);
		},
		"SunlightDirection",
		[](Gameplay::GameWorld& self) -> Vec3
		{
			return self.GetSunlightDirection();
		},
		"SetSunlightDirection",
		[](Gameplay::GameWorld& self, const Vec3& direction)
		{
			self.SetSunlightDirection(direction);
		},
		"SunlightIntensity",
		[](Gameplay::GameWorld& self) -> float
		{
			return self.GetSunlightIntensity();
		},
		"SetSunlightIntensity",
		[](Gameplay::GameWorld& self, float intensity)
		{
			self.SetSunlightIntensity(intensity);
		},
		"IsSunlightCastingShadows",
		[](Gameplay::GameWorld& self) -> bool
		{
			return self.GetSunlightCastsShadows();
		},
		"SetSunlightCastingShadows",
		[](Gameplay::GameWorld& self, bool bCastShadows)
		{
			self.SetSunlightCastsShadows(bCastShadows);
		},
		"IsUsingSkybox",
		[](Gameplay::GameWorld& self) -> bool
		{
			return self.GetUsingSkybox();
		},
		"SetUsingSkybox",
		[](Gameplay::GameWorld& self, bool bUsingSkybox)
		{
			self.SetUsingSkybox(bUsingSkybox);
		},
		"BackgroundColor",
		[](Gameplay::GameWorld& self) -> Vec3
		{
			return self.GetBackgroundColor();
		},
		"SetBackgroundColor",
		[](Gameplay::GameWorld& self, const Vec3& color)
		{
			self.SetBackgroundColor(color);
		},
		"SkyboxTextureAtIndex",
		[](Gameplay::GameWorld& self, int32_t index) -> TextureHandle
		{
			index--;
			index = Math::clamp(index, 0, 5);
			return self.GetSkyboxTexture(index);
		},
		"SetSkyboxTextureAtIndex",
		[](Gameplay::GameWorld& self, int32_t index, const TextureHandle& texture)
		{
			index--;
			index = Math::clamp(index, 0, 5);
			self.SetSkyboxTexture(index, texture);
		},
		"Gravity",
		[](Gameplay::GameWorld& self) -> Vec3
		{
			return self.GetGravity();
		},
		"SetGravity",
		[](Gameplay::GameWorld& self, const Vec3& gravity)
		{
			self.SetGravity(gravity);
		},
		"MainCamera",
		[](Gameplay::GameWorld& self) -> CameraComponentHandle
		{
			return self.GetMainCamera();
		},
		"SetMainCamera",
		[](Gameplay::GameWorld& self, const CameraComponentHandle& camera)
		{
			self.SetMainCamera(camera);
		}
	);
}

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

			if (componentID == Gameplay::CameraComponent::COMPONENT_NAME)
			{
				return sol::make_object(
					lua,
					CameraComponentHandle(
						actor.GetWorld(), ownerID, ID
					)
				);
			}

			if (componentID == Gameplay::PointLightComponent::COMPONENT_NAME)
			{
				return sol::make_object(
					lua,
					PointLightComponentHandle(
						actor.GetWorld(), ownerID, ID
					)
				);
			}

			if (componentID == Gameplay::SpotLightComponent::COMPONENT_NAME)
			{
				return sol::make_object(
					lua,
					SpotLightComponentHandle(
						actor.GetWorld(), ownerID, ID
					)
				);
			}

			if (componentID == Gameplay::StaticModelComponent::COMPONENT_NAME)
			{
				return sol::make_object(
					lua,
					StaticModelComponentHandle(
						actor.GetWorld(), ownerID, ID
					)
				);
			}

			if (componentID == Gameplay::SkeletalModelComponent::COMPONENT_NAME)
			{
				return sol::make_object(
					lua,
					SkeletalModelComponentHandle(
						actor.GetWorld(), ownerID, ID
					)
				);
			}

			if (componentID == Gameplay::PhysicsBodyComponent::COMPONENT_NAME)
			{
				return sol::make_object(
					lua,
					PhysicsBodyComponentHandle(
						actor.GetWorld(), ownerID, ID
					)
				);
			}

			if (componentID == Gameplay::SoundPlayerComponent::COMPONENT_NAME)
			{
				return sol::make_object(
					lua,
					SoundPlayerComponentHandle(
						actor.GetWorld(), ownerID, ID
					)
				);
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

	scriptState.new_enum<Gameplay::CameraMode>(
		"CameraMode",
		{
			{"Perspective", Gameplay::CameraMode::Perspective},
			{"Orthographic", Gameplay::CameraMode::Orthographic}
		}
	);

	scriptState.new_usertype<CameraComponentHandle>(
		"CameraComponent",
		sol::meta_function::equal_to, sol::overload([](
			const CameraComponentHandle& lhs,
			const CameraComponentHandle& rhs
		) -> bool
			{
				return lhs == rhs;
			}),
		"IsValid",
		[](CameraComponentHandle& self) -> bool
		{
			return self.IsValid();
		},
		"Mode",
		[](CameraComponentHandle& self) -> Gameplay::CameraMode
		{
			const Gameplay::CameraComponent& component = self.Get();
			return component.GetMode();
		},
		"SetMode",
		[](CameraComponentHandle& self, const Gameplay::CameraMode& mode)
		{
			Gameplay::CameraComponent& component = self.Get();
			return component.SetMode(mode);
		},
		"FOV",
		[](CameraComponentHandle& self) -> float
		{
			const Gameplay::CameraComponent& component = self.Get();
			return component.GetFOVAngle();
		},
		"SetFOV",
		[](CameraComponentHandle& self, float fov)
		{
			Gameplay::CameraComponent& component = self.Get();
			return component.SetFOVAngle(fov);
		},
		"Size",
		[](CameraComponentHandle& self) -> float
		{
			const Gameplay::CameraComponent& component = self.Get();
			return component.GetSize();
		},
		"SetSize",
		[](CameraComponentHandle& self, float size)
		{
			Gameplay::CameraComponent& component = self.Get();
			return component.SetSize(size);
		},
		"ScreenToWorldPoint",
		[](CameraComponentHandle& self, const Vec2& screenPoint)
		{
			Gameplay::CameraComponent& component = self.Get();
			return component.ScreenToWorldPoint(screenPoint);
		},
		"WorldToScreenPoint",
		[](CameraComponentHandle& self, const Vec3& worldPoint)
		{
			Gameplay::CameraComponent& component = self.Get();
			return component.WorldToScreenPoint(worldPoint);
		}
	);

	scriptState.new_usertype<PointLightComponentHandle>(
		"PointLightComponent",
		sol::meta_function::equal_to, sol::overload([](
			const PointLightComponentHandle& lhs,
			const PointLightComponentHandle& rhs
		) -> bool
			{
				return lhs == rhs;
			}),
		"IsValid",
		[](PointLightComponentHandle& self) -> bool
		{
			return self.IsValid();
		},
		"Color",
		[](PointLightComponentHandle& self) -> Vec3
		{
			const Gameplay::PointLightComponent& component = self.Get();
			return component.GetLightColor();
		},
		"SetColor",
		[](PointLightComponentHandle& self, const Vec3& color)
		{
			Gameplay::PointLightComponent& component = self.Get();
			return component.SetLightColor(color);
		},
		"Range",
		[](PointLightComponentHandle& self) -> float
		{
			const Gameplay::PointLightComponent& component = self.Get();
			return component.GetLightRange();
		},
		"SetRange",
		[](PointLightComponentHandle& self, float range)
		{
			Gameplay::PointLightComponent& component = self.Get();
			return component.SetLightRange(range);
		},
		"Intensity",
		[](PointLightComponentHandle& self) -> float
		{
			const Gameplay::PointLightComponent& component = self.Get();
			return component.GetLightIntensity();
		},
		"SetIntensity",
		[](PointLightComponentHandle& self, float intensity)
		{
			Gameplay::PointLightComponent& component = self.Get();
			return component.SetLightIntensity(intensity);
		}
	);

	scriptState.new_usertype<SpotLightComponentHandle>(
		"SpotLightComponent",
		sol::meta_function::equal_to, sol::overload([](
			const SpotLightComponentHandle& lhs,
			const SpotLightComponentHandle& rhs
		) -> bool
			{
				return lhs == rhs;
			}),
		"IsValid",
		[](CameraComponentHandle& self) -> bool
		{
			return self.IsValid();
		},
		"Color",
		[](SpotLightComponentHandle& self) -> Vec3
		{
			const Gameplay::SpotLightComponent& component = self.Get();
			return component.GetLightColor();
		},
		"SetColor",
		[](SpotLightComponentHandle& self, const Vec3& color)
		{
			Gameplay::SpotLightComponent& component = self.Get();
			return component.SetLightColor(color);
		},
		"Range",
		[](SpotLightComponentHandle& self) -> float
		{
			const Gameplay::SpotLightComponent& component = self.Get();
			return component.GetLightRange();
		},
		"SetRange",
		[](SpotLightComponentHandle& self, float range)
		{
			Gameplay::SpotLightComponent& component = self.Get();
			return component.SetLightRange(range);
		},
		"Intensity",
		[](SpotLightComponentHandle& self) -> float
		{
			const Gameplay::SpotLightComponent& component = self.Get();
			return component.GetLightIntensity();
		},
		"SetIntensity",
		[](SpotLightComponentHandle& self, float intensity)
		{
			Gameplay::SpotLightComponent& component = self.Get();
			return component.SetLightIntensity(intensity);
		},
		"InnerCone",
		[](SpotLightComponentHandle& self) -> float
		{
			const Gameplay::SpotLightComponent& component = self.Get();
			return component.GetInnerConeAngle();
		},
		"SetInnerCone",
		[](SpotLightComponentHandle& self, float angle)
		{
			Gameplay::SpotLightComponent& component = self.Get();
			return component.SetInnerConeAngle(angle);
		},
		"OuterCone",
		[](SpotLightComponentHandle& self) -> float
		{
			const Gameplay::SpotLightComponent& component = self.Get();
			return component.GetOuterConeAngle();
		},
		"SetOuterCone",
		[](SpotLightComponentHandle& self, float angle)
		{
			Gameplay::SpotLightComponent& component = self.Get();
			return component.SetOuterConeAngle(angle);
		}
	);

	scriptState.new_usertype<StaticModelComponentHandle>(
		"StaticModelComponent",
		sol::meta_function::equal_to, sol::overload([](
			const StaticModelComponentHandle& lhs,
			const StaticModelComponentHandle& rhs
		) -> bool
			{
				return lhs == rhs;
			}),
		"IsValid",
		[](StaticModelComponentHandle& self) -> bool
		{
			return self.IsValid();
		},
		"Model",
		[](StaticModelComponentHandle& self) -> StaticModelHandle
		{
			Gameplay::StaticModelComponent& component = self.Get();
			return component.GetStaticModel();
		},
		"SetModel",
		[](StaticModelComponentHandle& self, const StaticModelHandle& model)
		{
			Gameplay::StaticModelComponent& component = self.Get();
			component.SetStaticModel(model);
		},
		"MaterialCount",
		[](StaticModelComponentHandle& self) -> int32_t
		{
			Gameplay::StaticModelComponent& component = self.Get();
			return component.GetMaterialCount();
		},
		"MaterialAtIndex",
		[](StaticModelComponentHandle& self, int32_t index) -> MaterialHandle
		{
			Gameplay::StaticModelComponent& component = self.Get();
			index--;
			index = Math::clamp(index, 0, static_cast<int32_t>(component.GetMaterialCount()) - 1);
			return component.GetMaterial(index);
		},
		"SetMaterialAtIndex",
		[](StaticModelComponentHandle& self, int32_t index, const MaterialHandle& material)
		{
			Gameplay::StaticModelComponent& component = self.Get();
			index--;
			index = Math::clamp(index, 0, static_cast<int32_t>(component.GetMaterialCount()) - 1);
			component.SetMaterial(index, material);
		},
		"IsReceivingShadows",
		[](StaticModelComponentHandle& self) -> bool
		{
			Gameplay::StaticModelComponent& component = self.Get();
			return component.GetReceiveShadows();
		},
		"SetReceivingShadows",
		[](StaticModelComponentHandle& self, bool bReceiveShadows)
		{
			Gameplay::StaticModelComponent& component = self.Get();
			component.SetReceiveShadows(bReceiveShadows);
		},
		"IsCastingShadows",
		[](StaticModelComponentHandle& self) -> bool
		{
			Gameplay::StaticModelComponent& component = self.Get();
			return component.GetCastShadows();
		},
		"SetCastingShadows",
		[](StaticModelComponentHandle& self, bool bCastShadows)
		{
			Gameplay::StaticModelComponent& component = self.Get();
			component.SetCastShadows(bCastShadows);
		}
	);

	scriptState.new_usertype<SkeletalModelComponentHandle>(
		"StaticModelComponent",
		sol::meta_function::equal_to, sol::overload([](
			const SkeletalModelComponentHandle& lhs,
			const SkeletalModelComponentHandle& rhs
		) -> bool
			{
				return lhs == rhs;
			}),
		"IsValid",
		[](SkeletalModelComponentHandle& self) -> bool
		{
			return self.IsValid();
		},
		"Skeleton",
		[](SkeletalModelComponentHandle& self) -> SkeletonHandle
		{
			Gameplay::SkeletalModelComponent& component = self.Get();
			return component.GetSkeleton();
		},
		"SetSkeleton",
		[](SkeletalModelComponentHandle& self, const SkeletonHandle& skeleton)
		{
			Gameplay::SkeletalModelComponent& component = self.Get();
			component.SetSkeleton(skeleton);
		},
		"Model",
		[](SkeletalModelComponentHandle& self) -> SkeletalModelHandle
		{
			Gameplay::SkeletalModelComponent& component = self.Get();
			return component.GetModel();
		},
		"SetModel",
		[](SkeletalModelComponentHandle& self, const SkeletalModelHandle& model)
		{
			Gameplay::SkeletalModelComponent& component = self.Get();
			component.SetModel(model);
		},
		"MaterialCount",
		[](SkeletalModelComponentHandle& self) -> int32_t
		{
			Gameplay::SkeletalModelComponent& component = self.Get();
			return component.GetMaterialCount();
		},
		"MaterialAtIndex",
		[](SkeletalModelComponentHandle& self, int32_t index) -> MaterialHandle
		{
			Gameplay::SkeletalModelComponent& component = self.Get();
			index--;
			index = Math::clamp(index, 0, static_cast<int32_t>(component.GetMaterialCount()) - 1);
			return component.GetMaterial(index);
		},
		"SetMaterialAtIndex",
		[](SkeletalModelComponentHandle& self, int32_t index, const MaterialHandle& material)
		{
			Gameplay::SkeletalModelComponent& component = self.Get();
			index--;
			index = Math::clamp(index, 0, static_cast<int32_t>(component.GetMaterialCount()) - 1);
			component.SetMaterial(index, material);
		},
		"IsReceivingShadows",
		[](SkeletalModelComponentHandle& self) -> bool
		{
			Gameplay::SkeletalModelComponent& component = self.Get();
			return component.GetReceiveShadows();
		},
		"SetReceivingShadows",
		[](SkeletalModelComponentHandle& self, bool bReceiveShadows)
		{
			Gameplay::SkeletalModelComponent& component = self.Get();
			component.SetReceiveShadows(bReceiveShadows);
		},
		"IsCastingShadows",
		[](SkeletalModelComponentHandle& self) -> bool
		{
			Gameplay::SkeletalModelComponent& component = self.Get();
			return component.GetCastShadows();
		},
		"SetCastingShadows",
		[](SkeletalModelComponentHandle& self, bool bCastShadows)
		{
			Gameplay::SkeletalModelComponent& component = self.Get();
			component.SetCastShadows(bCastShadows);
		},
		"CurrentAnimation",
		[](SkeletalModelComponentHandle& self) -> SkeletalAnimationHandle
		{
			Gameplay::SkeletalModelComponent& component = self.Get();
			return component.GetCurrentAnimation();
		},
		"SetCurrentAnimation",
		[](SkeletalModelComponentHandle& self, SkeletalAnimationHandle animation)
		{
			Gameplay::SkeletalModelComponent& component = self.Get();
			component.SetCurrentAnimation(animation);
		},
		"IsPlayingOnStart",
		[](SkeletalModelComponentHandle& self) -> bool
		{
			Gameplay::SkeletalModelComponent& component = self.Get();
			return component.GetPlayOnStart();
		},
		"SetPlayingOnStart",
		[](SkeletalModelComponentHandle& self, bool bPlayOnStart)
		{
			Gameplay::SkeletalModelComponent& component = self.Get();
			component.SetPlayOnStart(bPlayOnStart);
		},
		"IsLooping",
		[](SkeletalModelComponentHandle& self) -> bool
		{
			Gameplay::SkeletalModelComponent& component = self.Get();
			return component.GetLooping();
		},
		"SetLooping",
		[](SkeletalModelComponentHandle& self, bool bLooping)
		{
			Gameplay::SkeletalModelComponent& component = self.Get();
			component.SetLooping(bLooping);
		},
		"Play",
		sol::overload(
			[](SkeletalModelComponentHandle& self, SkeletalAnimationHandle animation, bool bLooping)
			{
				Gameplay::SkeletalModelComponent& component = self.Get();
				component.SetLooping(bLooping);
				component.Play(animation);
			},
			[](SkeletalModelComponentHandle& self, SkeletalAnimationHandle animation)
			{
				Gameplay::SkeletalModelComponent& component = self.Get();
				component.SetLooping(false);
				component.Play(animation);
			}),
		"Stop",
		[](SkeletalModelComponentHandle& self)
		{
			Gameplay::SkeletalModelComponent& component = self.Get();
			component.Stop();
		}
	);
}

void Arg::Script::ScriptExport_ActorComponents_Physics(const ScriptEngine& scriptEngine)
{
	auto& scriptState = scriptEngine.GetState();

	scriptState.new_enum<Physics::PhysicsBodyShape>(
		"PhysicsShape",
		{
			{"Box", Physics::PhysicsBodyShape::Box},
			{"Sphere", Physics::PhysicsBodyShape::Sphere},
			{"Capsule", Physics::PhysicsBodyShape::Capsule}
		}
	);

	scriptState.new_usertype<PhysicsBodyComponentHandle>(
		"PhysicsBodyComponent",
		sol::meta_function::equal_to, sol::overload([](
			const PhysicsBodyComponentHandle& lhs,
			const PhysicsBodyComponentHandle& rhs
		) -> bool
			{
				return lhs == rhs;
			}),
		"IsValid",
		[](PhysicsBodyComponentHandle& self) -> bool
		{
			return self.IsValid();
		},
		"Shape",
		[](PhysicsBodyComponentHandle& self) -> Physics::PhysicsBodyShape
		{
			Gameplay::PhysicsBodyComponent& component = self.Get();
			return component.GetPhysicsShape();
		},
		"SetShape",
		[](PhysicsBodyComponentHandle& self, const Physics::PhysicsBodyShape& shape)
		{
			Gameplay::PhysicsBodyComponent& component = self.Get();
			return component.SetPhysicsShape(shape);
		},
		"Size",
		[](PhysicsBodyComponentHandle& self) -> Vec3
		{
			Gameplay::PhysicsBodyComponent& component = self.Get();
			return component.GetSize();
		},
		"SetSize",
		[](PhysicsBodyComponentHandle& self, const Vec3& size)
		{
			Gameplay::PhysicsBodyComponent& component = self.Get();
			return component.SetSize(size);
		},
		"IsDynamic",
		[](PhysicsBodyComponentHandle& self) -> bool
		{
			Gameplay::PhysicsBodyComponent& component = self.Get();
			return component.GetIsDynamic();
		},
		"SetSize",
		[](PhysicsBodyComponentHandle& self, bool bIsDynamic)
		{
			Gameplay::PhysicsBodyComponent& component = self.Get();
			return component.SetIsDynamic(bIsDynamic);
		},
		"Mass",
		[](PhysicsBodyComponentHandle& self) -> float
		{
			Gameplay::PhysicsBodyComponent& component = self.Get();
			return component.GetMass();
		},
		"SetMass",
		[](PhysicsBodyComponentHandle& self, float mass)
		{
			Gameplay::PhysicsBodyComponent& component = self.Get();
			return component.SetMass(mass);
		},
		"Damping",
		[](PhysicsBodyComponentHandle& self) -> float
		{
			Gameplay::PhysicsBodyComponent& component = self.Get();
			return component.GetDamping();
		},
		"SetDamping",
		[](PhysicsBodyComponentHandle& self, float damping)
		{
			Gameplay::PhysicsBodyComponent& component = self.Get();
			return component.SetDamping(damping);
		},
		"AngularDamping",
		[](PhysicsBodyComponentHandle& self) -> float
		{
			Gameplay::PhysicsBodyComponent& component = self.Get();
			return component.GetAngularDamping();
		},
		"SetAngularDamping",
		[](PhysicsBodyComponentHandle& self, float damping)
		{
			Gameplay::PhysicsBodyComponent& component = self.Get();
			return component.SetAngularDamping(damping);
		},
		"Bounciness",
		[](PhysicsBodyComponentHandle& self) -> float
		{
			Gameplay::PhysicsBodyComponent& component = self.Get();
			return component.GetBounciness();
		},
		"SetBounciness",
		[](PhysicsBodyComponentHandle& self, float bounciness)
		{
			Gameplay::PhysicsBodyComponent& component = self.Get();
			return component.SetBounciness(bounciness);
		},
		"IsXMovementLocked",
		[](PhysicsBodyComponentHandle& self) -> float
		{
			Gameplay::PhysicsBodyComponent& component = self.Get();
			return component.GetMovementLockX();
		},
		"SetXMovementLocked",
		[](PhysicsBodyComponentHandle& self, float bLock)
		{
			Gameplay::PhysicsBodyComponent& component = self.Get();
			return component.SetMovementLockX(bLock);
		},
		"IsYMovementLocked",
		[](PhysicsBodyComponentHandle& self) -> float
		{
			Gameplay::PhysicsBodyComponent& component = self.Get();
			return component.GetMovementLockY();
		},
		"SetYMovementLocked",
		[](PhysicsBodyComponentHandle& self, float bLock)
		{
			Gameplay::PhysicsBodyComponent& component = self.Get();
			return component.SetMovementLockY(bLock);
		},
		"IsZMovementLocked",
		[](PhysicsBodyComponentHandle& self) -> float
		{
			Gameplay::PhysicsBodyComponent& component = self.Get();
			return component.GetMovementLockZ();
		},
		"SetZMovementLocked",
		[](PhysicsBodyComponentHandle& self, float bLock)
		{
			Gameplay::PhysicsBodyComponent& component = self.Get();
			return component.SetMovementLockZ(bLock);
		},
		"IsXRotationLocked",
		[](PhysicsBodyComponentHandle& self) -> float
		{
			Gameplay::PhysicsBodyComponent& component = self.Get();
			return component.GetRotationLockX();
		},
		"SetXRotationLocked",
		[](PhysicsBodyComponentHandle& self, float bLock)
		{
			Gameplay::PhysicsBodyComponent& component = self.Get();
			return component.SetRotationLockX(bLock);
		},
		"IsYRotationLocked",
		[](PhysicsBodyComponentHandle& self) -> float
		{
			Gameplay::PhysicsBodyComponent& component = self.Get();
			return component.GetRotationLockY();
		},
		"SetYRotationLocked",
		[](PhysicsBodyComponentHandle& self, float bLock)
		{
			Gameplay::PhysicsBodyComponent& component = self.Get();
			return component.SetRotationLockY(bLock);
		},
		"IsZRotationLocked",
		[](PhysicsBodyComponentHandle& self) -> float
		{
			Gameplay::PhysicsBodyComponent& component = self.Get();
			return component.GetRotationLockZ();
		},
		"SetZRotationLocked",
		[](PhysicsBodyComponentHandle& self, float bLock)
		{
			Gameplay::PhysicsBodyComponent& component = self.Get();
			return component.SetRotationLockZ(bLock);
		},
		"Wake",
		[](PhysicsBodyComponentHandle& self)
		{
			Gameplay::PhysicsBodyComponent& component = self.Get();
			component.Wake();
		}
	);
}

void Arg::Script::ScriptExport_ActorComponents_Sound(const ScriptEngine& scriptEngine)
{
	auto& scriptState = scriptEngine.GetState();

	scriptState.new_usertype<SoundPlayerComponentHandle>(
		"SoundPlayerComponent",
		sol::meta_function::equal_to, sol::overload([](
			const SoundPlayerComponentHandle& lhs,
			const SoundPlayerComponentHandle& rhs
		) -> bool
			{
				return lhs == rhs;
			}),
		"IsValid",
		[](SoundPlayerComponentHandle& self) -> bool
		{
			return self.IsValid();
		},
		"Sound",
		[](SoundPlayerComponentHandle& self) -> SoundResourceHandle
		{
			Gameplay::SoundPlayerComponent& component = self.Get();
			return component.GetSound();
		},
		"SetSound",
		[](SoundPlayerComponentHandle& self, const SoundResourceHandle& sound)
		{
			Gameplay::SoundPlayerComponent& component = self.Get();
			component.SetSound(sound);
		},
		"IsPlayingOnStart",
		[](SoundPlayerComponentHandle& self) -> bool
		{
			Gameplay::SoundPlayerComponent& component = self.Get();
			return component.GetPlayOnStart();
		},
		"SetPlayingOnStart",
		[](SoundPlayerComponentHandle& self, bool bPlayOnStart)
		{
			Gameplay::SoundPlayerComponent& component = self.Get();
			component.SetPlayOnStart(bPlayOnStart);
		},
		"Volume",
		[](SoundPlayerComponentHandle& self) -> float
		{
			Gameplay::SoundPlayerComponent& component = self.Get();
			return component.GetVolume();
		},
		"SetVolume",
		[](SoundPlayerComponentHandle& self, float volume)
		{
			Gameplay::SoundPlayerComponent& component = self.Get();
			component.SetVolume(volume);
		},
		"VolumeVariance",
		[](SoundPlayerComponentHandle& self) -> float
		{
			Gameplay::SoundPlayerComponent& component = self.Get();
			return component.GetVolumeVariance();
		},
		"SetVolumeVariance",
		[](SoundPlayerComponentHandle& self, float variance)
		{
			Gameplay::SoundPlayerComponent& component = self.Get();
			component.SetVolumeVariance(variance);
		},
		"Pitch",
		[](SoundPlayerComponentHandle& self) -> float
		{
			Gameplay::SoundPlayerComponent& component = self.Get();
			return component.GetPitch();
		},
		"SetPitch",
		[](SoundPlayerComponentHandle& self, float pitch)
		{
			Gameplay::SoundPlayerComponent& component = self.Get();
			component.SetPitch(pitch);
		},
		"PitchVariance",
		[](SoundPlayerComponentHandle& self) -> float
		{
			Gameplay::SoundPlayerComponent& component = self.Get();
			return component.GetPitchVariance();
		},
		"SetPitchVariance",
		[](SoundPlayerComponentHandle& self, float variance)
		{
			Gameplay::SoundPlayerComponent& component = self.Get();
			component.SetPitchVariance(variance);
		},
		"IsLooping",
		[](SoundPlayerComponentHandle& self) -> bool
		{
			Gameplay::SoundPlayerComponent& component = self.Get();
			return component.GetIsLooping();
		},
		"SetLooping",
		[](SoundPlayerComponentHandle& self, bool bIsLooping)
		{
			Gameplay::SoundPlayerComponent& component = self.Get();
			component.SetIsLooping(bIsLooping);
		},
		"Is3D",
		[](SoundPlayerComponentHandle& self) -> bool
		{
			Gameplay::SoundPlayerComponent& component = self.Get();
			return component.GetIs3D();
		},
		"Set3D",
		[](SoundPlayerComponentHandle& self, bool bIs3D)
		{
			Gameplay::SoundPlayerComponent& component = self.Get();
			component.SetIs3D(bIs3D);
		},
		"Range",
		[](SoundPlayerComponentHandle& self) -> float
		{
			Gameplay::SoundPlayerComponent& component = self.Get();
			return component.GetDistance();
		},
		"SetRange",
		[](SoundPlayerComponentHandle& self, float range)
		{
			Gameplay::SoundPlayerComponent& component = self.Get();
			component.SetDistance(range);
		},
		"Play",
		[](SoundPlayerComponentHandle& self)
		{
			Gameplay::SoundPlayerComponent& component = self.Get();
			component.Play();
		},
		"Pause",
		[](SoundPlayerComponentHandle& self)
		{
			Gameplay::SoundPlayerComponent& component = self.Get();
			component.Pause();
		},
		"Unpause",
		[](SoundPlayerComponentHandle& self)
		{
			Gameplay::SoundPlayerComponent& component = self.Get();
			component.Unpause();
		},
		"Stop",
		[](SoundPlayerComponentHandle& self)
		{
			Gameplay::SoundPlayerComponent& component = self.Get();
			component.Stop();
		},
		"IsPlaying",
		[](SoundPlayerComponentHandle& self) -> bool
		{
			Gameplay::SoundPlayerComponent& component = self.Get();
			return component.IsPlaying();
		}
	);
}
