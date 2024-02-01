#include <arg_pch.hpp>
#include "ScriptEngine.hpp"

#include "ScriptComponent.hpp"
#include "Gameplay/World/Actor/ActorHandle.hpp"
#include "Gameplay/World/Actor/Component/ActorComponentHandle.hpp"
#include "ScriptGlue/ScriptGlueContent.hpp"
#include "ScriptGlue/ScriptGlueCore.hpp"
#include "ScriptGlue/ScriptGlueGameplay.hpp"
#include "ScriptGlue/ScriptGlueRenderer.hpp"

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

	ScriptExport_Resources(*this);

	ScriptExport_World(*this);
	ScriptExport_Actor(*this);
	ScriptExport_ActorComponents_Graphics(*this);

	ScriptExport_Renderer(*this);

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
		"_OnCreate", [](ScriptComponentHandle&)
		{
		},
		"_BeginPlay", [](ScriptComponentHandle&)
		{
		},
		"_Tick", [](ScriptComponentHandle&, float)
		{
		},
		"_OnDrawDebug", [](ScriptComponentHandle&, Renderer::RenderContext& context)
		{
		},
		"_OnDestroy", [](ScriptComponentHandle&)
		{
		},
		"Owner",
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
	componentTable["__index"] = (*m_pLuaState)[componentName];

	(*m_pLuaState)[componentName]["_instances"].get_or_create<sol::table>();

	m_pComponentRegistry->RegisterComponent(componentPrototype);
}

void Arg::Script::ScriptEngine::DefineField(
	const std::string& componentName,
	const std::string& fieldName,
	const sol::object& fieldDefaultValue
)
{
	const GUID componentID = std::hash<std::string>{}(componentName);
	const auto component = dynamic_cast<ScriptComponent*>(
		m_pComponentRegistry->GetComponent(
			componentID
		)
	);
	if (fieldDefaultValue.is<float>())
	{
		component->DefineField(fieldName, fieldDefaultValue.as<float>());
	}
	else if (fieldDefaultValue.is<int32_t>())
	{
		component->DefineField(fieldName, fieldDefaultValue.as<int32_t>());
	}
	else if (fieldDefaultValue.is<bool>())
	{
		component->DefineField(fieldName, fieldDefaultValue.as<bool>());
	}
	else if (fieldDefaultValue.is<Vec3>())
	{
		component->DefineField(fieldName, fieldDefaultValue.as<Vec3>());
	}
	else if (fieldDefaultValue.is<Gameplay::ActorHandle>())
	{
		component->DefineField(fieldName, fieldDefaultValue.as<Gameplay::ActorHandle>());
	}
	else if (fieldDefaultValue.is<TextureHandle>())
	{
		component->DefineField(fieldName, fieldDefaultValue.as<TextureHandle>());
	}
	else if (fieldDefaultValue.is<StaticModelHandle>())
	{
		component->DefineField(fieldName, fieldDefaultValue.as<StaticModelHandle>());
	}
	else if (fieldDefaultValue.is<SkeletonHandle>())
	{
		component->DefineField(fieldName, fieldDefaultValue.as<SkeletonHandle>());
	}
	else if (fieldDefaultValue.is<SkeletalModelHandle>())
	{
		component->DefineField(fieldName, fieldDefaultValue.as<SkeletalModelHandle>());
	}
	else if (fieldDefaultValue.is<SkeletalAnimationHandle>())
	{
		component->DefineField(fieldName, fieldDefaultValue.as<SkeletalAnimationHandle>());
	}
	else if (fieldDefaultValue.is<MaterialHandle>())
	{
		component->DefineField(fieldName, fieldDefaultValue.as<MaterialHandle>());
	}
	else if (fieldDefaultValue.is<SoundResourceHandle>())
	{
		component->DefineField(fieldName, fieldDefaultValue.as<SoundResourceHandle>());
	}
}
