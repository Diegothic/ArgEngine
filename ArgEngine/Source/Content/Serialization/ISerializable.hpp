#pragma once

#include <arg_pch.hpp>

namespace Arg
{
	namespace Content
	{
		class ISerializable
		{
		public:
			virtual auto Serialize(const std::filesystem::path& destination) const -> bool = 0;
			virtual auto Deserialize(const std::filesystem::path& source) -> bool = 0;
		};
	}
}
