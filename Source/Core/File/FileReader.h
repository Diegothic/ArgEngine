#pragma once

#include <filesystem>
#include <string>
#include <vector>

#include "Core/Memory/SmartPtr.h"

namespace Arg
{
	class FileReader
	{
	public:
		FileReader(std::filesystem::path path);

		const char* GetData() const { return m_Data.get()->data(); }
		std::string GetContents() const { return { m_Data.get()->data() }; }

		void Read();
		void ReadBinary();

	private:
		std::filesystem::path m_Path;
		Box<std::vector<char>> m_Data = nullptr;
	};
}
