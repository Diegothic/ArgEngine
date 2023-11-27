#pragma once

#include <arg_pch.hpp>

namespace Arg
{
	namespace Debug
	{
		using TimePoint = std::chrono::time_point<std::chrono::steady_clock>;

		class ScopeTimer
		{
		public:
			ScopeTimer(std::string name);
			~ScopeTimer();

		private:
			std::string m_Name;
			TimePoint m_StartTime;
		};
	}
}
