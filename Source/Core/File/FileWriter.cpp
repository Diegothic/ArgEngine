#include "FileWriter.h"

#include <fstream>

Arg::FileWriter::FileWriter(std::filesystem::path path)
	: m_Path(std::move(path))
{
}

void Arg::FileWriter::Write(const std::string& contents) const
{
	std::ofstream fileStream(m_Path, std::ios::out);
	fileStream.write(contents.c_str(), static_cast<std::streamsize>(contents.size()));
	fileStream.close();
}

void Arg::FileWriter::WriteBinary(const char* data, size_t size) const
{
	std::ofstream fileStream(m_Path, std::ios::out | std::ios::binary);
	fileStream.write(data, static_cast<std::streamsize>(size));
	fileStream.close();
}
