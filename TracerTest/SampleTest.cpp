#include "pch.h"
#include "CppUnitTest.h"
#include <array>
#include <vector>
#include "Norm3.h"
#include "Samples.h"
#include "Constants.h"
#include "Color.h"
#include <glm\glm.hpp>
#include <spdlog\spdlog.h>
#include <cmath>
#include <random>


using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TracerTest
{
	TEST_CLASS(SampleTest)
	{
	public:
		std::random_device rd{};
		std::mt19937 rnd;
		SampleTest() : rnd(std::mt19937(rd())) {}
		TEST_METHOD(TestHemisphereSampleAverage)
		{
			using namespace samples;
			constexpr auto NumSamples = 100000;

			std::uniform_real_distribution uni(0.0, 1.0);
			auto vec = glm::dvec3(0.0, 0.0, 1.0);
			auto norm = norm3(vec);
			const auto basis = norm3::orthogonalFromZ( norm );
			glm::dvec3 average = basis[2].getVec();
			for (std::size_t i = 0; i < NumSamples; ++i) {
				average = average + hemispheresampleCosWeighted(basis, uni(rnd), uni(rnd)).getVec();
			}
			auto normAvg = norm3(average);
			const auto dot = glm::dot(basis[2].getVec(), normAvg.getVec());
			Assert::IsTrue(std::abs(dot) > (1.0-constants::EPS));
		}
		TEST_METHOD(TestHemisphereSampleZoneCount)
		{
			using namespace samples;
			constexpr auto NumSamples = 100000;

			std::uniform_real_distribution uni(0.0,1.0);
			auto vec = glm::dvec3(0.0, 0.0, 1.0);
			auto norm = norm3(vec);
			const auto basis = norm3::orthogonalFromZ(norm);
			auto countXPositiv = 0u;
			auto countXNegativ = 0u;
			auto countYPositiv = 0u;
			auto countYNegativ = 0u;
			double uAvg = 0;
			double vAvg = 0;
			for (std::size_t i = 0; i < NumSamples; ++i) {
				auto u = uni(rnd);
				auto v = uni(rnd);
				uAvg += u;
				vAvg += v;
				auto sample = hemispheresampleCosWeighted(basis, u, v).getVec();
				if (sample.x > 0) {
					++countXPositiv;
				}
				else {
					++countXNegativ;
				}
				if (sample.y > 0) {
					++countYPositiv;
				}
				else {
					++countYNegativ;
				}
			}
			uAvg /= NumSamples;
			vAvg /= NumSamples;
			auto normX = (static_cast<double>(countXPositiv) / countXNegativ - 1.0);
			auto normY = (static_cast<double>(countYPositiv) / countYNegativ - 1.0);
			constexpr auto onePct = 0.01;
			Assert::IsTrue(std::abs(normX) < onePct);
			Assert::IsTrue(std::abs(normY) < onePct);
			Assert::IsTrue(std::abs(2 * uAvg) - 1.0 < onePct);
			Assert::IsTrue(std::abs(2 * vAvg) - 1.0 < onePct);
		}
		TEST_METHOD(TestHemisphereSampleStratifiedAverage) {
			using namespace samples;
			constexpr auto NumSamples = 10000u;
			std::uniform_real_distribution uni(0.0, 1.0);

			auto vec = glm::dvec3(0.0, 0.0, 1.0);
			auto norm = norm3(vec);
			const auto basis = norm3::orthogonalFromZ(norm);
			glm::dvec3 avg{};
			constexpr auto maxUSamples = 4;
			constexpr auto maxVSamples = 4;
			for (std::size_t i = 0; i < NumSamples; ++i) {
				for (auto vSample = 0u; vSample < maxVSamples; ++vSample) {
					for (auto uSample = 0u; uSample < maxUSamples; ++uSample) {
						const double u = (uSample + uni(rnd)) / maxUSamples;
						const double v = (vSample + uni(rnd)) / maxVSamples;
						const norm3 outbound = samples::hemispheresampleCosWeighted(basis, u, v);

						avg += outbound.getVec();
					}
				}
			}
			avg = glm::normalize(avg);
			Assert::IsTrue(avg.z > (1 - constants::EPS));
		}
	};
}