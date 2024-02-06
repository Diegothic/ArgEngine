﻿#include <arg_pch.hpp>
#include "ScriptGlueCore.hpp"

#include "Core/GUID.hpp"
#include "Core/Math/Math.hpp"
#include "Core/Event/Event.hpp"

void Arg::Script::ScriptExport_Core(const ScriptEngine& scriptEngine)
{
	auto& scriptState = scriptEngine.GetState();

	scriptState.new_usertype<GUID>(
		"GUID",
		sol::meta_function::to_string, [](const GUID& guid) -> std::string
		{
			return std::to_string(static_cast<uint64_t>(guid));
		}
	);

	scriptState.new_usertype<EventListenerHandle>(
		"EventListenerHandle"
	);
}

void Arg::Script::ScriptExport_Math(const ScriptEngine& scriptEngine)
{
	auto& scriptState = scriptEngine.GetState();

	scriptState.new_usertype<Vec3>(
		"Vec3",
		sol::constructors<Vec3(), Vec3(const Vec3&), Vec3(float), Vec3(float, float, float)>(),
		sol::meta_function::equal_to, sol::overload([](const Vec3& lhs, const Vec3& rhs) -> bool
		{
			return lhs == rhs;
		}),
		"X", &Vec3::x,
		"Y", &Vec3::y,
		"Z", &Vec3::z,
		sol::meta_function::addition, sol::overload([](const Vec3& lhs, const Vec3& rhs)
		{
			return lhs + rhs;
		}),
		sol::meta_function::subtraction, sol::overload([](const Vec3& lhs, const Vec3& rhs)
		{
			return lhs - rhs;
		}),
		sol::meta_function::multiplication, sol::overload([](const Vec3& lhs, const float& rhs)
		{
			return lhs * rhs;
		}),
		"Normalize", [](const Vec3& v) -> Vec3 { return Math::normalize(v); },
		"Length", [](const Vec3& v) -> float { return Math::length(v); },
		"Dot", [](const Vec3& lhs, const Vec3& rhs) -> float { return Math::dot(lhs, rhs); },
		"Cross", [](const Vec3& lhs, const Vec3& rhs) -> Vec3 { return Math::cross(lhs, rhs); }
	);

	scriptState.new_usertype<Vec2>(
		"Vec2",
		sol::constructors<Vec2(), Vec2(const Vec2&), Vec2(float), Vec2(float, float)>(),
		sol::meta_function::equal_to, sol::overload([](const Vec2& lhs, const Vec2& rhs) -> bool
		{
			return lhs == rhs;
		}),
		"X", &Vec2::x,
		"Y", &Vec2::y,
		sol::meta_function::addition, sol::overload([](const Vec2& lhs, const Vec2& rhs)
		{
			return lhs + rhs;
		}),
		sol::meta_function::subtraction, sol::overload([](const Vec2& lhs, const Vec2& rhs)
		{
			return lhs - rhs;
		}),
		sol::meta_function::multiplication, sol::overload([](const Vec2& lhs, const float& rhs)
		{
			return lhs * rhs;
		}),
		"Normalize", [](const Vec2& v) -> Vec2 { return Math::normalize(v); },
		"Length", [](const Vec2& v) -> float { return Math::length(v); }
	);

	sol::table mathTable = scriptState["Mathf"].get_or_create<sol::table>();
	mathTable.set_function(
		"Lerp",
		sol::overload(
			[](const float& a, const float& b, float t) -> float
			{
				return Math::Lerp(a, b, t);
			},
			[](const Vec2& a, const Vec2& b, float t) -> Vec2
			{
				return Math::Lerp(a, b, t);
			},
			[](const Vec3& a, const Vec3& b, float t) -> Vec3
			{
				return Math::Lerp(a, b, t);
			}
		)
	);
	mathTable.set_function(
		"SLerp",
		[](const Vec3& a, const Vec3& b, float t) -> Vec3
		{
			return Math::degrees(Math::ToEuler(Math::SLerp(
				Math::ToQuat(Math::radians(a)),
				Math::ToQuat(Math::radians(b)),
				t
			)));
		}
	);
}
