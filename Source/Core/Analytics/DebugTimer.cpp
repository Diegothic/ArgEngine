#include "DebugTimer.h"

#include "Debug/CoreLogger.h"


Arg::DebugTimer::DebugTimer(std::string name)
	: m_Name(std::move(name))
{
	m_StartTime = std::chrono::high_resolution_clock::now();
}

Arg::DebugTimer::~DebugTimer()
{
	const std::chrono::time_point<std::chrono::steady_clock> endTime = std::chrono::high_resolution_clock::now();
	const std::chrono::duration<double> duration = endTime - m_StartTime;
	AE_CORE_LOG_INFO("[Timer] \"%s\": %fs", m_Name.c_str(), duration.count());
}
