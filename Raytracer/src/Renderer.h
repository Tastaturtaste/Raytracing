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
	std::size_t sizeX{ 720 };
	std::size_t sizeY{ 480 };
};

class Renderer
{
	const Scene scene_;
	const RenderParams renderParams_;
	const double normalizer_;
	std::vector<Color> pixels_;
	Camera cam_;
	mutable Counter counter_;
	bool multithread_;

	Color radiance(const Ray& r, std::mt19937& rnd, unsigned int depth);

	auto generatePixelJob(std::size_t x, std::size_t y);

public:
	Renderer(Scene&& scene, const RenderParams& renderParams, bool multithread)
		: scene_(std::move(scene)), renderParams_(renderParams), normalizer_(1. / renderParams.sizeX), pixels_(std::vector<Color>(renderParams.sizeX* renderParams.sizeY)),
		cam_(Camera(renderParams.sizeX, renderParams.sizeY, { 0.5,0.5,0.05 }, { 0.5,0.5,0.5 }, renderParams.FOV)), counter_(Counter()), multithread_(multithread) {}

	std::vector<Color> render();
	std::size_t percentRendered();
};