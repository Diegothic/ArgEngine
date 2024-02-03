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

			auto GetAvgFrameTime() const -> const float& { return m_AvgFrameTime; }

			void Tick(float deltaTime)
			{
				m_UnscaledDeltaTime = deltaTime;
				m_DeltaTime = deltaTime * m_TimeScale;
				m_ElapsedTime += m_DeltaTime;

				m_SampleSum += m_UnscaledDeltaTime;
				m_Samples++;
				if (m_Samples >= 10)
				{
					m_AvgFrameTime = m_SampleSum / 10.0f;
					m_Samples = 0;
					m_SampleSum = 0.0f;
				}
			}

		private:
			float m_ElapsedTime = 0.0f;
			float m_DeltaTime = 1.0f / 30.0f;
			float m_UnscaledDeltaTime = 1.0f / 30.0f;
			float m_TimeScale = 1.0f;

			float m_AvgFrameTime = 0.0f;
			float m_SampleSum = 0.0f;
			int32_t m_Samples = 0;
		};
	}
}
