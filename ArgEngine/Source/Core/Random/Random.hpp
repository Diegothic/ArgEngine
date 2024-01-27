#pragma once

#include <arg_pch.hpp>

#include "LCG.hpp"

namespace Arg
{
	namespace Random
	{
		class Random
		{
		public:
			Random(const uint64_t seed = 0);
			~Random() = default;

			auto GetInitialSeed() const -> const uint64_t& { return m_InitialSeed; }
			auto GetSeed() const-> const uint64_t&;
			void SetSeed(const uint64_t seed);

			auto Next() -> uint64_t;
			auto NextInt() -> int32_t;
			auto NextFloat() -> float;

		private:
			LCG m_LCG;
			uint64_t m_InitialSeed;
		};
	}
}
