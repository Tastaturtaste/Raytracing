#include "Renderer.h"
#include "Counter.h"
#include "Timer.h"
#include "spdlog\spdlog.h"
#include "ThreadPool.h"
#include "Samples.h"
#include "Color.h"
#include "Ray.h"

#include <numeric>
#include <execution>
#include <future>


constexpr Ray outRay(const glm::dvec3& pos, const norm3& dir) noexcept {
	return Ray(pos + dir.getVec() * constants::EPS, dir);
}

Color Renderer::branchedRadiance(IntersectionTrace trace, std::mt19937& rnd, unsigned int depth) const {
	const std::array<norm3, 3> base = norm3::orthogonalFromZ(trace.normal);
	const auto uni{ std::uniform_real_distribution<>(0.0,1.0) };
	Color result{};
	const auto maxUSamples = renderParams_.numUSamples;
	const auto maxVSamples = renderParams_.numVSamples;
	for (auto vSample = 0u; vSample < maxVSamples; ++vSample) {
		for (auto uSample = 0u; uSample < maxUSamples; ++uSample) {
			const double u = (static_cast<double>(uSample) + uni(rnd)) / maxUSamples;
			const double v = (static_cast<double>(vSample) + uni(rnd)) / maxVSamples;
			const norm3 outbound = Samples::hemispheresampleCosWeighted(base, u, v);
			result += radiance(
				outRay(
					trace.hit_position, outbound),
				rnd, depth + 1);
		}
	}
	return result / static_cast<double>(maxUSamples * maxVSamples);
}

Color Renderer::radiance(Ray r, std::mt19937& rnd, unsigned int depth) const {
	if (depth > renderParams_.max_depth)
		return {};

	if (renderParams_.preview) {
		std::optional<IntersectionTrace> nearest = find_nearest(r);
		if (nearest.has_value())
			return nearest->material->diffuse_color;
		return {};
	}

	const auto uni{ std::uniform_real_distribution<>(0.0,1.0) };
	Color result{};
	Color convolution{1,1,1};

	while (depth <= renderParams_.max_depth) {
		++depth;
		std::optional<IntersectionTrace> nearest = find_nearest(r);
		if (!nearest.has_value()) {
			continue;
		}
		norm3 outbound{ norm3::yAxis() };
		result += convolute(nearest->material->light_color, convolution);
		convolution = convolute(convolution, nearest->material->diffuse_color);
		if (uni(rnd) < nearest->material->prob_diffuse)
		{
			// split only if first hit is diffuse
			if (depth < 1) {
				return branchedRadiance(nearest.value(), rnd, depth);
			}
			const std::array<norm3, 3> base = norm3::orthogonalFromZ(nearest->normal);
			outbound = Samples::hemispheresampleCosWeighted(base, uni(rnd), uni(rnd));
		}
		else
		{
			const std::array<norm3, 3> base = norm3::orthogonalFromZ(norm3(glm::reflect(r.direction().getVec(), nearest->normal.getVec())));
			outbound = Samples::conesampleCosWeighted(base, nearest->material->reflection_cone_radians, uni(rnd), uni(rnd));
		}
		r = Ray(nearest->hit_position, outbound);
	}
	return result;
}

//Color Renderer::radiance(const Ray& r, std::mt19937& rnd, unsigned int depth) {
//	if (depth > renderParams_.max_depth)
//		return {};
//	std::optional<IntersectionTrace> nearest = find_nearest(r);
//	if (!nearest.has_value()) {
//		//spdlog::info("No intersection found!");
//		return {};
//	}
//
//	if (renderParams_.preview) {
//		return nearest->material->diffuse_color;
//	}
//
//	const auto uni{ std::uniform_real_distribution<>(0.0,1.0) };
//
//	if (uni(rnd) < nearest->material->prob_diffuse)
//	{
//		const std::array<norm3, 3> base = norm3::orthogonalFromZ(nearest->normal);
//		Color result{};
//		const auto maxUSamples = depth < 1 ? renderParams_.numUSamples : 1;
//		const auto maxVSamples = depth < 1 ? renderParams_.numVSamples : 1;
//		for (auto vSample = 0u; vSample < maxVSamples; ++vSample) {
//			for (auto uSample = 0u; uSample < maxUSamples; ++uSample) {
//				const double u = (static_cast<double>(uSample) + uni(rnd)) / maxUSamples;
//				const double v = (static_cast<double>(vSample) + uni(rnd)) / maxVSamples;
//				const norm3 outbound = Samples::hemispheresampleCosWeighted(base, u, v);
//
//				result += radiance(
//					outRay(
//						nearest->hit_position, outbound),
//					rnd, depth + 1);
//			}
//		}
//		result = result / static_cast<double>(maxUSamples * maxVSamples);
//		return nearest->material->light_color +
//			convolute(result, nearest->material->diffuse_color);
//	}
//	else {
//		const std::array<norm3, 3> base = norm3::orthogonalFromZ(norm3(glm::reflect(r.direction().getVec(), nearest->normal.getVec())));
//		const norm3 outbound = Samples::conesampleCosWeighted(base, nearest->material->reflection_cone_radians, uni(rnd), uni(rnd));
//
//		return nearest->material->light_color +
//			convolute(
//				radiance(
//					outRay(nearest->hit_position, outbound), rnd, ++depth),
//				nearest->material->diffuse_color);
//	}
//}

auto Renderer::generateRenderJob(std::size_t sample) {

	std::random_device r_device;
	const unsigned int seed = static_cast<unsigned int>(r_device()) + static_cast<unsigned int>(sample);

	return 	[this, seed]() {
		auto rnd = std::mt19937(seed);
		std::vector<Color> pixels_;
		pixels_.reserve(renderParams_.sizeX * renderParams_.sizeY);
		Ray r{};
		for (std::size_t y = 0; y < renderParams_.sizeY; ++y) {
			for (std::size_t x = 0; x < renderParams_.sizeX; ++x) {
				r = cam_.getRay({ x, y }, rnd);
				//pixels_[y * renderParams_.sizeX + x] = radiance(r, rnd, 0);
				pixels_.emplace_back(radiance(r, rnd, 0) / renderParams_.samplesPerPixel);
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
			results.emplace_back(threadPool.addJob(generateRenderJob(jobsToAdd)));
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