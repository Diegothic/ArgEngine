#include "GUID.h"

const Arg::GUID Arg::GUID::Empty(0);

Arg::GUID::GUID(uint64_t ID)
	: m_ID(ID)
{

}

Arg::GUID Arg::GUIDGenerator::NewGUID()
{
	return m_Generator.Next() + 1;
}

Arg::GUIDGeneratorData Arg::GUIDGenerator::Archive()
{
	return { m_Generator.GetSeed() };
}

void Arg::GUIDGenerator::Unarchive(GUIDGeneratorData archive)
{
	m_Generator.SetSeed(archive.LastUsedID);
}
