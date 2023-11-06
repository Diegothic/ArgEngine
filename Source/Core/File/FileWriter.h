#pragma once

#include <filesystem>
#include <string>

namespace Arg
{
	class FileWriter
	{
	public:
		FileWriter(std::filesystem::path path);

		void Write(const std::string& contents) const;
		void WriteBinary(const char* data, size_t size) const;

	private:
		std::filesystem::path m_Path;
	};
}
