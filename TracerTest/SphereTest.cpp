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
			std::vector<Sphere> spheres({ Sphere(c,1.0,Material::green_diffuse()) });
			glm::dvec3 o = { 2,2,2 };
			norm3 d = norm3(c - o);
			Ray r = { o,d };
			Scene scene(spheres);
			auto inter = scene.spheres().at(0).intersect(r);
			Assert::IsTrue(inter.has_value());
		}
	};
}