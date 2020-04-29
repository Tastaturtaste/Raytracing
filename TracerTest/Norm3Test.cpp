#include "pch.h"
#include "CppUnitTest.h"
#include <random>
#include <glm/glm.hpp>
#include "Norm3.h"


using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TracerTest
{

	TEST_CLASS(NormTest)
	{
	public:
		std::random_device rd{};
		std::mt19937 rnd;
		NormTest() : rnd(std::mt19937(rd())) {}

		TEST_METHOD(DotTest) {
			norm3 basis = norm3(glm::dvec3( 1.0,0.0,0.0 ));
			std::uniform_real_distribution uni;
			for (int i = 0; i < 1000; ++i) {
				auto x = uni(rnd);
				auto y = uni(rnd);
				auto z = uni(rnd);
				norm3 vec = norm3(glm::dvec3( x,y,z ));
				Assert::AreEqual(glm::dot(basis.getVec(),vec.getVec()), vec.getVec().x);
			}
			basis = norm3(glm::dvec3(0.0, 1.0, 0.0));
			for (int i = 0; i < 1000; ++i) {
				auto x = uni(rnd);
				auto y = uni(rnd);
				auto z = uni(rnd);
				norm3 vec = norm3(glm::dvec3(x, y, z));
				Assert::AreEqual(glm::dot(basis.getVec(),vec.getVec()), vec.getVec().y);
			}
			basis = norm3(glm::dvec3(0.0, 0.0, 1.0));
			for (int i = 0; i < 1000; ++i) {
				auto x = uni(rnd);
				auto y = uni(rnd);
				auto z = uni(rnd);
				norm3 vec = norm3(glm::dvec3( x,y,z ));
				Assert::AreEqual(glm::dot(basis.getVec(),vec.getVec()), vec.getVec().z);
			}
		}
		TEST_METHOD(CrossTest) {
			std::uniform_real_distribution uni;
			for (int i = 0; i < 1000; ++i) {
				auto x = uni(rnd);
				auto y = uni(rnd);
				auto z = uni(rnd);
				norm3 vec1 = norm3(glm::dvec3( x,y,z ));
				x = uni(rnd);
				y = uni(rnd);
				z = uni(rnd);
				norm3 vec2 = norm3(glm::dvec3( x,y,z ));
				auto cross = glm::cross(vec1.getVec(),vec2.getVec());
				Assert::IsTrue(std::abs(glm::dot(cross,vec1.getVec())) < constants::EPS);
				Assert::IsTrue(std::abs(glm::dot(cross,vec2.getVec())) < constants::EPS);
			}
		}
	};
}
