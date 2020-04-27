#include "Color.h"
#include <chrono>
#include "Scene.h"
#include "Renderer.h"
#include "pngwriter.h"
#include "spdlog\spdlog.h"


int main(int argc, char* argv[]) {
	constexpr std::size_t maxx = 720;
	constexpr std::size_t maxy = 480;
	auto png = pngwriter(maxx, maxy, 0.0, "basic_scene.png");

	const RenderParams renderParams{
		.numUSamples{4}
		,.numVSamples{4}
		,.preview{false}
		,.max_depth{3}
		,.samplesPerPixel{100}
		,.FOV{90}
	};

	// begin rendering

	Renderer renderer(Scene::basic_scene(), renderParams, maxx, maxy, true);
	std::vector<Color> pixels = renderer.render();

	for (std::size_t y = 0; y < maxy; ++y) {
		for (std::size_t x = 0; x < maxx; ++x) {
			png.plot(static_cast<int>(x), static_cast<int>(y), pixels[y * maxx + x].r, pixels[y * maxx + x].g, pixels[y * maxx + x].b);
		}
	}

	png.close();

	return 0;
}