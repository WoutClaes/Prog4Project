#pragma once
#include "Components/GameComponent.h"
#include <vector>

namespace dae
{
	struct BenchTransform
	{
		float matrix[16] = {
			1,0,0,0,
			0,1,0,0,
			0,0,1,0,
			0,0,0,1
		};
	};

	struct GameObject3D
	{
		BenchTransform transform;
		int ID{};
	};

	struct GameObject3DAlt
	{
		int ID{};
	};

	class CacheBenchmarkComponent final : public GameComponent
	{
	public:
		explicit CacheBenchmarkComponent(GameObject* owner);
		~CacheBenchmarkComponent() override = default;

		void Update()       override {}
		void FixedUpdate()  override {}
		void Render() const override;

		CacheBenchmarkComponent(const CacheBenchmarkComponent&) = delete;
		CacheBenchmarkComponent(CacheBenchmarkComponent&&) = delete;
		CacheBenchmarkComponent& operator=(const CacheBenchmarkComponent&) = delete;
		CacheBenchmarkComponent& operator=(CacheBenchmarkComponent&&) = delete;

	private:
		// -- Exercise 1 --
		void RunExercise1();
		static std::vector<float> BenchmarkInts(int numSamples);

		// -- Exercise 2 --
		static std::vector<float> BenchmarkGameObject3D(int numSamples);
		static std::vector<float> BenchmarkGameObject3DAlt(int numSamples);

		template<typename Func>
		static std::vector<float> RunBenchmark(Func benchmark, int numSamples);

		int m_samplesEx1{ 10 };
		int m_samplesEx2{ 10 };

		std::vector<float> m_timingsEx1{};
		std::vector<float> m_timingsGO3D{};
		std::vector<float> m_timingsGO3DAlt{};
	};
}