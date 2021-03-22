#include "pch.h"
#include "CppUnitTest.h"
#include "util/Constants.h"
#include <random>
#include <glm\glm.hpp>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TracerTest
{
	TEST_CLASS(VecTest)
	{
	public:
		std::random_device rd{};
		std::mt19937 rnd;
		VecTest() : rnd(std::mt19937(rd())) {}
		TEST_METHOD(DotTest) {
			glm::dvec3 basis{ 1.0,0.0,0.0 };
			std::uniform_real_distribution uni;
			for (int i = 0; i < 1000; ++i) {
				auto x = uni(rnd);
				auto y = uni(rnd);
				auto z = uni(rnd);
				glm::dvec3 vec{ x,y,z };
				Assert::AreEqual(glm::dot(basis,vec), x);
			}
			basis = { 0.0,1.0,0.0 };
			for (int i = 0; i < 1000; ++i) {
				auto x = uni(rnd);
				auto y = uni(rnd);
				auto z = uni(rnd);
				glm::dvec3 vec{ x,y,z };
				Assert::AreEqual(glm::dot(basis, vec), y);
			}
			basis = { 0.0,0.0,1.0 };
			for (int i = 0; i < 1000; ++i) {
				auto x = uni(rnd);
				auto y = uni(rnd);
				auto z = uni(rnd);
				glm::dvec3 vec{ x,y,z };
				Assert::AreEqual(glm::dot(basis, vec), z);
			}
		}
		TEST_METHOD(CrossTest) {
			std::uniform_real_distribution uni;
			for (int i = 0; i < 1000; ++i) {
				auto x = uni(rnd);
				auto y = uni(rnd);
				auto z = uni(rnd);
				glm::dvec3 vec1{ x,y,z };
				x = uni(rnd);
				y = uni(rnd);
				z = uni(rnd);
				glm::dvec3 vec2{ x,y,z };
				auto cross = glm::cross(vec1,vec2);
				Assert::IsTrue(std::abs(glm::dot(cross,vec1)) < constants::EPS);
				Assert::IsTrue(std::abs(glm::dot(cross,vec2)) < constants::EPS);
			}
		}
	};
}
