#include "pch.h"
#include "CppUnitTest.h"
#include "loadScene.h"
#include <glm\glm.hpp>
#include <spdlog\spdlog.h>
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TracerTest
{
	TEST_CLASS(ObjLoaderTest)
	{
	public:
		TEST_METHOD(parseVertexTest) {
			auto l = "v  -1.01  0.00   0.99\n"; // obj encoding of vertex
			auto vec = Tracer::parse_vertex(l);
			spdlog::debug("vec is {}, {}, {}.\n", vec[0], vec[1], vec[2]);
			Assert::IsTrue(vec == glm::dvec3(-1.01, 0.00, 0.99));
		}
	};
}