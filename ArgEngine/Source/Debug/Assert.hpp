#pragma once

#include <arg_pch.hpp>

#include "Log.hpp"

#ifdef ARG_DEBUG

#define ARG_ASSERT_M(Check, Message)\
	{\
		if (!(Check))\
		{\
			ARG_CONSOLE_LOG_ERR(\
			"Assertion \"" #Check "\" failed:\n%s",\
			##Message)\
			__debugbreak();\
		}\
	}

#define ARG_ASSERT(Check)\
	{\
		if (!(Check))\
		{\
			ARG_CONSOLE_LOG_ERR(\
			"Assertion \"" #Check "\" failed!\n"\
			)\
			__debugbreak();\
		}\
	}

#else

#define ARG_ASSERT_M(Check, Message)

#define ARG_ASSERT(Check)

#endif // ARG_DEBUG
