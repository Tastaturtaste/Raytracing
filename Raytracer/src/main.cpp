#include "Color.h"
#include <chrono>
#include <charconv>
#include "Scene.h"
#include "loadScene.h"
#include "Ray.h"
#include "Renderer.h"
#include "pngwriter.h"
#include "spdlog\spdlog.h"
#include "Timer.h"
#include "argparse\argparse.hpp"
#include <fstream>


int main(int argc, char* argv[]) {

	using namespace Tracer;

	constexpr std::size_t default_width = 720;
	constexpr std::size_t default_height = 480;

	argparse::ArgumentParser program("raytracer");
	program.add_argument("-width").help("Width of the image").action([](std::string_view value) { 
		std::size_t i;
		auto [pos, ec] = std::from_chars(value.data(), value.data() + value.size(), i);
		if (ec == std::errc{}) {
			throw std::runtime_error("value for parameter width not valid!");
		}
		return i; 
		}).default_value(default_width);
	program.add_argument("-height").help("Height of the image").action([](std::string_view value) { 
		std::size_t i;
		auto [pos, ec] = std::from_chars(value.data(), value.data() + value.size(), i);
		if (ec == std::errc{}) {
			throw std::runtime_error("value for parameter height not valid!");
		}
		return i;
		}).default_value(default_height);
		program.add_argument("objfile").help("The obj file describing the scene").default_value(std::string(""));


	try {
		program.parse_args(argc, argv);
	}
	catch (std::runtime_error const& err) {
		spdlog::error("{}\n", err.what());
		std::terminate();
	}

	std::size_t const maxx = program.get<std::size_t>("-width");
	std::size_t const maxy = program.get<std::size_t>("-height");
	std::string const obj_filename = program.get<std::string>("objfile");

	auto png = pngwriter(maxx, maxy, 0.0, "renderimage.png");

	// Cornell parameter
	/*const RenderParams renderParams{
		.numUSamples{4}
		,.numVSamples{4}
		,.preview{true}
		,.max_depth{2}
		,.samplesPerPixel{100}
		,.FOV{90}
		,.sizeX{maxx}
		,.sizeY{maxy}
		,.camPosition{0., 1., 3.}
		,.camMotive{0., 1., 0.}
		,.camUp{norm3::yAxis()}
	};*/

	// debug scene parameter
	const RenderParams renderParams{
		.numUSamples{4}
		,.numVSamples{4}
		,.preview{true}
		,.max_depth{2}
		,.samplesPerPixel{10}
		,.FOV{90}
		,.sizeX{maxx}
		,.sizeY{maxy}
		,.camPosition{0., 1., 3.}
		,.camMotive{0., 1., 0.}
		,.camUp{norm3::yAxis()}
	};

	// begin rendering

	//Renderer renderer(Scene::hull_sphere(), renderParams);
	// Renderer renderer(Scene::debug_scene(), renderParams);
	// Renderer renderer(Scene::light_distribution(), renderParams);

	auto const scene = [&](){ 
		if (obj_filename.size() > 0) {
			auto obj_file = std::ifstream(obj_filename);
			return loadScene(obj_file);
		}
		else
			return Scenes::basic_scene();
		}();
	Renderer renderer([&scene](Ray const& r) { return scene.find_nearest(r); }, renderParams);

	int64_t time{};
	std::vector<Color> pixels;
	pixels.reserve(maxx * maxy);
	{
		auto timer{ Timer<std::chrono::milliseconds>(&time) };
		pixels = std::move(renderer.render());
	}
	spdlog::info("Rendering took {} Seconds.", time * 1. / 1000);

	for (std::size_t y = 0; y < maxy; ++y) {
		for (std::size_t x = 0; x < maxx; ++x) {
			png.plot(static_cast<int>(x), static_cast<int>(y), pixels.at(y * maxx + x).r(), pixels.at(y * maxx + x).g(), pixels.at(y * maxx + x).b());
		}
	}

	png.close();

	return 0;
}