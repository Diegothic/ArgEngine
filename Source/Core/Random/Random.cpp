#include "Random.h"

Arg::Random::Random(uint64_t seed)
	: m_LCG(34359738368, 3141592653, 2718281829),
	m_InitialSeed(seed)
{
}

uint64_t Arg::Random::GetSeed() const
{
	return m_LCG.GetSeed();
}

void Arg::Random::SetSeed(uint64_t seed)
{
	m_InitialSeed = seed;
	m_LCG.SetSeed(seed);
}

uint64_t Arg::Random::Next()
{
	return m_LCG.Next();
}

int32_t Arg::Random::NextInt()
{
	return static_cast<int32_t>(m_LCG.Next());
}
