#pragma once

#include <arg_pch.hpp>

#ifdef ARG_DEBUG

#define ARG_CONSOLE_LOG(Format, ...)\
	{\
		printf("\033[1;32m" "ARG::Core::Log: " "\033[0m"\
		Format "\n",\
		##__VA_ARGS__);\
	}

#define ARG_CONSOLE_LOG_WARN(Format, ...)\
	{\
		printf("\033[1;33m" "ARG::Core::Warning: " "\033[0m"\
		Format "\n" "\t(%d) " __FILE__ "\n",\
		##__VA_ARGS__, __LINE__);\
	}

#define ARG_CONSOLE_LOG_ERR(Format, ...)\
	{\
		printf("\033[1;31m" "ARG::Core::Error: " "\033[0m"\
		Format "\n" "\t(%d) " __FILE__ "\n",\
		##__VA_ARGS__, __LINE__);\
	}

#else

#define ARG_CONSOLE_LOG(Format, ...)

#define ARG_CONSOLE_LOG_WARN(Format, ...)

#define ARG_CONSOLE_LOG_ERR(Format, ...)

#endif // ARG_DEBUG
