#include <chrono>
#include <charconv>
#include <fstream>

#include "pngwriter.h"
#include "spdlog\spdlog.h"
#include "argparse\argparse.hpp"
#include "gsl-lite.hpp"

#include "Scene.h"
#include "Color.h"
#include "loadScene.h"
#include "Ray.h"
#include "Renderer.h"
#include "util/Timer.h"
#include "util/utilities.h"
#include "OcTree.h"


std::string constructFilename(std::string obj_file, RenderParams const& rp);

int main(int argc, char const* argv[]) {

	using namespace Tracer;

	argparse::ArgumentParser program("raytracer");
	program.add_argument("-o","--objfile").help("The obj file describing the scene. If no file is provided a default example scene will be rendered.").action([](std::string_view sv) -> std::string { return std::string(sv); }).default_value(std::string("Example.obj"));
	program.add_argument("-spp", "--sampled-per-pixel").help("samples per pixel").default_value(10).action([](std::string_view sv) {
		int const n = asNumber<int>(sv);
		if (n < 0)
			throw std::runtime_error("argument for number of samples per pixel not valid!");
		return n;
		});
	program.add_argument("-pv", "--preview").help("If a preview without global illumination should be generated.").implicit_value(true).default_value(false);
	program.add_argument("-split-u", "--num-first-bounce-splits-u").help("Number of rays generated for one coordinate on first hit.").default_value(4).action([](std::string_view sv) {
		int const n = asNumber<int>(sv);
		if (n < 1)
			throw std::runtime_error("Argument for number of first bounce splits not valid!");
		return n;
		});
	program.add_argument("-split-v", "--num-first-bounce-splits-v").help("Number of rays generated for one coordinate on first hit.").default_value(4).action([](std::string_view sv) {
		int const n = asNumber<int>(sv);
		if (n < 1)
			throw std::runtime_error("Argument for number of first bounce splits not valid!");
		return n;
		});
	program.add_argument("-d", "--max-depth").help("Number of bounces.").default_value(4).action([](std::string_view sv) {
		int const n = asNumber<int>(sv);
		if (n < 0)
			throw std::runtime_error("Argument for number of bounces not valid!");
		return n;
		});
	program.add_argument("-x", "--size-x").help("Width of the picture.").default_value(720).action([](std::string_view sv) {
		int const n = asNumber<int>(sv);
		if (n < 0)
			throw std::runtime_error("Argument for width of picture not valid!");
		return n;
		});
	program.add_argument("-y", "--size-y").help("Height of the picture.").default_value(720).action([](std::string_view sv) {
		int const n = asNumber<int>(sv);
		if (n < 0)
			throw std::runtime_error("Argument for height of picture not valid!");
		return n;
		});
	program.add_argument("-cp", "--cam-position").help("Position of the camera in the scene.").nargs(3).default_value(std::vector<double>{0., 1., 3.}).action([](std::string_view sv) {
		double const n = asNumber<double>(sv);
		return n;
		});
	program.add_argument("-cm", "--cam-motive").help("Point in the scene the camara looks at.").nargs(3).default_value(std::vector<double>{0., 1., 0.}).action([](std::string_view sv) {
		double const n = asNumber<double>(sv);
		return n;
		});

	try {
		program.parse_args(argc, argv);
	}
	catch (std::runtime_error const& err) {
		spdlog::error("{}\n", err.what());
	}
	std::size_t const maxx = program.get<int>("-x");
	std::size_t const maxy = program.get<int>("-y");

	auto camPosition = program.get<std::vector<double>>("-cp");
	if (camPosition.size() != 3) {
		throw std::runtime_error("Invalid number of coordinates for cam position!");
	}
	auto camMotive = program.get<std::vector<double>>("-cm");
	if (camMotive.size() != 3) {
		throw std::runtime_error("Invalid number of coordinates for cam motive");
	}

	const RenderParams renderParams{
		.numUSamples{program.get<int>("-split-u")}
		,.numVSamples{program.get<int>("-split-v")}
		,.preview{program.get<bool>("-pv")}
		,.maxDepth{program.get<int>("-d")}
		,.samplesPerPixel{program.get<int>("-spp")}
		,.FOV{45}
		,.sizeX{program.get<int>("-x")}
		,.sizeY{program.get<int>("-y")}
		,.camPosition{toGlm<double>(camPosition)}
		,.camMotive{toGlm<double>(camMotive)}
		,.camUp{norm3::yAxis()}
	};

	auto obj_filename = program.get<std::string>("--objfile");
	std::string outname = constructFilename(obj_filename, renderParams);
	auto png = pngwriter(renderParams.sizeX, renderParams.sizeY, 0.0, outname.c_str());

	// begin rendering

	auto const scene = [&](){ 
		if (obj_filename != "Example.obj") {
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
			png.plot(gsl::narrow_cast<int>(x+1), gsl::narrow_cast<int>(y+1), pixels.at(y * maxx + x).r(), pixels.at(y * maxx + x).g(), pixels.at(y * maxx + x).b());
		}
	}

	png.close();

	return 0;
}

std::string constructFilename(std::string obj_file, RenderParams const& rp) {
	return obj_file.substr(0, obj_file.find_last_of("."))
		.append("_").append(std::to_string(rp.samplesPerPixel)).append("spp")
		.append("_").append(std::to_string(rp.maxDepth)).append("d")
		.append(".png");
}