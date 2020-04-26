#pragma once
#include "Renderer.h"
#include "Samples.h"
#include <vector>
#include <future>
#include <random>
#include "spdlog\spdlog.h"

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
		const auto dif = nearest.material.diffuse_color();
		const auto spec = nearest.material.specular_color();
		const auto p = nearest.material.prob_diffuse();
		return {
			.r{std::lerp(spec.r,dif.r,static_cast<double>(p))},
			.g{std::lerp(spec.g,dif.r,static_cast<double>(p))},
			.b{std::lerp(spec.b,dif.b,static_cast<double>(p))}
		};
	}
	const auto uni{ std::uniform_real_distribution<>(0.0,1.0) };
	if (uni(rnd) < nearest.material.prob_diffuse())
	{
		Color result{};
		const auto maxUSamples = depth < 1 ? renderParams_.numUSamples : 1;
		const auto maxVSamples = depth < 1 ? renderParams_.numVSamples : 1;
		for (auto vSample = 0u; vSample < maxVSamples; ++vSample) {
			for (auto uSample = 0u; uSample < maxUSamples; ++uSample) {
				const double u = (uSample + uni(rnd)) / maxUSamples;
				const double v = (vSample + uni(rnd)) / maxVSamples;
				const Norm3 outbound = samples::hemispheresampleCosWeighted(nearest.normal, u, v);

				result += radiance(
					Ray(nearest.hit_position, outbound),
					rnd, ++depth);
			}
		}
		result = result / static_cast<double>(maxUSamples * maxVSamples);
		return nearest.material.light_color() +
			convolute(result, nearest.material.diffuse_color());
	}
	else {
		//const Norm3 outbound = nearest.normal.reflect(r.direction());
		/*const Norm3 outbound = samples::hemispheresampleUniform(nearest.normal.reflect(r.direction()), uni(rnd) * 5.0 / 180.0 * constants::pi, uni(rnd) * 2 * constants::pi);*/
		const Norm3 outbound = samples::conesampleCosWeighted(nearest.normal.reflect(r.direction()),5.0 / 180.0 * constants::pi, uni(rnd), uni(rnd));

		return nearest.material.light_color() +
			convolute(
				radiance(Ray(nearest.hit_position, outbound), rnd, ++depth),
				nearest.material.specular_color()
			);
	}
}

auto Renderer::generatePixelJob(std::size_t x, std::size_t y) {
	const auto x_rezi = 1. / maxx_;
	const auto y_rezi = 1. / maxy_;
	std::random_device r_device;
	const unsigned int seed = r_device();
	auto rnd = std::mt19937(seed + y * maxx_ + x);
	const Norm3 dir = cam_.getRayDirection(x, y, rnd);
	const auto uni = std::uniform_real_distribution(0.0, 1.0);
	return [this, x_rezi, y_rezi, uni, rnd, dir, x, y]
	() mutable {
		Color c{};
		Ray r{};
		for (unsigned int s = 0; s < renderParams_.samplesPerPixel; ++s) {
			r = Ray({ (x + uni(rnd)) * x_rezi,(y + uni(rnd)) * y_rezi,0.05 }, dir);
			c += radiance(r, rnd, 0) / renderParams_.samplesPerPixel;
		}
		c = clamp(c, 0.0, 1.0);
		return (counter_++, c); };
}

std::size_t Renderer::percentRendered() {
	return 100 * counter_.get() / pixels_.size();
}

std::vector<Color> Renderer::render() {

	const auto renderSingleThread = [&]() {
		for (std::size_t y = 0; y < maxy_; ++y) {
			for (std::size_t x = 0; x < maxx_; ++x) {
				pixels_[y * maxx_ + x] = generatePixelJob(x, y)();
			}
		}
	};

	const auto renderMultiThread = [&]() {
		std::vector<std::future<Color>> results(pixels_.size());

		for (std::size_t y = 0; y < maxy_; ++y) {
			for (std::size_t x = 0; x < maxx_; ++x) {
				results[y * maxx_ + x] = std::async(std::launch::async, generatePixelJob(x, y));
			}
		}
		auto pct = percentRendered();
		spdlog::info("{} Percent of pixels done", pct);
		do {
			std::this_thread::sleep_for(std::chrono::seconds(2));
			pct = percentRendered();
			spdlog::info("{} Percent of pixels done", pct);
		} while (pct < 100);

		for (std::size_t i = 0; i < pixels_.size(); ++i) {
			pixels_[i] = results[i].get();
		}
	};

	multithread_ ? renderMultiThread() : renderSingleThread();
	return pixels_;
}