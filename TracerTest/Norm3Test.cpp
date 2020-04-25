#include "pch.h"
#include "CppUnitTest.h"
#include <random>
#include "Vec3.h"
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
			Norm3 basis = Vec3( 1.0,0.0,0.0 ).norm();
			std::uniform_real_distribution uni;
			for (int i = 0; i < 1000; ++i) {
				auto x = uni(rnd);
				auto y = uni(rnd);
				auto z = uni(rnd);
				Norm3 vec = Vec3( x,y,z ).norm();
				Assert::AreEqual(basis.dot(vec), vec.x());
			}
			basis = Vec3(0.0, 1.0, 0.0).norm();
			for (int i = 0; i < 1000; ++i) {
				auto x = uni(rnd);
				auto y = uni(rnd);
				auto z = uni(rnd);
				Norm3 vec = Vec3(x, y, z).norm();
				Assert::AreEqual(basis.dot(vec), vec.y());
			}
			basis = Vec3(0.0, 0.0, 1.0).norm();
			for (int i = 0; i < 1000; ++i) {
				auto x = uni(rnd);
				auto y = uni(rnd);
				auto z = uni(rnd);
				Norm3 vec = Vec3( x,y,z ).norm();
				Assert::AreEqual(basis.dot(vec), vec.z());
			}
		}
		TEST_METHOD(CrossTest) {
			std::uniform_real_distribution uni;
			for (int i = 0; i < 1000; ++i) {
				auto x = uni(rnd);
				auto y = uni(rnd);
				auto z = uni(rnd);
				Norm3 vec1 = Vec3( x,y,z ).norm();
				x = uni(rnd);
				y = uni(rnd);
				z = uni(rnd);
				Norm3 vec2 = Vec3( x,y,z ).norm();
				auto cross = vec1.cross(vec2);
				Assert::IsTrue(fabs(cross.dot(vec1)) < constants::EPS);
				Assert::IsTrue(fabs(cross.dot(vec2)) < constants::EPS);
			}
		}
	};
}
