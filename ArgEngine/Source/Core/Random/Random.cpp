#include <arg_pch.hpp>
#include "Random.hpp"

Arg::Random::Random::Random(const uint64_t seed)
	: m_LCG(34359738368, 3141592653, 2718281829),
	m_InitialSeed(seed)
{
	m_LCG.SetSeed(seed);
}

auto Arg::Random::Random::GetSeed() const -> const uint64_t&
{
	return m_LCG.GetSeed();
}

void Arg::Random::Random::SetSeed(const uint64_t seed)
{
	m_InitialSeed = seed;
	m_LCG.SetSeed(seed);
}

auto Arg::Random::Random::Next() -> uint64_t
{
	return m_LCG.Next();
}

auto Arg::Random::Random::NextInt() -> int32_t
{
	return static_cast<int32_t>(m_LCG.Next());
}

auto Arg::Random::Random::NextFloat() -> float
{
	const int32_t random = static_cast<int32_t>(m_LCG.Next());
	return static_cast<float>(random % 1000) / 1000.0f;
}
