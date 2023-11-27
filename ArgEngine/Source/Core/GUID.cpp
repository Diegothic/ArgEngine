#include <arg_pch.hpp>
#include "GUID.hpp"

const Arg::GUID Arg::GUID::Empty(0);

Arg::GUID::GUID(uint64_t ID)
	: m_ID(ID)
{

}

auto Arg::GUIDGenerator::NewGUID() -> GUID
{
	return m_Generator.Next() + 1;
}
