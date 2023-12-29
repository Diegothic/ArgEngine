#pragma once

#include <arg_pch.hpp>

#include <sol/sol.hpp>

#include "Gameplay/World/Actor/ActorHandle.hpp"
#include "Gameplay/World/Actor/Component/ActorComponent.hpp"

namespace Arg
{
	namespace Script
	{
		class ScriptEngine;

		class ScriptComponent : public Gameplay::ActorComponent
		{
		public:
			enum ScriptComponentFieldType
			{
				Float
			};

			struct ScriptComponentField
			{
				std::string Name;
				ScriptComponentFieldType Type;
			};

		public:
			ScriptComponent(
				ScriptEngine* pScriptEngine,
				const std::string& name
			);

			ScriptComponent(ScriptComponent* pBase);

			auto VCreateDefault() -> std::shared_ptr<ActorComponent> override;

			auto VGetID() const -> GUID override;
			auto VGetName() const -> const std::string& override;

			void VBeginPlay() override;
			void VTick(const Gameplay::GameTime& gameTime) override;

			void VOnComponentAdded() override;
			void VOnComponentRemoved() override;

			auto GetFieldsCount() const -> size_t { return m_pBase->m_Fields.size(); }
			auto GetField(size_t index) const -> const ScriptComponentField&;

			void SetFieldValue(const std::string& name, float value);
			void GetFieldValue(const std::string& name, float& outValue) const;
			void DefineField(const std::string& name, const float& defaultValue);

		protected:
			auto VOnSerialize(YAML::Node& node) const -> bool override;
			auto VOnDeserialize(const YAML::Node& node) -> bool override;

		private:
			ScriptEngine* m_pScriptEngine = nullptr;

			GUID m_ID = GUID::Empty;
			std::string m_Name;
			std::string m_OwnerIDString;

			ScriptComponent* m_pBase = nullptr;

			std::vector<ScriptComponentField> m_Fields;
			std::unordered_map<std::string, float> m_FloatFieldValues;
		};
	}
}
