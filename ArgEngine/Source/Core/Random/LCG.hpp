#pragma once

#include <arg_pch.hpp>

namespace Arg
{
	namespace Random
	{
		class LCG
		{
		public:
			LCG(uint64_t m, uint64_t a, uint64_t c);
			~LCG() = default;

			auto GetSeed() const -> const uint64_t& { return m_Seed; }
			void SetSeed(uint64_t seed) { m_Seed = seed; }

			auto Next() -> uint64_t;

		private:
			uint64_t m_Seed;
			uint64_t m_M;
			uint64_t m_A;
			uint64_t m_C;
		};
	}
}
