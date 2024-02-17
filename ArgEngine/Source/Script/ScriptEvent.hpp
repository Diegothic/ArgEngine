#pragma once

#include <arg_pch.hpp>

#include "Core/Event/Event.hpp"
#include "sol/sol.hpp"

namespace Arg
{
	namespace Script
	{
		class ScriptEvent
		{
		public:
			void Invoke(const sol::table& data) const;

			auto AddListener(
				const sol::function& listener,
				const sol::object& obj
			) -> EventListenerHandle;
			void RemoveListener(const EventListenerHandle& handle);
			void Clear();

		private:
			Event<void(sol::table)> m_Event;
		};
	}
}
