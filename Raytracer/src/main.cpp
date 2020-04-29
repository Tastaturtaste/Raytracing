#include "Color.h"
#include <chrono>
#include "Scene.h"
#include "Renderer.h"
#include "pngwriter.h"
#include "spdlog\spdlog.h"


int main(int argc, char* argv[]) {
	constexpr std::size_t maxx = 720;
	constexpr std::size_t maxy = 480;
	auto png = pngwriter(maxx, maxy, 0.0, "renderimage.png");

	const RenderParams renderParams{
		.numUSamples{4}
		,.numVSamples{4}
		,.preview{false}
		,.max_depth{3}
		,.samplesPerPixel{100}
		,.FOV{90}
		,.sizeX{maxx}
		,.sizeY{maxy}
	};

	// begin rendering

	Renderer renderer(Scene::debug_scene(), renderParams, true);
	std::vector<Color> pixels = renderer.render();

	for (std::size_t y = 0; y < maxy; ++y) {
		for (std::size_t x = 0; x < maxx; ++x) {
			png.plot(static_cast<int>(x), static_cast<int>(y), pixels.at(y * maxx + x).r(), pixels.at(y * maxx + x).g(), pixels.at(y * maxx + x).b());
		}
	}

	png.close();

	return 0;
}