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

	if (state[m_Name]["_BeginPlay"].valid())
	{
		state[m_Name]["_BeginPlay"](scriptInstances[m_OwnerIDString]);
	}
}

void Arg::Script::ScriptComponent::VTick(const Gameplay::GameTime& gameTime)
{
	ActorComponent::VTick(gameTime);

	auto& state = m_pScriptEngine->GetState();
	sol::table scriptInstances = state[m_Name]["_instances"];

	if (state[m_Name]["_Tick"].valid())
	{
		state[m_Name]["_Tick"](scriptInstances[m_OwnerIDString], gameTime.GetDeltaTime());
	}
}

void Arg::Script::ScriptComponent::VDrawDebug(Renderer::RenderContext& context)
{
	ActorComponent::VDrawDebug(context);

	auto& state = m_pScriptEngine->GetState();
	sol::table scriptInstances = state[m_Name]["_instances"];

	if (state[m_Name]["_OnDrawDebug"].valid())
	{
		state[m_Name]["_OnDrawDebug"](scriptInstances[m_OwnerIDString], context);
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
		componentTable.set_function("Owner", [&](sol::table self)
		{
			return state["ScriptComponent"]["Owner"](self["__component"]);
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

	UpdateScriptFields();

	if (state[m_Name]["_OnCreate"].valid())
	{
		state[m_Name]["_OnCreate"](scriptInstances[m_OwnerIDString]);
	}
}

void Arg::Script::ScriptComponent::VOnComponentRemoved()
{
	auto& state = m_pScriptEngine->GetState();
	sol::table scriptInstances = state[m_Name]["_instances"];

	if (state[m_Name]["_OnDestroy"].valid())
	{
		state[m_Name]["_OnDestroy"](scriptInstances[m_OwnerIDString]);
	}

	scriptInstances[m_OwnerIDString] = sol::nil;
}

auto Arg::Script::ScriptComponent::GetField(size_t index) const -> const ScriptComponentField&
{
	ARG_ASSERT(index < m_pBase->m_Fields.size());
	return m_pBase->m_Fields[index];
}

void Arg::Script::ScriptComponent::SetFieldValue(const std::string& name, float value)
{
	ARG_ASSERT(m_pBase->m_FloatFieldValues.contains(name));
	m_FloatFieldValues[name] = value;

	auto& state = m_pScriptEngine->GetState();
	sol::table scriptInstances = state[m_Name]["_instances"];
	scriptInstances[m_OwnerIDString][name] = value;
}

void Arg::Script::ScriptComponent::GetFieldValue(const std::string& name, float& outValue) const
{
	ARG_ASSERT(m_pBase->m_FloatFieldValues.contains(name));
	outValue = m_FloatFieldValues.contains(name)
		           ? m_FloatFieldValues.at(name)
		           : m_pBase->m_FloatFieldValues.at(name);
}

void Arg::Script::ScriptComponent::DefineField(const std::string& name, const float& defaultValue)
{
	m_Fields.emplace_back(name, ScriptComponentFieldType::FieldFloat);
	m_FloatFieldValues[name] = defaultValue;
}

void Arg::Script::ScriptComponent::SetFieldValue(const std::string& name, int32_t value)
{
	ARG_ASSERT(m_pBase->m_IntegerFieldValues.contains(name));
	m_IntegerFieldValues[name] = value;

	auto& state = m_pScriptEngine->GetState();
	sol::table scriptInstances = state[m_Name]["_instances"];
	scriptInstances[m_OwnerIDString][name] = value;
}

void Arg::Script::ScriptComponent::GetFieldValue(const std::string& name, int32_t& outValue) const
{
	ARG_ASSERT(m_pBase->m_IntegerFieldValues.contains(name));
	outValue = m_IntegerFieldValues.contains(name)
		           ? m_IntegerFieldValues.at(name)
		           : m_pBase->m_IntegerFieldValues.at(name);
}

void Arg::Script::ScriptComponent::DefineField(const std::string& name, const int32_t& defaultValue)
{
	m_Fields.emplace_back(name, ScriptComponentFieldType::FieldInteger);
	m_IntegerFieldValues[name] = defaultValue;
}

void Arg::Script::ScriptComponent::SetFieldValue(const std::string& name, bool value)
{
	ARG_ASSERT(m_pBase->m_BooleanFieldValues.contains(name));
	m_BooleanFieldValues[name] = value;

	auto& state = m_pScriptEngine->GetState();
	sol::table scriptInstances = state[m_Name]["_instances"];
	scriptInstances[m_OwnerIDString][name] = value;
}

void Arg::Script::ScriptComponent::GetFieldValue(const std::string& name, bool& outValue) const
{
	ARG_ASSERT(m_pBase->m_BooleanFieldValues.contains(name));
	outValue = m_BooleanFieldValues.contains(name)
		           ? m_BooleanFieldValues.at(name)
		           : m_pBase->m_BooleanFieldValues.at(name);
}

void Arg::Script::ScriptComponent::DefineField(const std::string& name, const bool& defaultValue)
{
	m_Fields.emplace_back(name, ScriptComponentFieldType::FieldBoolean);
	m_BooleanFieldValues[name] = defaultValue;
}

void Arg::Script::ScriptComponent::SetFieldValue(const std::string& name, Vec3 value)
{
	ARG_ASSERT(m_pBase->m_Vec3FieldValues.contains(name));
	m_Vec3FieldValues[name] = value;

	auto& state = m_pScriptEngine->GetState();
	sol::table scriptInstances = state[m_Name]["_instances"];
	scriptInstances[m_OwnerIDString][name] = value;
}

void Arg::Script::ScriptComponent::GetFieldValue(const std::string& name, Vec3& outValue) const
{
	ARG_ASSERT(m_pBase->m_Vec3FieldValues.contains(name));
	outValue = m_Vec3FieldValues.contains(name)
		           ? m_Vec3FieldValues.at(name)
		           : m_pBase->m_Vec3FieldValues.at(name);
}

void Arg::Script::ScriptComponent::DefineField(const std::string& name, const Vec3& defaultValue)
{
	m_Fields.emplace_back(name, ScriptComponentFieldType::FieldVec3);
	m_Vec3FieldValues[name] = defaultValue;
}

void Arg::Script::ScriptComponent::SetFieldValue(const std::string& name, Gameplay::ActorHandle value)
{
	ARG_ASSERT(m_pBase->m_ActorFieldValues.contains(name));
	m_ActorFieldValues[name] = value;

	auto& state = m_pScriptEngine->GetState();
	sol::table scriptInstances = state[m_Name]["_instances"];
	scriptInstances[m_OwnerIDString][name] = value;
}

void Arg::Script::ScriptComponent::GetFieldValue(const std::string& name, Gameplay::ActorHandle& outValue) const
{
	ARG_ASSERT(m_pBase->m_ActorFieldValues.contains(name));
	outValue = m_ActorFieldValues.contains(name)
		           ? m_ActorFieldValues.at(name)
		           : m_pBase->m_ActorFieldValues.at(name);
}

void Arg::Script::ScriptComponent::DefineField(const std::string& name, const Gameplay::ActorHandle& defaultValue)
{
	m_Fields.emplace_back(name, ScriptComponentFieldType::FieldActor);
	m_ActorFieldValues[name] = defaultValue;
}

void Arg::Script::ScriptComponent::SetFieldValue(const std::string& name, TextureHandle value)
{
	ARG_ASSERT(m_pBase->m_TextureFieldValues.contains(name));
	m_TextureFieldValues[name] = value;

	auto& state = m_pScriptEngine->GetState();
	sol::table scriptInstances = state[m_Name]["_instances"];
	scriptInstances[m_OwnerIDString][name] = value;
}

void Arg::Script::ScriptComponent::GetFieldValue(const std::string& name, TextureHandle& outValue) const
{
	ARG_ASSERT(m_pBase->m_TextureFieldValues.contains(name));
	outValue = m_TextureFieldValues.contains(name)
		           ? m_TextureFieldValues.at(name)
		           : m_pBase->m_TextureFieldValues.at(name);
}

void Arg::Script::ScriptComponent::DefineField(const std::string& name, const TextureHandle& defaultValue)
{
	m_Fields.emplace_back(name, ScriptComponentFieldType::FieldTexture);
	m_TextureFieldValues[name] = defaultValue;
}

void Arg::Script::ScriptComponent::SetFieldValue(const std::string& name, StaticModelHandle value)
{
	ARG_ASSERT(m_pBase->m_StaticModelFieldValues.contains(name));
	m_StaticModelFieldValues[name] = value;

	auto& state = m_pScriptEngine->GetState();
	sol::table scriptInstances = state[m_Name]["_instances"];
	scriptInstances[m_OwnerIDString][name] = value;
}

void Arg::Script::ScriptComponent::GetFieldValue(const std::string& name, StaticModelHandle& outValue) const
{
	ARG_ASSERT(m_pBase->m_StaticModelFieldValues.contains(name));
	outValue = m_StaticModelFieldValues.contains(name)
		           ? m_StaticModelFieldValues.at(name)
		           : m_pBase->m_StaticModelFieldValues.at(name);
}

void Arg::Script::ScriptComponent::DefineField(const std::string& name, const StaticModelHandle& defaultValue)
{
	m_Fields.emplace_back(name, ScriptComponentFieldType::FieldStaticModel);
	m_StaticModelFieldValues[name] = defaultValue;
}

void Arg::Script::ScriptComponent::SetFieldValue(const std::string& name, SkeletonHandle value)
{
	ARG_ASSERT(m_pBase->m_SkeletonFieldValues.contains(name));
	m_SkeletonFieldValues[name] = value;

	auto& state = m_pScriptEngine->GetState();
	sol::table scriptInstances = state[m_Name]["_instances"];
	scriptInstances[m_OwnerIDString][name] = value;
}

void Arg::Script::ScriptComponent::GetFieldValue(const std::string& name, SkeletonHandle& outValue) const
{
	ARG_ASSERT(m_pBase->m_SkeletonFieldValues.contains(name));
	outValue = m_SkeletonFieldValues.contains(name)
		           ? m_SkeletonFieldValues.at(name)
		           : m_pBase->m_SkeletonFieldValues.at(name);
}

void Arg::Script::ScriptComponent::DefineField(const std::string& name, const SkeletonHandle& defaultValue)
{
	m_Fields.emplace_back(name, ScriptComponentFieldType::FieldSkeleton);
	m_SkeletonFieldValues[name] = defaultValue;
}

void Arg::Script::ScriptComponent::SetFieldValue(const std::string& name, SkeletalModelHandle value)
{
	ARG_ASSERT(m_pBase->m_SkeletalModelFieldValues.contains(name));
	m_SkeletalModelFieldValues[name] = value;

	auto& state = m_pScriptEngine->GetState();
	sol::table scriptInstances = state[m_Name]["_instances"];
	scriptInstances[m_OwnerIDString][name] = value;
}

void Arg::Script::ScriptComponent::GetFieldValue(const std::string& name, SkeletalModelHandle& outValue) const
{
	ARG_ASSERT(m_pBase->m_SkeletalModelFieldValues.contains(name));
	outValue = m_SkeletalModelFieldValues.contains(name)
		           ? m_SkeletalModelFieldValues.at(name)
		           : m_pBase->m_SkeletalModelFieldValues.at(name);
}

void Arg::Script::ScriptComponent::DefineField(const std::string& name, const SkeletalModelHandle& defaultValue)
{
	m_Fields.emplace_back(name, ScriptComponentFieldType::FieldSkeletalModel);
	m_SkeletalModelFieldValues[name] = defaultValue;
}

void Arg::Script::ScriptComponent::SetFieldValue(const std::string& name, SkeletalAnimationHandle value)
{
	ARG_ASSERT(m_pBase->m_SkeletalAnimationFieldValues.contains(name));
	m_SkeletalAnimationFieldValues[name] = value;

	auto& state = m_pScriptEngine->GetState();
	sol::table scriptInstances = state[m_Name]["_instances"];
	scriptInstances[m_OwnerIDString][name] = value;
}

void Arg::Script::ScriptComponent::GetFieldValue(const std::string& name, SkeletalAnimationHandle& outValue) const
{
	ARG_ASSERT(m_pBase->m_SkeletalAnimationFieldValues.contains(name));
	outValue = m_SkeletalAnimationFieldValues.contains(name)
		           ? m_SkeletalAnimationFieldValues.at(name)
		           : m_pBase->m_SkeletalAnimationFieldValues.at(name);
}

void Arg::Script::ScriptComponent::DefineField(const std::string& name, const SkeletalAnimationHandle& defaultValue)
{
	m_Fields.emplace_back(name, ScriptComponentFieldType::FieldSkeletalAnimation);
	m_SkeletalAnimationFieldValues[name] = defaultValue;
}

void Arg::Script::ScriptComponent::SetFieldValue(const std::string& name, MaterialHandle value)
{
	ARG_ASSERT(m_pBase->m_MaterialFieldValues.contains(name));
	m_MaterialFieldValues[name] = value;

	auto& state = m_pScriptEngine->GetState();
	sol::table scriptInstances = state[m_Name]["_instances"];
	scriptInstances[m_OwnerIDString][name] = value;
}

void Arg::Script::ScriptComponent::GetFieldValue(const std::string& name, MaterialHandle& outValue) const
{
	ARG_ASSERT(m_pBase->m_MaterialFieldValues.contains(name));
	outValue = m_MaterialFieldValues.contains(name)
		           ? m_MaterialFieldValues.at(name)
		           : m_pBase->m_MaterialFieldValues.at(name);
}

void Arg::Script::ScriptComponent::DefineField(const std::string& name, const MaterialHandle& defaultValue)
{
	m_Fields.emplace_back(name, ScriptComponentFieldType::FieldMaterial);
	m_MaterialFieldValues[name] = defaultValue;
}

void Arg::Script::ScriptComponent::SetFieldValue(const std::string& name, SoundResourceHandle value)
{
	ARG_ASSERT(m_pBase->m_SoundFieldValues.contains(name));
	m_SoundFieldValues[name] = value;

	auto& state = m_pScriptEngine->GetState();
	sol::table scriptInstances = state[m_Name]["_instances"];
	scriptInstances[m_OwnerIDString][name] = value;
}

void Arg::Script::ScriptComponent::GetFieldValue(const std::string& name, SoundResourceHandle& outValue) const
{
	ARG_ASSERT(m_pBase->m_SoundFieldValues.contains(name));
	outValue = m_SoundFieldValues.contains(name)
		           ? m_SoundFieldValues.at(name)
		           : m_pBase->m_SoundFieldValues.at(name);
}

void Arg::Script::ScriptComponent::DefineField(const std::string& name, const SoundResourceHandle& defaultValue)
{
	m_Fields.emplace_back(name, ScriptComponentFieldType::FieldSound);
	m_SoundFieldValues[name] = defaultValue;
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
		case ScriptComponentFieldType::FieldFloat:
			{
				float fieldValue;
				GetFieldValue(field.Name, fieldValue);
				fieldNode["Value"] = fieldValue;
				break;
			}
		case ScriptComponentFieldType::FieldInteger:
			{
				int32_t fieldValue;
				GetFieldValue(field.Name, fieldValue);
				fieldNode["Value"] = fieldValue;
				break;
			}
		case ScriptComponentFieldType::FieldBoolean:
			{
				bool fieldValue;
				GetFieldValue(field.Name, fieldValue);
				fieldNode["Value"] = fieldValue;
				break;
			}
		case ScriptComponentFieldType::FieldVec3:
			{
				Vec3 fieldValue;
				GetFieldValue(field.Name, fieldValue);
				fieldNode["Value"] = fieldValue;
				break;
			}
		case ScriptComponentFieldType::FieldActor:
			{
				Gameplay::ActorHandle fieldValue;
				GetFieldValue(field.Name, fieldValue);
				fieldNode["Value"] = fieldValue.GetActorID();
				break;
			}
		case ScriptComponentFieldType::FieldTexture:
			{
				TextureHandle fieldValue;
				GetFieldValue(field.Name, fieldValue);
				fieldNode["Value"] = fieldValue.GetID();
				break;
			}
		case ScriptComponentFieldType::FieldStaticModel:
			{
				StaticModelHandle fieldValue;
				GetFieldValue(field.Name, fieldValue);
				fieldNode["Value"] = fieldValue.GetID();
				break;
			}
		case ScriptComponentFieldType::FieldSkeleton:
			{
				SkeletonHandle fieldValue;
				GetFieldValue(field.Name, fieldValue);
				fieldNode["Value"] = fieldValue.GetID();
				break;
			}
		case ScriptComponentFieldType::FieldSkeletalModel:
			{
				SkeletalModelHandle fieldValue;
				GetFieldValue(field.Name, fieldValue);
				fieldNode["Value"] = fieldValue.GetID();
				break;
			}
		case ScriptComponentFieldType::FieldSkeletalAnimation:
			{
				SkeletalAnimationHandle fieldValue;
				GetFieldValue(field.Name, fieldValue);
				fieldNode["Value"] = fieldValue.GetID();
				break;
			}
		case ScriptComponentFieldType::FieldMaterial:
			{
				MaterialHandle fieldValue;
				GetFieldValue(field.Name, fieldValue);
				fieldNode["Value"] = fieldValue.GetID();
				break;
			}
		case ScriptComponentFieldType::FieldSound:
			{
				SoundResourceHandle fieldValue;
				GetFieldValue(field.Name, fieldValue);
				fieldNode["Value"] = fieldValue.GetID();
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
			case ScriptComponentFieldType::FieldFloat:
				{
					m_FloatFieldValues[fieldName] = ValueOr<float>(fieldNode["Value"], 0.0f);
					break;
				}
			case ScriptComponentFieldType::FieldInteger:
				{
					m_IntegerFieldValues[fieldName] = ValueOr<int32_t>(fieldNode["Value"], 0);
					break;
				}
			case ScriptComponentFieldType::FieldBoolean:
				{
					m_BooleanFieldValues[fieldName] = ValueOr<bool>(fieldNode["Value"], false);
					break;
				}
			case ScriptComponentFieldType::FieldVec3:
				{
					m_Vec3FieldValues[fieldName] = ValueOr<Vec3>(fieldNode["Value"], Math::VEC3_ZERO);
					break;
				}
			case ScriptComponentFieldType::FieldActor:
				{
					m_ActorFieldValues[fieldName] = Gameplay::ActorHandle(
						GetOwner()->GetWorld(),
						ValueOr<GUID>(fieldNode["Value"], GUID::Empty)
					);
					break;
				}
			case ScriptComponentFieldType::FieldTexture:
				{
					m_TextureFieldValues[fieldName] = TextureHandle(
						ValueOr<GUID>(fieldNode["Value"], GUID::Empty),
						GetResourceCache()
					);
					break;
				}
			case ScriptComponentFieldType::FieldStaticModel:
				{
					m_StaticModelFieldValues[fieldName] = StaticModelHandle(
						ValueOr<GUID>(fieldNode["Value"], GUID::Empty),
						GetResourceCache()
					);
					break;
				}
			case ScriptComponentFieldType::FieldSkeleton:
				{
					m_SkeletonFieldValues[fieldName] = SkeletonHandle(
						ValueOr<GUID>(fieldNode["Value"], GUID::Empty),
						GetResourceCache()
					);
					break;
				}
			case ScriptComponentFieldType::FieldSkeletalModel:
				{
					m_SkeletalModelFieldValues[fieldName] = SkeletalModelHandle(
						ValueOr<GUID>(fieldNode["Value"], GUID::Empty),
						GetResourceCache()
					);
					break;
				}
			case ScriptComponentFieldType::FieldSkeletalAnimation:
				{
					m_SkeletalAnimationFieldValues[fieldName] = SkeletalAnimationHandle(
						ValueOr<GUID>(fieldNode["Value"], GUID::Empty),
						GetResourceCache()
					);
					break;
				}
			case ScriptComponentFieldType::FieldMaterial:
				{
					m_MaterialFieldValues[fieldName] = MaterialHandle(
						ValueOr<GUID>(fieldNode["Value"], GUID::Empty),
						GetResourceCache()
					);
					break;
				}
			case ScriptComponentFieldType::FieldSound:
				{
					m_SoundFieldValues[fieldName] = SoundResourceHandle(
						ValueOr<GUID>(fieldNode["Value"], GUID::Empty),
						GetResourceCache()
					);
					break;
				}
			}
		}
	}

	return true;
}

void Arg::Script::ScriptComponent::UpdateScriptFields()
{
	auto& state = m_pScriptEngine->GetState();
	sol::table scriptInstances = state[m_Name]["_instances"];
	const std::string& ownerIDString = m_OwnerIDString;

	auto Fn_ScriptSetFieldValues = [&]<typename T>(
		std::unordered_map<std::string, T> baseValues,
		std::unordered_map<std::string, T> currentValues)
	{
		for (const auto& fieldName : baseValues | std::ranges::views::keys)
		{
			scriptInstances[ownerIDString][fieldName] = currentValues.contains(fieldName)
				                                              ? currentValues.at(fieldName)
				                                              : baseValues.at(fieldName);
		}
	};

	Fn_ScriptSetFieldValues(m_pBase->m_FloatFieldValues, m_FloatFieldValues);
	Fn_ScriptSetFieldValues(m_pBase->m_IntegerFieldValues, m_IntegerFieldValues);
	Fn_ScriptSetFieldValues(m_pBase->m_BooleanFieldValues, m_BooleanFieldValues);
	Fn_ScriptSetFieldValues(m_pBase->m_Vec3FieldValues, m_Vec3FieldValues);
	Fn_ScriptSetFieldValues(m_pBase->m_ActorFieldValues, m_ActorFieldValues);
	Fn_ScriptSetFieldValues(m_pBase->m_TextureFieldValues, m_TextureFieldValues);
	Fn_ScriptSetFieldValues(m_pBase->m_StaticModelFieldValues, m_StaticModelFieldValues);
	Fn_ScriptSetFieldValues(m_pBase->m_SkeletonFieldValues, m_SkeletonFieldValues);
	Fn_ScriptSetFieldValues(m_pBase->m_SkeletalModelFieldValues, m_SkeletalModelFieldValues);
	Fn_ScriptSetFieldValues(m_pBase->m_SkeletalAnimationFieldValues, m_SkeletalAnimationFieldValues);
	Fn_ScriptSetFieldValues(m_pBase->m_MaterialFieldValues, m_MaterialFieldValues);
	Fn_ScriptSetFieldValues(m_pBase->m_SoundFieldValues, m_SoundFieldValues);

	auto Fn_ScriptRefreshFieldValues = [&]<typename T>(
		std::unordered_map<std::string, T> baseValues,
		std::unordered_map<std::string, T> currentValues)
	{
		std::vector<std::string> fieldsToRemove;
		fieldsToRemove.reserve(currentValues.size());
		for (const auto& fieldName : currentValues | std::ranges::views::keys)
		{
			if (!baseValues.contains(fieldName))
			{
				fieldsToRemove.push_back(fieldName);
			}
		}

		for (const auto& fieldName : fieldsToRemove)
		{
			currentValues.erase(fieldName);
		}
	};

	Fn_ScriptRefreshFieldValues(m_pBase->m_FloatFieldValues, m_FloatFieldValues);
	Fn_ScriptRefreshFieldValues(m_pBase->m_IntegerFieldValues, m_IntegerFieldValues);
	Fn_ScriptRefreshFieldValues(m_pBase->m_BooleanFieldValues, m_BooleanFieldValues);
	Fn_ScriptRefreshFieldValues(m_pBase->m_Vec3FieldValues, m_Vec3FieldValues);
	Fn_ScriptRefreshFieldValues(m_pBase->m_ActorFieldValues, m_ActorFieldValues);
	Fn_ScriptRefreshFieldValues(m_pBase->m_TextureFieldValues, m_TextureFieldValues);
	Fn_ScriptRefreshFieldValues(m_pBase->m_StaticModelFieldValues, m_StaticModelFieldValues);
	Fn_ScriptRefreshFieldValues(m_pBase->m_SkeletonFieldValues, m_SkeletonFieldValues);
	Fn_ScriptRefreshFieldValues(m_pBase->m_SkeletalModelFieldValues, m_SkeletalModelFieldValues);
	Fn_ScriptRefreshFieldValues(m_pBase->m_SkeletalAnimationFieldValues, m_SkeletalAnimationFieldValues);
	Fn_ScriptRefreshFieldValues(m_pBase->m_MaterialFieldValues, m_MaterialFieldValues);
	Fn_ScriptRefreshFieldValues(m_pBase->m_SoundFieldValues, m_SoundFieldValues);
}
