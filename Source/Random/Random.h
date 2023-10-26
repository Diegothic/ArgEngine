#pragma once

#include <cstdint>

#include "LCG.h"

namespace Arg
{
	class Random
	{
	public:
		Random(uint64_t seed = 0);
		~Random() = default;

		uint64_t GetInitialSeed() const { return m_InitialSeed; }
		uint64_t GetSeed() const;
		void SetSeed(uint64_t seed);

		uint64_t Next();
		int32_t NextInt();

	private:
		LCG m_LCG;
		uint64_t m_InitialSeed;
	};
}
