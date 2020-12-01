#include "pch.h"
#include "CppUnitTest.h"
#include <glm\glm.hpp>
#include "Sphere.h"
#include "Scene.h"
#include <random>


using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TracerTest
{
	TEST_CLASS(SphereTest)
	{
	public:
		std::random_device rd{};
		std::mt19937 rnd;
		SphereTest() : rnd(std::mt19937(rd())) {}

		TEST_METHOD(TestIntersect)
		{
			glm::dvec3 c = { 0.0,0.0,0.0 };
			glm::dvec3 o = { 2,2,2 };
			norm3 d = norm3(c - o);
			Ray r = { o,d };
			Scene<Sphere> scene({ Sphere(c,1.0,Material::green_diffuse()) });
			auto inter = scene.find_nearest(r);
			Assert::IsTrue(inter.has_value());
		}
	};
}