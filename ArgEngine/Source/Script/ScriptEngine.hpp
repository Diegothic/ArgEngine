#pragma once

#include <arg_pch.hpp>

#include <sol/sol.hpp>

#include "Gameplay/World/GameWorld.hpp"

namespace Arg
{
	namespace Script
	{
		struct ScriptEngineSpec
		{
			std::filesystem::path RootDirectory;
			std::string SourceFileName;
			Gameplay::ComponentRegistry* ComponentRegistry;
		};

		class ScriptEngine
		{
		public:
			ScriptEngine(const ScriptEngineSpec& spec);

			void Initialize();
			void Rebuild();
			void Load();

			auto GetState() const -> sol::state& { return *m_pLuaState; }

		private:
			void DefineComponent(const std::string& componentName);
			void DefineField(
				const std::string& componentName,
				const std::string& fieldName,
				const sol::object& fieldDefaultValue
			);

		private:
			std::filesystem::path m_RootDirectory;
			std::string m_SourceFileName;

			std::unique_ptr<sol::state> m_pLuaState = nullptr;
			Gameplay::ComponentRegistry* m_pComponentRegistry = nullptr;
		};
	}
}
