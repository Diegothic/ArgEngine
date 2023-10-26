#pragma once

#include <cstdint>

namespace Arg
{
	class LCG
	{
	public:
		LCG(uint64_t m, uint64_t a, uint64_t c);
		~LCG() = default;

		uint64_t GetSeed() const { return m_Seed; }
		void SetSeed(uint64_t seed) { m_Seed = seed; }

		uint64_t Next();

	private:
		uint64_t m_Seed;
		uint64_t m_M;
		uint64_t m_A;
		uint64_t m_C;
	};
}
