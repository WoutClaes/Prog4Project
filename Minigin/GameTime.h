#pragma once
#include <chrono>
#include <algorithm>

namespace dae
{
	class GameTime final
	{
	public:
		static GameTime& GetInstance()
		{
			static GameTime instance;
			return instance;
		}

		void Update()
		{
			using namespace std::chrono;

			auto now = high_resolution_clock::now();
			m_DeltaTime = duration<float>(now - m_LastTime).count();
			m_LastTime = now;
			m_Accumulator += m_DeltaTime;
		}

		void ConsumeFixedStep() { m_Accumulator -= m_FixedTimeStep; }

		bool NeedsFixedUpdate() const { return m_Accumulator >= m_FixedTimeStep; }

		float GetDeltaTime() const { return m_DeltaTime; }
		float GetFixedTimeStep()  const { return m_FixedTimeStep; }

		GameTime(const GameTime&) = delete;
		GameTime& operator=(const GameTime&) = delete;
		GameTime(GameTime&&) = delete;
		GameTime& operator=(GameTime&&) = delete;

	private:
		GameTime()
			: m_LastTime(std::chrono::high_resolution_clock::now())
			, m_DeltaTime(0.f)
			, m_Accumulator(0.f)
			, m_FixedTimeStep(0.02f) // 50 fixed updates per second
		{}

		std::chrono::high_resolution_clock::time_point m_LastTime;
		float m_DeltaTime;
		float m_Accumulator;
		float m_FixedTimeStep;
	};
}