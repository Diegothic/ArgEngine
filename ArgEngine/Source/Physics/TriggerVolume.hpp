#pragma once

#include <arg_pch.hpp>

#include "Core/GUID.hpp"
#include "Core/Math/Math.hpp"

namespace Arg
{
	namespace Physics
	{
		enum TriggerVolumeShape
		{
			TBox,
			TSphere
		};

		enum TriggerVolumeFlag
		{
			BodyEntered = 1 << 0,
			BodyExit = 1 << 1,
		};

		class TriggerVolume
		{
		public:
			TriggerVolume(const GUID& actorID);

		public:
			void Initialize(int32_t userIndex);

		public:
			auto GetActorID() const -> const GUID& { return m_ActorID; }
			auto GetUserIndex() const -> const int32_t& { return m_UserIndex; }

			auto GetFlag(const TriggerVolumeFlag& flag) const -> bool;
			void SetFlag(const TriggerVolumeFlag& flag, bool value);

			void BeginOverlap();
			void AddOverlap(const int32_t& userIndex);
			void AddTriggerEntered(int32_t index);
			void AddTriggerExit(int32_t index);
			auto HasOverlappedWith(const int32_t& userIndex) const -> bool;
			auto HasOverlappedLastWith(const int32_t& userIndex) const -> bool;

			auto GetOverlappedLastCount() const -> size_t;
			auto GetOverlappedLast(size_t index) const -> const int32_t&;

			auto GetTriggerEnteredCount() const -> size_t;
			auto GetTriggerEntered(size_t index) const -> const int32_t&;

			auto GetTriggerExitCount() const -> size_t;
			auto GetTriggerExit(size_t index) const -> const int32_t&;

		public:
			auto GetShape() const -> const TriggerVolumeShape& { return m_Shape; }
			void SetShape(const TriggerVolumeShape& shape);

			auto GetSize() const -> const Vec3& { return m_Size; }
			void SetSize(const Vec3& size);

		private:
			int32_t m_Flags = 0;

			GUID m_ActorID = GUID::Empty;

			int32_t m_UserIndex = -1;

			std::vector<int32_t> m_Overlaps;
			std::vector<int32_t> m_TriggerEntered;
			std::vector<int32_t> m_TriggerExit;
			std::vector<int32_t> m_LastOverlaps;

			TriggerVolumeShape m_Shape = TriggerVolumeShape::TBox;
			Vec3 m_Size = Vec3(1.0f, 1.0f, 1.0f);
		};
	}
}
