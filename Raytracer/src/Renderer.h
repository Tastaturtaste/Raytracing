#pragma once
#include "IntersectionTrace.h"
#include "Camera.h"
#include "func_ref.h"
#include <optional>
#include <algorithm>
#include <functional>

struct RenderParams {
	int numUSamples{ 4 };
	int numVSamples{ 4 };
	bool preview{ false };
	int maxDepth{ 2 };
	int samplesPerPixel{ 10 };
	double FOV{ 90 };
	int sizeX{ 720 };
	int sizeY{ 480 };
	glm::dvec3 camPosition{ 0.5, 0.05, 0.5 };
	glm::dvec3 camMotive{ 0.5, 0.5, 0.5 };
	norm3 camUp{ norm3::zAxis() };
};

namespace bxdf {
	struct bxdfResult;
}

class Renderer
{
	using intersecfn = std::optional<IntersectionTrace>(Ray const& r);
	func_ref<intersecfn> find_nearest;
	RenderParams const renderParams_;
	double const normalizer_;
	Camera const cam_;

	Color branchingRadiance(Ray r, std::mt19937& rnd, unsigned int depth) const;
	/*Color branchedRadiance(IntersectionTrace const& trace, std::mt19937& rnd, unsigned int depth) const;*/
	/*Color radiance(Ray r, std::mt19937& rnd, unsigned int depth) const;*/
	Color radiance(IntersectionTrace const& trace, bxdf::bxdfResult, std::mt19937& rnd, unsigned int depth) const;

	auto generateRenderJob(std::size_t sample);

public:
	Renderer(func_ref<intersecfn> f, const RenderParams& renderParams)
		: find_nearest(f), renderParams_(renderParams), normalizer_(1. / renderParams.sizeX),
		cam_(Camera(renderParams.sizeX, renderParams.sizeY, renderParams.camPosition, renderParams.camMotive,renderParams.camUp, renderParams.FOV * constants::pi / 180)) {}

	std::vector<Color> render();
	std::vector<Color> render2();
	std::size_t percentRendered();
};