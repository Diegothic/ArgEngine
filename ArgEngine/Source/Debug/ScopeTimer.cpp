#include <arg_pch.hpp>
#include "ScopeTimer.hpp"

#include "Log.hpp"

Arg::Debug::ScopeTimer::ScopeTimer(std::string name)
	: m_Name(std::move(name))
{
	m_StartTime = std::chrono::high_resolution_clock::now();
}

Arg::Debug::ScopeTimer::~ScopeTimer()
{
	const auto endTime = std::chrono::high_resolution_clock::now();
	const std::chrono::duration<double> duration = endTime - m_StartTime;
	ARG_CONSOLE_LOG("[Timer] \"%s\": %fs", m_Name.c_str(), duration.count());
}
