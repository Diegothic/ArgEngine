#include <arg_pch.hpp>
#include "ScriptEngine.hpp"

#include "ScriptComponent.hpp"
#include "Gameplay/World/Actor/ActorHandle.hpp"
#include "Gameplay/World/Actor/Component/ActorComponentHandle.hpp"
#include "ScriptGlue/ScriptGlueCore.hpp"
#include "ScriptGlue/ScriptGlueGameplay.hpp"

using ScriptComponentHandle = Arg::Gameplay::ActorComponentHandle<Arg::Script::ScriptComponent>;

Arg::Script::ScriptEngine::ScriptEngine(const ScriptEngineSpec& spec)
	: m_RootDirectory(spec.RootDirectory),
	  m_SourceFileName(spec.SourceFileName),
	  m_pComponentRegistry(spec.ComponentRegistry)
{
}

void Arg::Script::ScriptEngine::Initialize()
{
	if (!std::filesystem::exists(m_RootDirectory))
	{
		std::filesystem::create_directory(m_RootDirectory);
	}
}

void Arg::Script::ScriptEngine::Rebuild()
{
	const std::filesystem::path scriptsDestinationFilePath = (m_RootDirectory / m_SourceFileName)
		.replace_extension(".lua");
	const std::filesystem::path scriptsSourceFilePath = (m_RootDirectory / m_SourceFileName)
		.replace_extension(".src");

	std::ofstream scriptsDestinationFile(
		scriptsDestinationFilePath,
		std::ios::out
		| std::ofstream::trunc
	);

	{
		std::ifstream scriptsSourceFile(scriptsSourceFilePath);
		std::string line;
		while (std::getline(scriptsSourceFile, line))
		{
			const std::filesystem::path scriptFilePath = m_RootDirectory / line;
			if (!std::filesystem::exists(scriptFilePath))
			{
				continue;
			}

			std::ifstream scriptFile(scriptFilePath);
			std::string scriptLine;
			while (std::getline(scriptFile, scriptLine))
			{
				scriptsDestinationFile << scriptLine;
				scriptsDestinationFile << std::endl;
			}

			scriptFile.close();
		}

		scriptsSourceFile.close();
	}

	scriptsDestinationFile.close();
}

void Arg::Script::ScriptEngine::Load()
{
	const std::filesystem::path scriptsSourceFilePath = (m_RootDirectory / m_SourceFileName)
		.replace_extension(".lua");

	m_pLuaState = nullptr;
	m_pLuaState = std::make_unique<sol::state>();
	m_pLuaState->open_libraries(
		sol::lib::base,
		sol::lib::package,
		sol::lib::string,
		sol::lib::coroutine,
		sol::lib::os,
		sol::lib::table,
		sol::lib::utf8,
		sol::lib::math,
		sol::lib::io
	);

	ScriptExport_Core(*this);
	ScriptExport_Math(*this);

	ScriptExport_World(*this);
	ScriptExport_Actor(*this);
	ScriptExport_ActorComponents(*this);

	(*m_pLuaState)["DEF_COMPONENT"] = [&](const std::string& componentName)
	{
		DefineComponent(componentName);
	};

	(*m_pLuaState)["DEF_FIELD"] = [&](
		const std::string& componentName,
		const std::string& fieldName,
		const sol::object& value
	)
		{
			DefineField(componentName, fieldName, value);
		};


	m_pLuaState->new_usertype<ScriptComponentHandle>(
		"ScriptComponent",
		"_on_create", [](ScriptComponentHandle&)
		{
		},
		"_begin_play", [](ScriptComponentHandle&)
		{
		},
		"_tick", [](ScriptComponentHandle&, float)
		{
		},
		"_on_destroy", [](ScriptComponentHandle&)
		{
		},
		"get_owner",
		[](ScriptComponentHandle& self) -> Gameplay::ActorHandle
		{
			const ScriptComponent& component = self.Get();
			const Gameplay::Actor* actor = component.GetOwner();
			return {actor->GetWorld(), actor->GetID()};
		}
	);

	m_pLuaState->script_file(scriptsSourceFilePath.string());
}

void Arg::Script::ScriptEngine::DefineComponent(const std::string& componentName)
{
	ScriptComponent* componentPrototype = new ScriptComponent(this, componentName);
	sol::table componentTable = (*m_pLuaState)[componentName].get_or_create<sol::table>();
	componentTable[sol::metatable_key] = (*m_pLuaState)["ScriptComponent"];

	(*m_pLuaState)[componentName]["_instances"].get_or_create<sol::table>();

	m_pComponentRegistry->RegisterComponent(componentPrototype);
}

void Arg::Script::ScriptEngine::DefineField(
	const std::string& componentName,
	const std::string& fieldName,
	const sol::object& fieldDefaultValue
)
{
	if (fieldDefaultValue.is<float>())
	{
		const GUID componentID = std::hash<std::string>{}(componentName);
		const auto component = dynamic_cast<ScriptComponent*>(
			m_pComponentRegistry->GetComponent(
				componentID
			)
		);
		component->DefineField(fieldName, fieldDefaultValue.as<float>());
	}
}
