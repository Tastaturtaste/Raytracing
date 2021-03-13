#include "Renderer.h"
#include "Counter.h"
#include "Timer.h"
#include "spdlog\spdlog.h"
#include "ThreadPool.h"
#include "Samples.h"
#include "Color.h"
#include "Ray.h"
#include "BxDFs.h"
#include "gsl-lite.hpp"

#include <numeric>
#include <execution>
#include <future>


// Moves the outgoing ray a little bit in the ray direction to avoid instantly hitting the origin surface again
constexpr Ray outRay(const glm::dvec3& pos, const norm3& dir) noexcept {
	return Ray(pos + dir.getVec() * constants::EPS, dir);
}

//enum class reflectionType{diffuse=0, specular};
//
//inline Ray getOutray(IntersectionTrace const& trace, Ray const& incoming, reflectionType rt, const double u, const double v) {
//	norm3 outbound{ norm3::yAxis() };
//	switch (rt){
//	case reflectionType::diffuse: {
//		const std::array<norm3, 3> base = norm3::orthogonalFromY(trace.normal);
//		outbound = Samples::hemispheresampleCosWeighted(base, u, v);
//	}
//		break;
//
//	case reflectionType::specular: {
//		const std::array<norm3, 3> base = norm3::orthogonalFromY(norm3(glm::reflect(incoming.direction().getVec(), trace.normal.getVec())));
//		outbound = Samples::conesampleCosWeighted(base, 0.0 * trace.material->reflection_cone_radians, u, v);
//	}
//		break;
//	default:
//		throw std::logic_error("Unhandled reflectionType!");
//	}
//	return outRay(trace.hit_position, outbound);
//}

Color Renderer::branchingRadiance(Ray r, std::mt19937& rnd, unsigned int depth) const {
	Color result{};
	std::optional<IntersectionTrace> trace = find_nearest(r);
	if (!trace.has_value()) {
		return result;
	}
	// In preview mode no bounces are made
	else if (renderParams_.preview) {
		return trace.value().material->diffuse_color;
	}

	const auto uni{ std::uniform_real_distribution<>(0.0,1.0) };
	const auto maxUSamples = renderParams_.numUSamples;
	const auto maxVSamples = renderParams_.numVSamples;
	for (auto vSample = 0; vSample < maxVSamples; ++vSample) {
		for (auto uSample = 0; uSample < maxUSamples; ++uSample) {
			const double u = (static_cast<double>(uSample) + uni(rnd)) / maxUSamples;
			const double v = (static_cast<double>(vSample) + uni(rnd)) / maxVSamples;
			bxdf::bxdfResult bxdfRes = bxdf::bxdf(r, trace.value(), u, v, uni(rnd));
			result += radiance(trace.value(), std::move(bxdfRes), rnd, depth);
		}
	}
	return result / maxUSamples / maxVSamples;
}

Color Renderer::radiance(IntersectionTrace const& trace, bxdf::bxdfResult bxdfRes, std::mt19937& rnd, unsigned int depth) const {
	// depth == number of bounces after initial hit

	const auto uni{ std::uniform_real_distribution<>(0.0,1.0) };
	Color result{ trace.material->light_color };
	Color convolution{ bxdfRes.attenuation };

	Ray r{trace.hit_position, bxdfRes.out};

	std::optional<IntersectionTrace> nearest{ trace };
	for (; depth < renderParams_.maxDepth; ++depth) {
		nearest = find_nearest(r);
		// If no material is hit no further bounces have to be made
		// No hit counts as black
		if (!nearest.has_value()) {
			break;
		}
		result += convolution * nearest->material->light_color;
		bxdfRes = bxdf::bxdf(r, nearest.value(), uni(rnd), uni(rnd), uni(rnd));
		convolution *= bxdfRes.attenuation;
		r = { nearest->hit_position, bxdfRes.out };
	}
	return result;
}

auto Renderer::generateRenderJob(std::size_t sample) {

	std::random_device r_device;
	const unsigned int seed = gsl::narrow_cast<unsigned int>(r_device()) + gsl::narrow_cast<unsigned int>(sample);

	return 	[this, seed]() {
		auto rnd = std::mt19937(seed);
		std::vector<Color> pixels_;
		std::uniform_real_distribution<double> const uni{ 0.0,1.0 };
		pixels_.reserve(renderParams_.sizeX * renderParams_.sizeY);
		Ray r{};
		for (std::size_t y = 0; y < renderParams_.sizeY; ++y) {
			for (std::size_t x = 0; x < renderParams_.sizeX; ++x) {
				r = cam_.getRay({ x, y }, uni(rnd), uni(rnd));
				//pixels_[y * renderParams_.sizeX + x] = radiance(r, rnd, 0);
				pixels_.emplace_back(branchingRadiance(r, rnd, 0u) / renderParams_.samplesPerPixel);
			}
		}
		return std::move(pixels_);
	};
}

void inline gammaCorrection(std::vector<Color>& image, double correctionValue = 1.0 / 2.2) noexcept {
	std::for_each(std::execution::par_unseq, image.begin(), image.end(), [correctionValue](Color& c) {
		c.vec_ = glm::pow(c.vec_, decltype(c.vec_)(correctionValue));
		});
}

std::vector<Color> Renderer::render2() {

	std::vector<std::size_t> samples(renderParams_.samplesPerPixel);
	std::iota(samples.begin(), samples.end(), 1u);

	const auto binaryReduce = [this](std::vector<Color>&& lhs, std::vector<Color>&& rhs) {
		std::transform(lhs.begin(), lhs.end(), rhs.begin(), lhs.begin(), [](const auto& a, const auto& b) {return a + b; });
		return lhs;
	};
	const auto unaryOp = [this](std::size_t sample) {return generateRenderJob(sample)(); };

	auto rawPixels = std::transform_reduce(std::execution::par_unseq, samples.begin(), samples.end(), std::vector<Color>(renderParams_.sizeX * renderParams_.sizeY), binaryReduce, unaryOp);
	//gammaCorrection(rawPixels);
	return std::move(rawPixels);
}

std::vector<Color> Renderer::render() {
	const auto numCores = std::thread::hardware_concurrency();
	auto threadPool = ThreadPool(numCores);

	std::vector<std::future<std::vector<Color>>> results;
	results.reserve(numCores);

	auto samplesDone = 0u;
	auto samplesLeft = renderParams_.samplesPerPixel;
	auto pct = (100 * samplesDone) / renderParams_.samplesPerPixel;
	const auto checkFutureStatus = [](auto& f) {return f.wait_for(std::chrono::milliseconds(1)) == std::future_status::ready; };

	std::vector<Color> pixels_(renderParams_.sizeX * renderParams_.sizeY);
	const auto addSample = [&](const std::vector<Color>&& sample) noexcept {
		std::transform(pixels_.begin(), pixels_.end(), sample.begin(), pixels_.begin(), std::plus<Color>());
	};
	const auto applyFutures = [&]() {
		auto it = std::find_if(results.begin(), results.end(), checkFutureStatus);
		while (it != results.end())
		{
			addSample(it->get());
			++samplesDone;
			results.erase(it);
			it = std::find_if(results.begin(), results.end(), checkFutureStatus);
		}
	};
	do {
		auto spaceInQueue = std::max<std::size_t>(threadPool.numThreads() - threadPool.numJobs(), 0u);
		auto jobsToAdd = std::min<std::size_t>(samplesLeft, spaceInQueue);
		for (; jobsToAdd > 0; --jobsToAdd) {
			results.emplace_back(threadPool.addJob(generateRenderJob(samplesLeft)));
			--samplesLeft;
		}
		applyFutures();

		pct = (100 * samplesDone) / renderParams_.samplesPerPixel;
		spdlog::info("{} Percent of samples done", pct);

		std::this_thread::sleep_for(std::chrono::milliseconds(100));

	} while (samplesDone < renderParams_.samplesPerPixel);

	//gammaCorrection(pixels_);

	return std::move(pixels_);
}