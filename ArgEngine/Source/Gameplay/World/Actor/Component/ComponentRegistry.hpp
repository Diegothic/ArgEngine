#pragma once

#include <arg_pch.hpp>

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

			void RegisterComponents();

		private:
			std::unordered_map<GUID, std::unique_ptr<ActorComponent>> m_ComponentRegistry;
		};
	}
}
