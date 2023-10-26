#pragma once
#include <cstdint>

namespace Arg
{
	class Inspector
	{
	public:
		uint64_t GetInspectedItemID() const { return m_InspectedItemID; }

		void SetInspectedItem(uint64_t itemID) 
		{
			m_InspectedItemID = itemID;
		}

	private:
		uint64_t m_InspectedItemID = 0;
	};
}
