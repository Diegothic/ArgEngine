#pragma once

#include <arg_pch.hpp>

#include "Script/ScriptEngine.hpp"

namespace Arg
{
	namespace Script
	{
		void ScriptExport_Actor(const ScriptEngine& scriptEngine);
		void ScriptExport_ActorComponents(const ScriptEngine& scriptEngine);
		void ScriptExport_World(const ScriptEngine& scriptEngine);
	}
}
