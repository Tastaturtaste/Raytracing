#include "pch.h"
#include "CppUnitTest.h"
#include <glm\glm.hpp>
#include <spdlog\spdlog.h>
#include "loadScene.h"
#include <string>
#include <string_view>


using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TracerTest
{
	TEST_CLASS(ObjLoaderTest)
	{
	public:
		TEST_METHOD(parseVertexTest) {
			using namespace Tracer;
			std::string line = "v -1.01 0.00 0.99";
			std::string_view lineView{ line };
			auto l = splitString(lineView, " \t"); // obj encoding of vertex
			//
			//auto vec = Tracer::parse_vertex<double>({ l.begin() + 1, l.end() });
			//spdlog::debug("vec is {}, {}, {}.\n", vec.x, vec.y, vec.z);
			//Assert::IsTrue(vec == glm::dvec3(-1.01, 0.00, 0.99));
		}
	};
}