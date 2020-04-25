#include "Constants.h"
#include "Ray.h"
#include "Norm3.h"
#include "Vec3.h"
#include "Sphere.h"
#include "Color.h"
#include <vector>
#include <optional>
#include <random>
#include <cmath>
#include <future>
#include <atomic>
#include <chrono>
#include <mutex>
#include "Scene.h"
#include "IntersectionTrace.h"
#include "RenderParams.h"
#include "pngwriter.h"
#include "spdlog\spdlog.h"

constexpr bool single_thread = false;

std::optional<IntersectionTrace> intersect(const Sphere& sphere, const Ray& r) noexcept {
	const Vec3 op = sphere.origin() - r.origin(); // Solve t^2*d.d + 2*t*(o-p).d + (o-p).(o-p)-R^2 = 0
	const double b = op.dot(r.direction());
	const double r_squared = sphere.radius() * sphere.radius();
	double det = b * b - op.lensq() + r_squared;
	if (det < 0)
		return {};

	det = sqrt(det);
	const auto minusT = b - det;
	const auto plusT = b + det;
	if (minusT < constants::EPS && plusT < constants::EPS)
		return {};

	const auto t = minusT > constants::EPS ? minusT : plusT;
	const auto hitPosition = r.positionAlong(t);
	auto normal = sphere.getNormal(hitPosition);
	const bool inside = normal.dot(r.direction()) > 0;
	if (inside)
		normal = -normal;
	return IntersectionTrace(inside, normal, hitPosition, t, sphere.material());
}

std::optional<IntersectionTrace> find_nearest(const Scene& scene, const Ray& r) noexcept {
	std::optional<IntersectionTrace> nearest{};
	std::optional<IntersectionTrace> temp{};
	double min_dist = std::numeric_limits<double>::infinity();
	for (const Sphere& sphere : scene.spheres()) {
		temp = intersect(sphere, r);
		if (temp.has_value() && temp->distance < min_dist) {
			min_dist = temp->distance;
			nearest = temp;
		}
	}
	return nearest;
}

struct CameraParams {
	double viewAngle{ 0.25 * constants::pi };
};

Norm3 hemispheresample(Norm3 normal, double aroundX, double aroundZ) noexcept {
	const auto xy = Norm3::orthogonal_from_z(normal);
	const auto x = xy.first;
	const auto y = xy.second;
	return normal.rotate(x, aroundX).rotate(normal, aroundZ);
}

Norm3 hemispheresample2(Norm3 normal, double u, double v) noexcept {
	const auto xy = Norm3::orthogonal_from_z(normal);
	const auto x = xy.first;
	const auto y = xy.second;

	auto theta = 2 * constants::pi * u;
	auto radiusSquared = v;
	auto radius = sqrt(radiusSquared);
	auto pos = Vec3(cos(theta) * radius, sin(theta) * radius, sqrt(1 - radiusSquared));
	return pos.transform( { x,y,normal }).norm();
}

Color radiance(const Ray& r, const Scene& scene, std::mt19937& rnd, const RenderParams& params, unsigned int depth) {
	if (depth > params.max_depth)
		return {};
	IntersectionTrace nearest{};
	if (auto maybe_nearest = find_nearest(scene, r); !maybe_nearest.has_value()) {
		//spdlog::info("No intersection found!");
		return {};
	}
	else {
		nearest = maybe_nearest.value();
	}
	
	if (params.preview) {
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
		const auto maxUSamples = depth < 1 ? params.numUSamples : 1;
		const auto maxVSamples = depth < 1 ? params.numVSamples : 1;
		for (auto vSample = 0; vSample < maxVSamples; ++vSample) {
			for (auto uSample = 0; uSample < maxUSamples; ++uSample) {
				/*const double aroundZ = 2 * constants::pi * (uSample + uni(rnd)) / maxUSamples;
				const double aroundX = 0.5 * constants::pi * (vSample + uni(rnd)) / maxVSamples;
				const Norm3 outbound = hemispheresample(nearest.normal, aroundX, aroundZ);*/
				const double u = (uSample + uni(rnd)) / maxUSamples;
				const double v = (vSample + uni(rnd)) / maxVSamples;
				const Norm3 outbound = hemispheresample2(nearest.normal, u, v);

				result += radiance(
					Ray(nearest.hit_position, outbound),
					scene, rnd, params, ++depth);
			}
		}
		result = result / static_cast<double>(maxUSamples * maxVSamples);
		return nearest.material.light_color() +
			convolute(result, nearest.material.diffuse_color());
	}
	else {
		//const Norm3 outbound = nearest.normal.reflect(r.direction());
		const Norm3 outbound = hemispheresample(nearest.normal.reflect(r.direction()), uni(rnd)*5.0 / 180.0 * constants::pi, uni(rnd) * 2 * constants::pi);
		return nearest.material.light_color() +
			convolute(
				radiance(Ray(nearest.hit_position, outbound),scene, rnd, params, ++depth),
				nearest.material.specular_color()
			);
	}
}

struct Counter {
	unsigned int i{};
	std::mutex m{};
	unsigned int operator++(int) {
		const std::lock_guard<std::mutex> lock(m);
		const auto temp = i;
		++i;
		return temp;
	}
	unsigned int get() {
		const std::lock_guard<std::mutex> lock(m);
		return i;
	}
	//explicit operator unsigned int() const { return i; }
};

int main(int argc, char* argv[]) {
	constexpr std::size_t maxx = 720;
	constexpr std::size_t maxy = 720;
	std::vector<Color> pixels(maxx*maxy); // count along row
	auto png = pngwriter(maxx, maxy, 0.0, "basic_scene.png");

	constexpr double x_rezi = 1. / maxx;
	constexpr double y_rezi = 1. / maxy;

	/*const Scene scene = Scene::basic_scene();*/
	const Scene scene{ Scene::hull_sphere() };
	auto rng = std::mt19937();
	const auto uni = std::uniform_real_distribution(0.0, 1.0);

	const RenderParams renderParams{
		.numUSamples{4}
		,.numVSamples{4}
		,.preview{false}
		,.max_depth{5}
		,.samplesPerPixel{1000}
	};
	const CameraParams camParams{};

	Counter counter{};

	auto job = [&counter, &scene, &renderParams, x_rezi, y_rezi](std::size_t x, std::size_t y, std::mt19937 rng) {
		const auto uni = std::uniform_real_distribution(0.0, 1.0);
		Color c{};
		Ray r{};
		for (unsigned int s = 0; s < renderParams.samplesPerPixel; ++s) {
			r = Ray({ (x + uni(rng)) * x_rezi,(y + uni(rng)) * y_rezi,0.05 }, Norm3::zAxis());
			c += radiance(r, scene, rng, renderParams, 0) / renderParams.samplesPerPixel;
		}
		c = clamp(c , 0.0, 1.0);
		return (counter++, c);};

	Color c{};
	if (single_thread){
		for (std::size_t y = 0; y < maxy; ++y) {
			for (std::size_t x = 0; x < maxx; ++x) {
				c = job(x, y, rng);
				pixels[y * maxx + x] = c;
				c = {};
			}
		}
	}
	else {
		std::vector<std::future<Color>> results(maxx*maxy);
		assert(("array of pixels and array of futures are not of same length!",pixels.size() == results.size()));
		std::random_device r_device;
		const unsigned int seed = r_device();
		for (std::size_t y = 0; y < maxy; ++y) {
			for (std::size_t x = 0; x < maxx; ++x) {
				results[y * maxx + x] = std::async(std::launch::async, job, x, y, std::mt19937(seed + y * maxx + x));
				//std::mt19937(r_device()+y*maxx + x)
			}
		}
		const auto lastco = counter.get() / (maxx * maxy);
		spdlog::info("{} Percent of pixels done", 100 * lastco);
		do {
			std::this_thread::sleep_for(std::chrono::seconds(2));
			/*if (counter.get() / (maxx * maxy) - lastco > 0.01) {
				lastco = counter.get() / (maxx * maxy);
				spdlog::info("{} Percent of pixels done", 100 * lastco);
			}*/
			spdlog::info("{} Percent of pixels done", 100 * counter.get() / (maxx*maxy));
		} while (counter.get() < maxx * maxy);
		for (std::size_t i = 0; i < pixels.size(); ++i) {
			pixels[i] = results[i].get();
		}
	}

	for (std::size_t y = 0; y < maxy; ++y) {
		for (std::size_t x = 0; x < maxx; ++x) {
			png.plot(static_cast<int>(x), static_cast<int>(y), pixels[y * maxx + x].r, pixels[y * maxx + x].g, pixels[y * maxx + x].b);
		}
	}

	png.close();

	return 0;
}