#pragma once

#include <arg_pch.hpp>

namespace Arg
{
	namespace Editor
	{
		namespace Dialog
		{
			class MessageBoxDialog
			{
			public:
				static void ShowInfo(
					const std::string& text,
					const std::string& caption = std::string()
				);
				static void ShowWarning(
					const std::string& text,
					const std::string& caption = std::string()
				);
				static void ShowError(
					const std::string& text,
					const std::string& caption = std::string()
				);
				static auto AskUser(
					const std::string& text,
					const std::string& caption = std::string()
				) -> bool;

			private:
				MessageBoxDialog();
			};
		}
	}
}
