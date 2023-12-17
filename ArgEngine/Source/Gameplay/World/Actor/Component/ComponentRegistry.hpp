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

			void Clear();
			void RegisterComponents();

		private:
			std::unordered_map<GUID, std::unique_ptr<ActorComponent>> m_ComponentRegistry;
		};
	}
}
