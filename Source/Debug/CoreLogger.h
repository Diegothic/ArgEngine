#pragma once

#include <cstdio>

#ifndef AE_RELEASE

#define AE_CORE_LOG_INFO(Format, ...)\
{\
	printf("\033[1;32m" "ARG::Core::Info: " "\033[0m"\
	Format "\n",\
	##__VA_ARGS__);\
}

#define AE_CORE_LOG_WARN(Format, ...)\
{\
	printf("\033[1;33m" "ARG::Core::Warning: " "\033[0m"\
	Format "\n" "\t(%d) " __FILE__ "\n",\
	##__VA_ARGS__, __LINE__);\
}

#define AE_CORE_LOG_ERR(Format, ...)\
{\
	printf("\033[1;31m" "ARG::Core::Error: " "\033[0m"\
	Format "\n" "\t(%d) " __FILE__ "\n",\
	##__VA_ARGS__, __LINE__);\
}

#else

#define AE_CORE_LOG_INFO(Format, ...)

#define AE_CORE_LOG_WARN(Format, ...)

#define AE_CORE_LOG_ERR(Format, ...)

#endif // AE_RELEASE
