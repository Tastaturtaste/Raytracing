#include "pch.h"
#include "CppUnitTest.h"
#include <array>
#include <vector>
#include "Norm3.h"
#include "Samples.h"
#include "util/Constants.h"
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
		const size_t NumSamples = 100000;

		TEST_METHOD(TestHemisphereSampleAverageX)
		{
			using namespace Samples;
			std::uniform_real_distribution<double> uni(0.0, 1.0);
			auto vec = glm::dvec3(1.0, 0.0, 0.0);
			auto norm = norm3(vec);
			const auto basis = norm3::orthogonalFromY(norm);
			glm::dvec3 average = glm::dvec3(0);
			for (std::size_t i = 0; i < NumSamples; ++i) {
				average = average + hemispheresampleCosWeighted(basis, uni(rnd), uni(rnd)).getVec();
			}
			auto normAvg = norm3(average);
			const auto dot = glm::dot(norm.getVec(), normAvg.getVec());
			Assert::IsTrue(std::abs(dot) > (1.0 - constants::EPS));
		}
		TEST_METHOD(TestHemisphereSampleAverageY)
		{
			using namespace Samples;
			std::uniform_real_distribution<double> uni(0.0, 1.0);
			auto vec = glm::dvec3(0.0, 1.0, 0.0);
			auto norm = norm3(vec);
			const auto basis = norm3::orthogonalFromY(norm);
			glm::dvec3 average = glm::dvec3(0);
			for (std::size_t i = 0; i < NumSamples; ++i) {
				average = average + hemispheresampleCosWeighted(basis, uni(rnd), uni(rnd)).getVec();
			}
			auto normAvg = norm3(average);
			const auto dot = glm::dot(norm.getVec(), normAvg.getVec());
			Assert::IsTrue(std::abs(dot) > (1.0 - constants::EPS));
		}
		TEST_METHOD(TestHemisphereSampleAverageZ)
		{
			using namespace Samples;
			std::uniform_real_distribution<double> uni(0.0, 1.0);
			auto vec = glm::dvec3(0.0, 0.0, 1.0);
			auto norm = norm3(vec);
			const auto basis = norm3::orthogonalFromY(norm);
			glm::dvec3 average = glm::dvec3(0);
			for (std::size_t i = 0; i < NumSamples; ++i) {
				average = average + hemispheresampleCosWeighted(basis, uni(rnd), uni(rnd)).getVec();
			}
			auto normAvg = norm3(average);
			const auto dot = glm::dot(norm.getVec(), normAvg.getVec());
			Assert::IsTrue(std::abs(dot) > (1.0 - constants::EPS));


		}
		TEST_METHOD(TestHemisphereSampleZoneCount)
		{
			using namespace Samples;
			std::uniform_real_distribution<double> uni(0.0, 1.0);
			auto vec = glm::dvec3(0.0, 0.0, 1.0);
			auto norm = norm3(vec);
			const auto basis = norm3::orthogonalFromY(norm);
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
			using namespace Samples;
			constexpr auto NumSamples = 10000u;
			std::uniform_real_distribution uni(0.0, 1.0);

			auto vec = glm::dvec3(0.0, 0.0, 1.0);
			auto norm = norm3(vec);
			const auto basis = norm3::orthogonalFromY(norm);
			glm::dvec3 avg{};
			constexpr auto maxUSamples = 4;
			constexpr auto maxVSamples = 4;
			for (std::size_t i = 0; i < NumSamples; ++i) {
				for (auto vSample = 0u; vSample < maxVSamples; ++vSample) {
					for (auto uSample = 0u; uSample < maxUSamples; ++uSample) {
						const double u = (uSample + uni(rnd)) / maxUSamples;
						const double v = (vSample + uni(rnd)) / maxVSamples;
						const norm3 outbound = Samples::hemispheresampleCosWeighted(basis, u, v);

						avg += outbound.getVec();
					}
				}
			}
			avg = glm::normalize(avg);
			Assert::IsTrue(avg.z > (1 - constants::EPS));
		}
		TEST_METHOD(TestConeSamplesInCone) {
			using namespace Samples;
			std::uniform_real_distribution uni(0.0, 1.0);

			for (std::size_t i = 0; i < NumSamples; ++i) {
				auto norm = norm3(glm::dvec3(1 - 2*uni(rnd), 1 - 2*uni(rnd), 1 - 2*uni(rnd)));
				const auto basis = norm3::orthogonalFromY(norm);
				const double coneAngle = uni(rnd) * constants::pi;
				const auto u = uni(rnd);
				const auto v = uni(rnd);
				const norm3 outbound = Samples::conesampleCosWeighted(basis, coneAngle, u, v);
				auto const dot = glm::dot(outbound.getVec(), norm.getVec());
				auto const cos = glm::cos(0.5 * coneAngle);
				if (dot >= cos)
				{
					Assert::IsTrue(dot >= cos);

				}
			}
		}
	};
}