#pragma once
#include "IntersectionTrace.h"
#include "Camera.h"
#include "func_ref.h"
#include <optional>
#include <algorithm>
#include <functional>

class Color;
class Ray;
class IntersectionTrace;

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
	using intersecfn = std::optional<IntersectionTrace>(Ray const& r);
	func_ref<intersecfn> find_nearest;
	RenderParams const renderParams_;
	double const normalizer_;
	Camera const cam_;

	Color radiance(const Ray& r, std::mt19937& rnd, unsigned int depth);

	auto generateRenderJob(std::size_t sample);

public:
	Renderer(func_ref<intersecfn> f, const RenderParams& renderParams)
		: find_nearest(f), renderParams_(renderParams), normalizer_(1. / renderParams.sizeX),
		cam_(Camera(renderParams.sizeX, renderParams.sizeY, glm::dvec3( 0.5,0.05,0.5 ), glm::dvec3( 0.5,0.5,0.5 ),norm3::zAxis(), renderParams.FOV * constants::pi / 180)) {}

	std::vector<Color> render();
	std::vector<Color> render2();
	std::size_t percentRendered();
};