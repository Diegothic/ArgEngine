#include "FileReader.h"

#include <fstream>

Arg::FileReader::FileReader(std::filesystem::path path)
	: m_Path(std::move(path))
{
}


void Arg::FileReader::Read()
{
	std::ifstream fileStream(m_Path, std::ios::ate);
	const std::streamsize fileSize = fileStream.tellg();
	fileStream.seekg(0, std::ios::beg);
	m_Data = NewBox<std::vector<char>>(fileSize);
	fileStream.read(m_Data->data(), fileSize);
	fileStream.close();
}

void Arg::FileReader::ReadBinary()
{
	std::ifstream fileStream(m_Path, std::ios::binary | std::ios::ate);
	const std::streamsize fileSize = fileStream.tellg();
	fileStream.seekg(0, std::ios::beg);
	m_Data = NewBox<std::vector<char>>(fileSize);
	fileStream.read(m_Data->data(), fileSize);
	fileStream.close();
}
