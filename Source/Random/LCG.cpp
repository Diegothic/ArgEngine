#include "LCG.h"

Arg::LCG::LCG(uint64_t m, uint64_t a, uint64_t c)
	: m_Seed(0),
	m_M(m),
	m_A(a),
	m_C(c)
{
}

uint64_t Arg::LCG::Next()
{
	const uint64_t result = (m_A * (m_Seed + m_C)) % m_M;
	m_Seed = result;
	return result;
}
