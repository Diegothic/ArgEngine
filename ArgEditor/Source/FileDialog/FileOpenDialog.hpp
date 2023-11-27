#pragma once

#include <arg_pch.hpp>

namespace Arg
{
	namespace Editor
	{
		class FileOpenDialog
		{
		public:
			auto GetFile(std::filesystem::path& outResult) -> bool;
			auto GetDirectory(std::filesystem::path& outResult) -> bool;
		};
	}
}
