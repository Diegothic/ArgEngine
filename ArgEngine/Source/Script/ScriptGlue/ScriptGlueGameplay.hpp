#pragma once

#include <arg_pch.hpp>

#include "Script/ScriptEngine.hpp"

namespace Arg
{
	namespace Script
	{
		void ScriptExport_World(const ScriptEngine& scriptEngine);
		void ScriptExport_Actor(const ScriptEngine& scriptEngine);
		void ScriptExport_ActorComponents_Graphics(const ScriptEngine& scriptEngine);
		void ScriptExport_ActorComponents_Physics(const ScriptEngine& scriptEngine);
		void ScriptExport_ActorComponents_Sound(const ScriptEngine& scriptEngine);
	}
}
