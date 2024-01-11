#include <arg_pch.hpp>
#include "ScriptComponent.hpp"

#include "ScriptEngine.hpp"
#include "Gameplay/World/Actor/Component/ActorComponentHandle.hpp"

using ScriptComponentHandle = Arg::Gameplay::ActorComponentHandle<Arg::Script::ScriptComponent>;

Arg::Script::ScriptComponent::ScriptComponent(
	ScriptEngine* pScriptEngine,
	const std::string& name
)
	: m_pScriptEngine(pScriptEngine),
	  m_Name(name)
{
	m_bCanEverRender = false;
	m_ID = std::hash<std::string>{}(m_Name);
}

Arg::Script::ScriptComponent::ScriptComponent(ScriptComponent* pBase)
	: m_pScriptEngine(pBase->m_pScriptEngine),
	  m_Name(pBase->m_Name),
	  m_pBase(pBase)
{
	m_bCanEverRender = false;
	m_ID = std::hash<std::string>{}(m_Name);
}

auto Arg::Script::ScriptComponent::VCreateDefault() -> std::shared_ptr<ActorComponent>
{
	return std::make_shared<ScriptComponent>(this);
}

auto Arg::Script::ScriptComponent::VGetID() const -> GUID
{
	return m_ID;
}

auto Arg::Script::ScriptComponent::VGetName() const -> const std::string&
{
	return m_Name;
}

void Arg::Script::ScriptComponent::VBeginPlay()
{
	ActorComponent::VBeginPlay();

	auto& state = m_pScriptEngine->GetState();
	sol::table scriptInstances = state[m_Name]["_instances"];

	if (state[m_Name]["_begin_play"].valid())
	{
		state[m_Name]["_begin_play"](scriptInstances[m_OwnerIDString]);
	}
}

void Arg::Script::ScriptComponent::VTick(const Gameplay::GameTime& gameTime)
{
	ActorComponent::VTick(gameTime);

	auto& state = m_pScriptEngine->GetState();
	sol::table scriptInstances = state[m_Name]["_instances"];

	if (state[m_Name]["_tick"].valid())
	{
		state[m_Name]["_tick"](scriptInstances[m_OwnerIDString], gameTime.GetDeltaTime());
	}
}

void Arg::Script::ScriptComponent::VOnComponentAdded()
{
	auto& state = m_pScriptEngine->GetState();
	sol::table scriptInstances = state[m_Name]["_instances"];

	Gameplay::GameWorld* pWorld = GetOwner()->GetWorld();
	const GUID& ownerID = GetOwner()->GetID();
	m_OwnerIDString = std::to_string(ownerID);

	sol::table componentTable = scriptInstances[m_OwnerIDString]
		.get_or_create<sol::table>();

	auto componentHandle = ScriptComponentHandle(pWorld, ownerID, m_ID);
	componentTable["__component"] = componentHandle;

	componentTable[sol::metatable_key] = state[m_Name];
	{
		componentTable.set_function("get_owner", [&](sol::table self)
		{
			return state["ScriptComponent"]["get_owner"](self["__component"]);
		});
		componentTable.set_function(
			sol::meta_function::new_index,
			[](sol::table self,
			   const sol::object& key,
			   const sol::object& value)
			{
				self[key] = value;
			});
	}

	for (const auto& fieldName : m_pBase->m_FloatFieldValues | std::ranges::views::keys)
	{
		scriptInstances[m_OwnerIDString][fieldName] = m_FloatFieldValues.contains(fieldName)
			                                              ? m_FloatFieldValues.at(fieldName)
			                                              : m_pBase->m_FloatFieldValues.at(fieldName);
	}

	if (state[m_Name]["_on_create"].valid())
	{
		state[m_Name]["_on_create"](scriptInstances[m_OwnerIDString]);
	}
}

void Arg::Script::ScriptComponent::VOnComponentRemoved()
{
	auto& state = m_pScriptEngine->GetState();
	sol::table scriptInstances = state[m_Name]["_instances"];

	if (state[m_Name]["_on_destroy"].valid())
	{
		state[m_Name]["_on_destroy"](scriptInstances[m_OwnerIDString]);
	}

	scriptInstances[m_OwnerIDString] = sol::nil;
}

auto Arg::Script::ScriptComponent::GetField(size_t index) const -> const ScriptComponentField&
{
	ARG_ASSERT(index < m_pBase->m_Fields.size(), "");
	return m_pBase->m_Fields[index];
}

void Arg::Script::ScriptComponent::SetFieldValue(const std::string& name, float value)
{
	ARG_ASSERT(m_pBase->m_FloatFieldValues.contains(name), "");
	m_FloatFieldValues[name] = value;

	auto& state = m_pScriptEngine->GetState();
	sol::table scriptInstances = state[m_Name]["_instances"];
	scriptInstances[m_OwnerIDString][name] = value;
}

void Arg::Script::ScriptComponent::GetFieldValue(const std::string& name, float& outValue) const
{
	ARG_ASSERT(m_pBase->m_FloatFieldValues.contains(name), "");
	outValue = m_FloatFieldValues.contains(name)
		           ? m_FloatFieldValues.at(name)
		           : m_pBase->m_FloatFieldValues.at(name);
}

void Arg::Script::ScriptComponent::DefineField(const std::string& name, const float& defaultValue)
{
	m_Fields.emplace_back(name, ScriptComponentFieldType::Float);
	m_FloatFieldValues[name] = defaultValue;
}

bool Arg::Script::ScriptComponent::VOnSerialize(YAML::Node& node) const
{
	const bool bIsSuccess = ActorComponent::VOnSerialize(node);
	if (!bIsSuccess)
	{
		return false;
	}

	auto fieldsNode = node["Fields"];
	for (size_t i = 0; i < GetFieldsCount(); i++)
	{
		YAML::Node fieldNode;
		const auto& field = GetField(i);
		fieldNode["Name"] = field.Name;
		fieldNode["Type"] = static_cast<uint64_t>(field.Type);
		switch (field.Type)
		{
		case ScriptComponentFieldType::Float:
			{
				float fieldValue;
				GetFieldValue(field.Name, fieldValue);
				fieldNode["Value"] = fieldValue;
				break;
			}
		}

		fieldsNode[i] = fieldNode;
	}

	node["Fields"] = fieldsNode;

	return true;
}

bool Arg::Script::ScriptComponent::VOnDeserialize(const YAML::Node& node)
{
	const bool bIsSuccess = ActorComponent::VOnDeserialize(node);
	if (!bIsSuccess)
	{
		return false;
	}

	if (node["Fields"])
	{
		const auto& fieldsNode = node["Fields"];
		for (size_t i = 0; i < fieldsNode.size(); i++)
		{
			const auto& fieldNode = fieldsNode[i];
			const std::string fieldName = ValueOr<std::string>(fieldNode["Name"], "Default");
			const ScriptComponentFieldType fieldType = static_cast<ScriptComponentFieldType>(
				ValueOr<uint64_t>(fieldNode["Type"], 0)
			);
			switch (fieldType)
			{
			case ScriptComponentFieldType::Float:
				{
					m_FloatFieldValues[fieldName] = ValueOr<float>(fieldNode["Value"], 0.0f);
					break;
				}
			}
		}
	}

	return true;
}
