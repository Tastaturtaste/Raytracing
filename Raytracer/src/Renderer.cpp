#pragma once
#include "Renderer.h"
#include "Samples.h"
#include <vector>
#include <future>
#include <random>
#include <chrono>
#include "Timer.h"
#include "spdlog\spdlog.h"
#include "ThreadPool.h"


Color Renderer::radiance(const Ray& r, std::mt19937& rnd, unsigned int depth) {
	if (depth > renderParams_.max_depth)
		return {};
	IntersectionTrace nearest{};
	if (auto maybe_nearest = scene_.find_nearest(r); !maybe_nearest.has_value()) {
		//spdlog::info("No intersection found!");
		return {};
	}
	else {
		nearest = maybe_nearest.value();
	}
	if (renderParams_.preview) {
		const auto dif = nearest.material->diffuse_color();
		const auto spec = nearest.material->specular_color();
		const auto p = nearest.material->prob_diffuse();
		return Color(
			std::lerp(spec.r(),dif.r(),static_cast<double>(p)),
			std::lerp(spec.g(),dif.r(),static_cast<double>(p)),
			std::lerp(spec.b(),dif.b(),static_cast<double>(p))
		);
	}

	
	const auto uni{ std::uniform_real_distribution<>(0.0,1.0) };
	if (uni(rnd) < nearest.material->prob_diffuse())
	{
		const std::array<norm3, 3> base = norm3::orthogonalFromZ(nearest.normal);
		Color result{};
		const auto maxUSamples = depth < 1 ? renderParams_.numUSamples : 1;
		const auto maxVSamples = depth < 1 ? renderParams_.numVSamples : 1;
		for (auto vSample = 0u; vSample < maxVSamples; ++vSample) {
			for (auto uSample = 0u; uSample < maxUSamples; ++uSample) {
				const double u = (static_cast<double>(uSample) + uni(rnd)) / maxUSamples;
				const double v = (static_cast<double>(vSample) + uni(rnd)) / maxVSamples;
				const norm3 outbound = samples::hemispheresampleCosWeighted(base, u, v);

				result += radiance(
					Ray(nearest.hit_position, outbound),
					rnd, depth + 1);
			}
		}
		result = result / static_cast<double>(maxUSamples * maxVSamples);
		return nearest.material->light_color() +
			convolute(result, nearest.material->diffuse_color());
	}
	else {
		//const norm3 outbound = nearest.normal.reflect(r.direction());
		/*const norm3 outbound = samples::hemispheresampleUniform(nearest.normal.reflect(r.direction()), uni(rnd) * 5.0 / 180.0 * constants::pi, uni(rnd) * 2 * constants::pi);*/
		const std::array<norm3, 3> base = norm3::orthogonalFromZ(norm3(glm::reflect(r.direction().getVec(), nearest.normal.getVec())));
		const norm3 outbound = samples::conesampleCosWeighted(base,5.0 / 180.0 * constants::pi, uni(rnd), uni(rnd));

		return nearest.material->light_color() +
			convolute(
				radiance(Ray(nearest.hit_position, outbound), rnd, ++depth),
				nearest.material->specular_color()
			);
	}
}

auto Renderer::generateRenderJob(std::size_t sample) {
	/*const auto x_rezi = 1. / renderParams_.sizeX;
	const auto y_rezi = 1. / renderParams_.sizeY;*/
	std::random_device r_device;
	const unsigned int seed = static_cast<unsigned int>(r_device()) + static_cast<unsigned int>(sample);
	
	/*const norm3 dir = cam_.getRayDirection(x, y, rnd);
	const auto uni = std::uniform_real_distribution(0.0, 1.0);*/
	return 	[&, seed]() {
		auto rnd = std::mt19937(seed);
		auto pixels_ = std::vector<Color>(renderParams_.sizeX * renderParams_.sizeY);
		Ray r{};
		for (std::size_t y = 0; y < renderParams_.sizeY; ++y) {
			for (std::size_t x = 0; x < renderParams_.sizeX; ++x) {
				r = cam_.getRay(x, y, rnd);
				pixels_[y * renderParams_.sizeX + x] = radiance(r, rnd, 0);
			}
		}
		return std::move(pixels_); 
	};
}

void gammaCorrection(std::vector<Color>& image, double correctionValue = 1.0 / 2.2) {
	for (Color& c : image) {
		c.vec_ = glm::pow(c.vec_, decltype(c.vec_)(correctionValue));
	}
}

std::vector<Color> Renderer::render() {
	const auto numCores = std::thread::hardware_concurrency();
	auto threadPool = ThreadPool(numCores);
	std::vector<std::future<std::vector<Color>>> results;
	results.reserve(numCores);

	const auto addSample = [&](const std::vector<Color>&& sample) noexcept {
		for (std::size_t i = 0; i < pixels_.size(); ++i) {
			pixels_[i] += sample[i] / renderParams_.samplesPerPixel;
		}
	};
	auto samplesDone = 0u;
	auto samplesLeft = renderParams_.samplesPerPixel;
	auto pct = (100 * samplesDone) / renderParams_.samplesPerPixel;
	const auto checkFutureStatus = [](auto& f) {return f.wait_for(std::chrono::milliseconds(1)) == std::future_status::ready; };
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

	gammaCorrection(pixels_);

	return std::move(pixels_);
}

//std::vector<Color> Renderer::render() {
//	auto numCores = std::thread::hardware_concurrency();
//	std::vector<std::future<std::vector<Color>>> results;
//	results.reserve(numCores);
//	const auto addSample = [&](const std::vector<Color>& sample) noexcept {
//		for (std::size_t i = 0; i < pixels_.size(); ++i) {
//			pixels_[i] += sample[i] / renderParams_.samplesPerPixel;
//		}
//	};
//
//	auto samplesDone = 0u;
//	const auto checkFutureStatus = [](auto& f) {return f.wait_for(std::chrono::milliseconds(1)) == std::future_status::ready; };
//	auto pct = samplesDone / renderParams_.samplesPerPixel * 100;
//	std::size_t currentSample = 0;
//
//	const auto jobSpawner = [&]() {
//		const auto numLeft = renderParams_.samplesPerPixel - currentSample;
//		const auto cpusFree = numCores - results.size();
//		const auto numToSpawn = std::min<std::size_t>(numLeft, cpusFree);
//		for (std::size_t i = 0; i < numToSpawn; ++i) {
//			results.emplace_back(std::async(std::launch::async,generateRenderJob(currentSample)));
//			currentSample += i;
//		}
//	};
//
//	do {
//		jobSpawner();
//		for (auto readyFuture = std::find_if(results.begin(), results.end(), checkFutureStatus); readyFuture != results.end(); readyFuture = std::find_if(results.begin(), results.end(), checkFutureStatus)) {
//			addSample(readyFuture->get());
//			++samplesDone;
//			results.erase(readyFuture);
//		}
//
//		pct = (100 * samplesDone) / renderParams_.samplesPerPixel;
//		spdlog::info("{} Percent of samples done", pct);
//
//		std::this_thread::sleep_for(std::chrono::milliseconds(100));
//			
//	} while (samplesDone < renderParams_.samplesPerPixel);
//
//	gammaCorrection(pixels_);
//	return std::move(pixels_);
//}