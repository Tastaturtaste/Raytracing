#include "pch.h"
#include "CppUnitTest.h"
#include "Vec3.h"
#include "Constants.h"
#include <random>

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
			Vec3 basis{ 1.0,0.0,0.0 };
			std::uniform_real_distribution uni;
			for (int i = 0; i < 1000; ++i) {
				auto x = uni(rnd);
				auto y = uni(rnd);
				auto z = uni(rnd);
				Vec3 vec{ x,y,z };
				Assert::AreEqual(basis.dot(vec), x);
			}
			basis = { 0.0,1.0,0.0 };
			for (int i = 0; i < 1000; ++i) {
				auto x = uni(rnd);
				auto y = uni(rnd);
				auto z = uni(rnd);
				Vec3 vec{ x,y,z };
				Assert::AreEqual(basis.dot(vec), y);
			}
			basis = { 0.0,0.0,1.0 };
			for (int i = 0; i < 1000; ++i) {
				auto x = uni(rnd);
				auto y = uni(rnd);
				auto z = uni(rnd);
				Vec3 vec{ x,y,z };
				Assert::AreEqual(basis.dot(vec), z);
			}
		}
		TEST_METHOD(CrossTest) {
			std::uniform_real_distribution uni;
			for (int i = 0; i < 1000; ++i) {
				auto x = uni(rnd);
				auto y = uni(rnd);
				auto z = uni(rnd);
				Vec3 vec1{ x,y,z };
				x = uni(rnd);
				y = uni(rnd);
				z = uni(rnd);
				Vec3 vec2{ x,y,z };
				auto cross = vec1.cross(vec2);
				Assert::IsTrue(fabs(cross.dot(vec1)) < constants::EPS);
				Assert::IsTrue(fabs(cross.dot(vec2)) < constants::EPS);
			}
		}
	};
}
