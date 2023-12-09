#pragma once

#include <arg_pch.hpp>

namespace Arg
{
	namespace Gameplay
	{
		class GameTime
		{
		public:
			GameTime() = default;

			auto GetElapsedTime() const -> const float& { return m_ElapsedTime; }
			auto GetDeltaTime() const -> const float& { return m_DeltaTime; }
			auto GetUnscaledDeltaTime() const -> const float& { return m_UnscaledDeltaTime; }

			auto GetTimeScale() const -> const float& { return m_TimeScale; }
			void SetTimeScale(const float timeScale) { m_TimeScale = timeScale; }

			void Tick(const float deltaTime)
			{
				m_UnscaledDeltaTime = deltaTime;
				m_DeltaTime = deltaTime * m_TimeScale;
				m_ElapsedTime += m_DeltaTime;
			}

		private:
			float m_ElapsedTime = 0.0f;
			float m_DeltaTime = 1.0f / 30.0f;
			float m_UnscaledDeltaTime = 1.0f / 30.0f;
			float m_TimeScale = 1.0f;
		};
	}
}

