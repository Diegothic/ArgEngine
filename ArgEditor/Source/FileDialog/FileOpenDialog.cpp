#include <arg_pch.hpp>
#include "FileOpenDialog.hpp"

#include <shobjidl.h>

auto Arg::Editor::FileOpenDialog::GetFile(std::filesystem::path& outResult) -> bool
{
	bool result = false;
	HRESULT hr = CoInitializeEx(
		NULL,
		COINIT_APARTMENTTHREADED
		| COINIT_DISABLE_OLE1DDE
	);
	if (SUCCEEDED(hr))
	{
		IFileOpenDialog* pFileOpen;
		hr = CoCreateInstance(
			CLSID_FileOpenDialog,
			NULL,
			CLSCTX_ALL,
			IID_IFileOpenDialog,
			reinterpret_cast<void**>(&pFileOpen)
		);

		if (SUCCEEDED(hr))
		{
			hr = pFileOpen->Show(NULL);
			if (SUCCEEDED(hr))
			{
				IShellItem* pItem;
				hr = pFileOpen->GetResult(&pItem);
				if (SUCCEEDED(hr))
				{
					PWSTR pszFilePath;
					hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

					if (SUCCEEDED(hr))
					{
						outResult = pszFilePath;
						result = true;
						CoTaskMemFree(pszFilePath);
					}
					pItem->Release();
				}
			}
			pFileOpen->Release();
		}
		CoUninitialize();
	}

	return result;
}

auto Arg::Editor::FileOpenDialog::GetDirectory(std::filesystem::path& outResult) -> bool
{
	bool result = false;
	HRESULT hr = CoInitializeEx(
		NULL,
		COINIT_APARTMENTTHREADED
		| COINIT_DISABLE_OLE1DDE
	);
	if (SUCCEEDED(hr))
	{
		IFileOpenDialog* pFileOpen;
		hr = CoCreateInstance(
			CLSID_FileOpenDialog,
			NULL,
			CLSCTX_ALL,
			IID_IFileOpenDialog,
			reinterpret_cast<void**>(&pFileOpen)
		);

		FILEOPENDIALOGOPTIONS opt{};
		pFileOpen->GetOptions(&opt);
		pFileOpen->SetOptions(opt | FOS_PICKFOLDERS | FOS_PATHMUSTEXIST | FOS_FORCEFILESYSTEM);

		if (SUCCEEDED(hr))
		{
			hr = pFileOpen->Show(NULL);
			if (SUCCEEDED(hr))
			{
				IShellItem* pItem;
				hr = pFileOpen->GetResult(&pItem);
				if (SUCCEEDED(hr))
				{
					PWSTR pszFilePath;
					hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

					if (SUCCEEDED(hr))
					{
						outResult = pszFilePath;
						result = true;
						CoTaskMemFree(pszFilePath);
					}
					pItem->Release();
				}
			}
			pFileOpen->Release();
		}
		CoUninitialize();
	}

	return result;
}
