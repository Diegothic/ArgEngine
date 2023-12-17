#pragma once

#include <arg_pch.hpp>

#include "Log.hpp"

#ifdef ARG_DEBUG

#define ARG_ASSERT(Check, Message)\
	{\
		if (!(Check))\
		{\
			ARG_CONSOLE_LOG_ERR(\
			"Assertion \"" #Check "\" failed:\n%s",\
			##Message)\
			__debugbreak();\
		}\
	}

#else

#define ARG_ASSERT(Check, Message)

#endif // ARG_DEBUG
