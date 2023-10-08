#pragma once

#include <memory>

namespace Arg
{
	template<typename T>
	using Box = std::unique_ptr<T>;

	template<typename T, typename ... Args>
	constexpr Box<T> NewBox(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	using Rc = std::shared_ptr<T>;

	template<typename T, typename ... Args>
	constexpr Rc<T> NewRc(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}
}
