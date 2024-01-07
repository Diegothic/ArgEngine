#pragma once

#include <arg_pch.hpp>

#include "Core/GUID.hpp"
#include "ActorComponent.hpp"

namespace Arg
{
	namespace Gameplay
	{
		class ComponentRegistry
		{
		public:
			auto CreateComponent(
				const std::string& componentID
			) const->std::shared_ptr<ActorComponent>;
			auto CreateComponent(
				const GUID& componentID
			) const->std::shared_ptr<ActorComponent>;

			void RegisterComponents();
			void RegisterComponent(ActorComponent* prototype);
			void Clear();

			auto ContainsComponent(const GUID& componentID) const -> bool;

			auto GetComponent(const GUID& componentID) const -> ActorComponent*;
			auto GetComponentCount() const -> size_t;
			auto GetComponentID(size_t index) const -> GUID;
			auto GetComponentName(size_t index) const -> const std::string&;

		private:
			std::unordered_map<GUID, std::unique_ptr<ActorComponent>> m_ComponentRegistry;
			std::vector<GUID> m_ComponentIDs;
		};
	}
}
