#include <arg_pch.hpp>
#include "MessageBoxDialog.hpp"

void Arg::Editor::Dialog::MessageBoxDialog::ShowInfo(
	const std::string& text,
	const std::string& caption
)
{
	MessageBox(
		NULL,
		std::wstring(text.begin(), text.end()).c_str(),
		std::wstring(caption.begin(), caption.end()).c_str(),
		MB_ICONINFORMATION
	);
}

void Arg::Editor::Dialog::MessageBoxDialog::ShowWarning(
	const std::string& text,
	const std::string& caption
)
{
	MessageBox(
		NULL,
		std::wstring(text.begin(), text.end()).c_str(),
		std::wstring(caption.begin(), caption.end()).c_str(),
		MB_ICONWARNING
	);
}

void Arg::Editor::Dialog::MessageBoxDialog::ShowError(
	const std::string& text,
	const std::string& caption
)
{
	MessageBox(
		NULL,
		std::wstring(text.begin(), text.end()).c_str(),
		std::wstring(caption.begin(), caption.end()).c_str(),
		MB_ICONERROR
	);
}

auto Arg::Editor::Dialog::MessageBoxDialog::AskUser(
	const std::string& text,
	const std::string& caption
) -> bool
{
	int32_t msgboxID = MessageBox(
		NULL,
		std::wstring(text.begin(), text.end()).c_str(),
		std::wstring(caption.begin(), caption.end()).c_str(),
		MB_ICONQUESTION | MB_YESNO
	);

	return msgboxID == IDYES;
}
