#pragma once

#include <arg_pch.hpp>

#include "Random/Random.hpp"

namespace Arg
{
	class GUID
	{
	public:
		GUID() = default;
		GUID(uint64_t ID);
		GUID(const GUID& other) = default;
		operator uint64_t() const { return m_ID; }
		auto operator==(const GUID& other) const -> bool { return m_ID == other.m_ID; }

	public:
		static const GUID Empty;

	private:
		uint64_t m_ID = 0;
	};

	class GUIDGenerator
	{
	public:
		auto NewGUID() -> GUID;

	private:
		Random::Random m_Generator;
	};
}

namespace std
{
	template <typename T> struct hash;

	template <>
	struct hash<Arg::GUID>
	{
		auto operator()(const Arg::GUID& ID) const noexcept -> size_t
		{
			return (uint64_t)ID;
		}
	};
}
