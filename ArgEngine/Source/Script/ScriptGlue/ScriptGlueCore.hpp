#pragma once

#include <arg_pch.hpp>

#include "Script/ScriptEngine.hpp"

namespace Arg
{
	namespace Script
	{
		void ScriptExport_Core(const ScriptEngine& scriptEngine);
		void ScriptExport_Math(const ScriptEngine& scriptEngine);
	}
}
