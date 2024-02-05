#pragma once

#include <arg_pch.hpp>

#include "Core/GUID.hpp"
#include "Core/Random/Random.hpp"

namespace Arg
{
	class EventListenerHandle;

	template <typename TSignature>
	class Event;

	template <typename TReturn, typename... TArgs>
	class Event<TReturn(TArgs...)>
	{
		using FnType = std::function<TReturn(TArgs...)>;

	private:
		struct EventListener
		{
			GUID ID;
			FnType Fn;
		};

	public:
		void Invoke(TArgs... args) const;

		auto AddListener(const FnType& listener) -> EventListenerHandle;
		void RemoveListener(const EventListenerHandle& handle);
		void Clear();

	private:
		Random::Random m_Generator;
		std::vector<EventListener> m_Listeners;
	};

	class EventListenerHandle
	{
	private:
		EventListenerHandle(const GUID& ID) : m_ID(ID)
		{
		}

	private:
		GUID m_ID;

	private:
		template <typename T>
		friend class Event;
	};

	template <typename TReturn, typename... TArgs>
	void Event<TReturn(TArgs...)>::Invoke(TArgs... args) const
	{
		for (const auto& listener : m_Listeners)
		{
			listener.Fn(args...);
		}
	}

	template <typename TReturn, typename... TArgs>
	auto Event<TReturn(TArgs...)>::AddListener(const FnType& listener) -> EventListenerHandle
	{
		GUID ID = m_Generator.Next();
		m_Listeners.emplace_back(ID, listener);
		return {ID};
	}

	template <typename TReturn, typename... TArgs>
	void Event<TReturn(TArgs...)>::RemoveListener(const EventListenerHandle& handle)
	{
		std::erase_if(m_Listeners, [&](const EventListener& listener)
		{
			return listener.ID == handle.m_ID;
		});
	}

	template <typename TReturn, typename... TArgs>
	void Event<TReturn(TArgs...)>::Clear()
	{
		m_Listeners.clear();
		m_Generator = Random::Random();
	}
}
