#include "pch.h"
#include "CppUnitTest.h"
#include <random>
#include <glm/glm.hpp>
#include "Camera.h"
#include "Norm3.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TracerTest
{

	TEST_CLASS(CameraTest) {
	public:
		std::random_device rd{};
		std::mt19937 rnd;
		std::size_t maxx{ 720 };
		std::size_t maxy{ 480 };
		glm::dvec3 camPos{ 0.5,0.05,0.5 };
		glm::dvec3 motive{ 0.5,0.5,0.5 };
		norm3 up{ norm3::zAxis() };
		double FOV{ 90.0 * constants::pi / 180.0 };

		Camera cam;

		CameraTest() : rnd(std::mt19937(rd())), cam(maxx, maxy, camPos, motive, up, FOV) {}

		TEST_METHOD(CameraAvgDirection) {
			glm::dvec3 avg_{};
			norm3 dir = norm3(motive - camPos);
			for (std::size_t y = 0; y < maxy; ++y) {
				for (std::size_t x = 0; x < maxx; ++x) {
					avg_ += cam.getRay(x, y, rnd).direction().getVec();
				}
			}
			norm3 avg = norm3(avg_);
			Assert::IsTrue(glm::dot(avg.getVec(), dir.getVec()) > (1.0 - constants::EPS));
		}
	};
};