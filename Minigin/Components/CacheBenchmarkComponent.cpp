#include "Components/CacheBenchmarkComponent.h"

#include <imgui.h>
#include <chrono>
#include <algorithm>
#include <numeric>
#include <vector>

namespace dae
{
	static constexpr int   INT_BUFFER_SIZE = 1 << 26;
	static constexpr int   GO_BUFFER_SIZE = 1 << 14;
	static constexpr int   NUM_STEPS = 11;
	static constexpr float X_LABELS[] = { 1,2,4,8,16,32,64,128,256,512,1024 };

	CacheBenchmarkComponent::CacheBenchmarkComponent(GameObject* owner)
		: GameComponent(owner)
	{
	}

	template<typename Func>
	std::vector<float> CacheBenchmarkComponent::RunBenchmark(Func benchmark, int numSamples)
	{
		std::vector<float> results;
		results.reserve(NUM_STEPS);

		for (int step = 1; step <= 1024; step *= 2)
		{
			std::vector<long long> timings;
			timings.reserve(numSamples);

			for (int s = 0; s < numSamples; ++s)
			{
				const auto start = std::chrono::high_resolution_clock::now();
				benchmark(step);
				const auto end = std::chrono::high_resolution_clock::now();
				timings.push_back(
					std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()
				);
			}

			std::sort(timings.begin(), timings.end());
			if (timings.size() > 2)
			{
				timings.erase(timings.begin());
				timings.erase(timings.end() - 1);
			}

			const long long sum = std::accumulate(timings.begin(), timings.end(), 0LL);
			results.push_back(static_cast<float>(sum) / static_cast<float>(timings.size()));
		}

		return results;
	}

	std::vector<float> CacheBenchmarkComponent::BenchmarkInts(int numSamples)
	{
		int* arr = new int[INT_BUFFER_SIZE];
		for (int i = 0; i < INT_BUFFER_SIZE; ++i)
			arr[i] = i;

		auto result = RunBenchmark([arr](int step)
			{
				for (int i = 0; i < INT_BUFFER_SIZE; i += step)
					arr[i] *= 2;
			}, numSamples);

		delete[] arr;
		return result;
	}

	std::vector<float> CacheBenchmarkComponent::BenchmarkGameObject3D(int numSamples)
	{
		std::vector<GameObject3D> objects(GO_BUFFER_SIZE);
		for (int i = 0; i < GO_BUFFER_SIZE; ++i)
			objects[i].ID = i;

		return RunBenchmark([&objects](int step)
			{
				for (int i = 0; i < GO_BUFFER_SIZE; i += step)
					objects[i].ID *= 2;
			}, numSamples);
	}

	std::vector<float> CacheBenchmarkComponent::BenchmarkGameObject3DAlt(int numSamples)
	{
		std::vector<GameObject3DAlt> objects(GO_BUFFER_SIZE);
		for (int i = 0; i < GO_BUFFER_SIZE; ++i)
			objects[i].ID = i;

		return RunBenchmark([&objects](int step)
			{
				for (int i = 0; i < GO_BUFFER_SIZE; i += step)
					objects[i].ID *= 2;
			}, numSamples);
	}

	static void DrawColoredPlot(const std::vector<float>& data, ImVec2 size, ImU32 color)
	{
		if (data.empty()) return;

		const float maxVal = *std::max_element(data.begin(), data.end());
		if (maxVal <= 0.f) return;

		ImVec2 origin = ImGui::GetCursorScreenPos();
		ImGui::InvisibleButton("##plotarea", size);
		ImDrawList* dl = ImGui::GetWindowDrawList();

		dl->AddRectFilled(origin, ImVec2(origin.x + size.x, origin.y + size.y), IM_COL32(40, 40, 40, 255));
		dl->AddRect(origin, ImVec2(origin.x + size.x, origin.y + size.y), IM_COL32(80, 80, 80, 255));

		const int count = static_cast<int>(data.size());
		for (int i = 1; i < count; ++i)
		{
			const float x0 = origin.x + (static_cast<float>(i - 1) / (count - 1)) * size.x;
			const float x1 = origin.x + (static_cast<float>(i) / (count - 1)) * size.x;
			const float y0 = origin.y + size.y - (data[i - 1] / maxVal) * size.y;
			const float y1 = origin.y + size.y - (data[i] / maxVal) * size.y;
			dl->AddLine(ImVec2(x0, y0), ImVec2(x1, y1), color, 2.f);
		}
	}
	static void DrawCombinedPlot(const std::vector<float>& dataA, ImU32 colorA,
		const std::vector<float>& dataB, ImU32 colorB,
		ImVec2 size)
	{
		if (dataA.empty() || dataB.empty()) return;

		const float maxVal = std::max(
			*std::max_element(dataA.begin(), dataA.end()),
			*std::max_element(dataB.begin(), dataB.end())
		);
		if (maxVal <= 0.f) return;

		ImVec2 origin = ImGui::GetCursorScreenPos();
		ImGui::InvisibleButton("##combinedplot", size);
		ImDrawList* dl = ImGui::GetWindowDrawList();

		dl->AddRectFilled(origin, ImVec2(origin.x + size.x, origin.y + size.y), IM_COL32(40, 40, 40, 255));
		dl->AddRect(origin, ImVec2(origin.x + size.x, origin.y + size.y), IM_COL32(80, 80, 80, 255));

		auto drawLine = [&](const std::vector<float>& data, ImU32 col)
			{
				const int count = static_cast<int>(data.size());
				for (int i = 1; i < count; ++i)
				{
					const float x0 = origin.x + (static_cast<float>(i - 1) / (count - 1)) * size.x;
					const float x1 = origin.x + (static_cast<float>(i) / (count - 1)) * size.x;
					const float y0 = origin.y + size.y - (data[i - 1] / maxVal) * size.y;
					const float y1 = origin.y + size.y - (data[i] / maxVal) * size.y;
					dl->AddLine(ImVec2(x0, y0), ImVec2(x1, y1), col, 2.f);
				}
			};

		drawLine(dataA, colorA);
		drawLine(dataB, colorB);
	}

	void CacheBenchmarkComponent::Render() const
	{
		auto* self = const_cast<CacheBenchmarkComponent*>(this);

		constexpr ImU32 COLOR_EX1 = IM_COL32(255, 165, 0, 255); // orange
		constexpr ImU32 COLOR_GO3D = IM_COL32(86, 156, 214, 255); // blue
		constexpr ImU32 COLOR_GO3DALT = IM_COL32(78, 201, 176, 255); // teal

		// ===== Exercise 1 window =====
		ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Once);
		ImGui::Begin("Exercise 1");

		ImGui::InputInt("# samples##ex1", &self->m_samplesEx1);
		if (self->m_samplesEx1 < 1) self->m_samplesEx1 = 1;

		if (ImGui::Button("Thrash the cache##ex1"))
			self->m_timingsEx1 = BenchmarkInts(m_samplesEx1);

		if (!m_timingsEx1.empty())
			DrawColoredPlot(m_timingsEx1, ImVec2(300, 150), COLOR_EX1);

		ImGui::End();

		// ===== Exercise 2 window =====
		ImGui::SetNextWindowPos(ImVec2(340, 10), ImGuiCond_Once);
		ImGui::Begin("Exercise 2");

		ImGui::InputInt("# samples##ex2", &self->m_samplesEx2);
		if (self->m_samplesEx2 < 1) self->m_samplesEx2 = 1;

		if (ImGui::Button("Thrash the cache with GameObject3D"))
			self->m_timingsGO3D = BenchmarkGameObject3D(m_samplesEx2);

		if (ImGui::Button("Thrash the cache with GameObject3DAlt"))
			self->m_timingsGO3DAlt = BenchmarkGameObject3DAlt(m_samplesEx2);

		if (!m_timingsGO3D.empty())
		{
			ImGui::TextColored(ImVec4(0.34f, 0.61f, 0.84f, 1.f), "GameObject3D");
			DrawColoredPlot(m_timingsGO3D, ImVec2(300, 100), COLOR_GO3D);
		}

		if (!m_timingsGO3DAlt.empty())
		{
			ImGui::TextColored(ImVec4(0.31f, 0.79f, 0.69f, 1.f), "GameObject3DAlt");
			DrawColoredPlot(m_timingsGO3DAlt, ImVec2(300, 100), COLOR_GO3DALT);
		}

		if (!m_timingsGO3D.empty() && !m_timingsGO3DAlt.empty())
		{
			ImGui::Separator();
			ImGui::TextColored(ImVec4(0.34f, 0.61f, 0.84f, 1.f), "GO3D");
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(0.31f, 0.79f, 0.69f, 1.f), "  GO3DAlt  ");
			ImGui::SameLine();
			ImGui::Text("(combined)");
			DrawCombinedPlot(m_timingsGO3D, COLOR_GO3D, m_timingsGO3DAlt, COLOR_GO3DALT, ImVec2(300, 120));
		}

		ImGui::End();
	}
}