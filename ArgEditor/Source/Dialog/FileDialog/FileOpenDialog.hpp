#pragma once

#include <arg_pch.hpp>

namespace Arg
{
	namespace Editor
	{
		namespace Dialog
		{
			class FileOpenDialog
			{
			public:
				static auto GetFile(std::filesystem::path& outResult) -> bool;
				static auto GetDirectory(std::filesystem::path& outResult) -> bool;

			private:
				FileOpenDialog();
			};
		}
	}
}
