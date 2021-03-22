#include "pch.h"
#include "CppUnitTest.h"
#include <glm\glm.hpp>
#include "primitives/Sphere.h"
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
			std::vector<std::unique_ptr<Material>> materials{};
			materials.push_back(std::make_unique<Material>(Material::green_diffuse()));
			std::vector<Geometry> geometries{
				Sphere(c,1.0,materials.at(0).get())
			};
			Scene scene{ std::move(geometries), std::move(materials) };
			auto inter = scene.find_nearest(r);
			Assert::IsTrue(inter.has_value());
		}
	};
}