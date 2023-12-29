#include <arg_pch.hpp>
#include "ScriptGlueGameplay.hpp"

#include "Gameplay/World/Actor/ActorHandle.hpp"

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
		"get_name",
		[](Gameplay::ActorHandle& self) -> std::string
		{
			const Gameplay::Actor& actor = self.Get();
			return actor.GetName();
		},
		"get_parent",
		[](Gameplay::ActorHandle& self) -> Gameplay::ActorHandle
		{
			const Gameplay::Actor& actor = self.Get();
			return Gameplay::ActorHandle(actor.GetWorld(), actor.GetParentActor()->GetID());
		},
		"get_position",
		[](Gameplay::ActorHandle& self) -> Vec3
		{
			const Gameplay::Actor& actor = self.Get();
			return actor.GetPosition();
		},
		"set_position",
		[](Gameplay::ActorHandle& self, const Vec3& position)
		{
			Gameplay::Actor& actor = self.Get();
			return actor.SetPosition(position);
		},
		"get_rotation",
		[](Gameplay::ActorHandle& self) -> Vec3
		{
			const Gameplay::Actor& actor = self.Get();
			return actor.GetRotation();
		},
		"set_rotation",
		[](Gameplay::ActorHandle& self, const Vec3& rotation)
		{
			Gameplay::Actor& actor = self.Get();
			return actor.SetRotation(rotation);
		},
		"get_scale",
		[](Gameplay::ActorHandle& self) -> Vec3
		{
			const Gameplay::Actor& actor = self.Get();
			return actor.GetScale();
		},
		"set_scale",
		[](Gameplay::ActorHandle& self, const Vec3& scale)
		{
			Gameplay::Actor& actor = self.Get();
			return actor.SetScale(scale);
		}
	);
}

void Arg::Script::ScriptExport_ActorComponents(const ScriptEngine& scriptEngine)
{
}

void Arg::Script::ScriptExport_World(const ScriptEngine& scriptEngine)
{
}
