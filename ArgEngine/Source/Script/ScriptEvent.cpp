#include <arg_pch.hpp>
#include "ScriptEvent.hpp"

void Arg::Script::ScriptEvent::Invoke(const sol::table& data) const
{
	m_Event.Invoke(data);
}

auto Arg::Script::ScriptEvent::AddListener(
	const sol::function& listener,
	const sol::object& obj
) -> EventListenerHandle
{
	return m_Event.AddListener(
		[listener, obj](const sol::table& data)
		{
			if (listener.valid() && obj.valid())
			{
				listener(obj, data);
			}
		}
	);
}

void Arg::Script::ScriptEvent::RemoveListener(const EventListenerHandle& handle)
{
	m_Event.RemoveListener(handle);
}

void Arg::Script::ScriptEvent::Clear()
{
	m_Event.Clear();
}
