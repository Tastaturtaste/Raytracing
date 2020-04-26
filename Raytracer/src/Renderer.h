#pragma once
#include "Scene.h"
#include "Color.h"
#include "Camera.h"
#include "Counter.h"
#include <random>

struct RenderParams {
	unsigned int numUSamples{ 4 };
	unsigned int numVSamples{ 4 };
	bool preview{ false };
	unsigned int max_depth{ 2 };
	unsigned int samplesPerPixel{ 10 };
	double FOV{ 90 };
};

class Renderer
{
	const Scene scene_;
	const RenderParams renderParams_;
	const std::size_t maxx_;
	const std::size_t maxy_;
	std::vector<Color> pixels_;
	Camera cam_;
	mutable Counter counter_;
	bool multithread_;

	Color radiance(const Ray& r, std::mt19937& rnd, unsigned int depth);

	auto generatePixelJob(std::size_t x, std::size_t y);

public:
	Renderer(Scene&& scene, const RenderParams& renderParams, std::size_t maxx, std::size_t maxy, bool multithread)
		: scene_(std::move(scene)), renderParams_(renderParams), maxx_(maxx), maxy_(maxy), pixels_(std::vector<Color>(maxx* maxy)), cam_(Camera(maxx, maxy, renderParams.FOV)), counter_(Counter()), multithread_(multithread) {}

	std::vector<Color> render();
	std::size_t percentRendered();
};