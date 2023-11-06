#pragma once

#include <chrono>

namespace Arg
{
	class DebugTimer
	{
	public:
		DebugTimer(std::string name);
		~DebugTimer();
	private:
		std::string m_Name;
		std::chrono::time_point<std::chrono::steady_clock> m_StartTime;
	};
}
