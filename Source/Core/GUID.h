#pragma once

#include <cstdint>

#include "Random/Random.h"

namespace Arg
{
	class GUID
	{
	public:
		GUID(uint64_t ID);
		GUID(const GUID& other) = default;
		operator uint64_t() const { return m_ID; }
		bool operator==(const GUID& other) const { return m_ID == other.m_ID; }

	public:
		static const GUID Empty;

	private:
		uint64_t m_ID;
	};

	struct GUIDGeneratorData
	{
		uint64_t LastUsedID;
	};

	class GUIDGenerator
	{
	public:
		GUID NewGUID();

		GUIDGeneratorData Archive();
		void Unarchive(GUIDGeneratorData archive);

	private:
		Random m_Generator;
	};
}
