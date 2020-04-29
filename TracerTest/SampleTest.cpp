#include "pch.h"
#include "CppUnitTest.h"
#include <array>
#include <vector>
#include "Norm3.h"
#include "Samples.h"
#include "Constants.h"
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
			std::uniform_real_distribution uni;
			auto vec = glm::dvec3(0.0, 0.0, 1.0);
			auto norm = norm3(vec);
			const auto basis = norm3::orthogonalFromZ( norm );
			glm::dvec3 average = basis[2].getVec();
			for (std::size_t i = 0; i < 10000; ++i) {
				average = average + hemispheresampleCosWeighted(basis, uni(rnd), uni(rnd)).getVec();
			}
			auto normAvg = norm3(average);
			spdlog::info("TestHemisphereSample:		x: {}, y: {}, z: {}", normAvg.x(), normAvg.y(), normAvg.z());
			const auto dot = glm::dot(basis[2].getVec(), normAvg.getVec());
			Assert::IsTrue(std::abs(dot) > (1.0-constants::EPS));
		}
	};
}