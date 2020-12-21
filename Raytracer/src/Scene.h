#pragma once
#include <vector>
#include <array>
#include <optional>
#include <glm\vec3.hpp>
#include "Sphere.h"
#include "Triangle.h"
#include "Material.h"
#include "IntersectionTrace.h"
#include <variant>


template<class... Geometries>
class Scene
{
	using Geometry = std::variant<Geometries...>;
	std::vector<Geometry> geometries_{};
	norm3 basisX_{ glm::dvec3(1.0,0.0,0.0) };	// right
	norm3 basisY_{ glm::dvec3(0.0,1.0,0.0) };	// forward
	norm3 basisZ_{ glm::dvec3(0.0,0.0,1.0) };	// up
public:
	constexpr Scene(std::vector<Geometry> geometries)
		: geometries_(geometries) {}
	constexpr Scene(std::vector<Geometry> geometries, norm3 basisX, norm3 basisY, norm3 basisZ)
		: geometries_(geometries), basisX_(basisX), basisY_(basisY), basisZ_(basisZ) {}

	[[nodiscard]] constexpr auto& geometries() const noexcept { return geometries_; }

	void add_sphere(Sphere sphere) { geometries_.push_back(std::move(sphere)); }
	[[nodiscard]] std::optional<IntersectionTrace> find_nearest (const Ray& r) const;

	[[nodiscard]] constexpr norm3 basisX() const noexcept { return basisX_; }
	[[nodiscard]] constexpr norm3 basisY() const noexcept { return basisY_; }
	[[nodiscard]] constexpr norm3 basisZ() const noexcept { return basisZ_; }
};

template<class... Geometries>
std::optional<IntersectionTrace> Scene<Geometries...>::find_nearest(const Ray& r) const {
	std::optional<IntersectionTrace> nearest;
	double min_dist = std::numeric_limits<double>::infinity();
	for (const Geometry& geometry : geometries_) {
		auto temp = std::visit([&r](auto const& g) noexcept {
			return g.intersect(r);
			}, geometry);
		//auto temp = sphere.intersect(r);
		if (temp.has_value() && temp->distance < min_dist) {
			min_dist = temp->distance;
			nearest.emplace(std::move(temp.value()));
		}
	}
	return nearest;
}

namespace Scenes {

inline Scene<Sphere> basic_scene()
{
	constexpr double greyval = 0.2;
	return Scene<Sphere>({
	Sphere(glm::dvec3(1e5,		0.5,		0.5),		1e5, Material::grey_diffuse(greyval)),		// left wall
	Sphere(glm::dvec3(-1e5 + 1.0,	0.5,		0.5),		1e5, Material::grey_diffuse(greyval)),		// right wall
	Sphere(glm::dvec3(0.5,		0.5,		-1e5 + 1),	1e5, Material::grey_diffuse(greyval)),		// ceiling
	Sphere(glm::dvec3(0.5,		0.5,		1e5),		1e5, Material::grey_diffuse(greyval)),		// floor
	Sphere(glm::dvec3(0.5,		-1e5 + 1,		0.5),		1e5, Material::grey_diffuse(greyval)),		// backwall
	Sphere(glm::dvec3(0.5,		1e5,		0.5),		1e5, Material::grey_diffuse(greyval)),		// frontwall
	Sphere(glm::dvec3(0.4,		0.2,		0.85),		0.1, Material::white_light(50.0)),	// lightsource
	Sphere(glm::dvec3(0.7,		0.5,		0.2),		0.2, Material::green_diffuse()),	// green sphere
	Sphere(glm::dvec3(0.2,		0.7,		0.2),		0.2, Material::red_shiny())			// shine red sphere
		});
}

inline Scene<Sphere, Triangle> basic_scene_with_triangle()
{
	constexpr double greyval = 0.2;
	return Scene<Sphere, Triangle>({
	Sphere(glm::dvec3(1e5,		0.5,		0.5),		1e5, Material::grey_diffuse(greyval)),		// left wall
	Sphere(glm::dvec3(-1e5 + 1.0,	0.5,		0.5),		1e5, Material::grey_diffuse(greyval)),		// right wall
	Sphere(glm::dvec3(0.5,		0.5,		-1e5 + 1),	1e5, Material::grey_diffuse(greyval)),		// ceiling
	Sphere(glm::dvec3(0.5,		0.5,		1e5),		1e5, Material::grey_diffuse(greyval)),		// floor
	Sphere(glm::dvec3(0.5,		-1e5 + 1,		0.5),		1e5, Material::grey_diffuse(greyval)),		// backwall
	Sphere(glm::dvec3(0.5,		1e5,		0.5),		1e5, Material::grey_diffuse(greyval)),		// frontwall
	Sphere(glm::dvec3(0.4,		0.2,		0.85),		0.1, Material::white_light(50.0)),	// lightsource
	Sphere(glm::dvec3(0.7,		0.5,		0.2),		0.2, Material::green_diffuse()),	// green sphere
	Sphere(glm::dvec3(0.2,		0.7,		0.2),		0.2, Material::red_shiny()),			// shine red sphere
	Triangle({glm::dvec3(0.5,0.7,0.9),glm::dvec3(0.9,0.7,0.1),glm::dvec3(0.1,0.7,0.1)},Material::red_shiny())
		});
}

//inline Scene<Sphere> hull_sphere()
//{
//	constexpr double greyval = 0.6;
//	return Scene<Geometries...>({
//	Sphere(glm::dvec3(0.5,	 0.5,	0.5),		2.0, Material::grey_diffuse(greyval)),		// wall
//	Sphere(glm::dvec3(0.1,	-0.2,	0.85),		0.1, Material::white_light(20.0)),	// lightsource
//	Sphere(glm::dvec3(0.7,	 0.5,	0.2),		0.2, Material::green_diffuse()),	// green sphere
//	Sphere(glm::dvec3(0.2,	 0.7,	0.2),		0.2, Material::red_shiny()),		// shine red sphere
//	Sphere(glm::dvec3(0.5,	 0.8,	0.5),		0.2, Material::mirror())			// mirror sphere
//		});
//}
//
//inline Scene<Sphere> light_distribution()
//{
//	constexpr double greyval = 0.6;
//	return Scene<Geometries...>({
//	Sphere(glm::dvec3(0.5,	 0.5,	0.5),		2.0, Material::grey_diffuse(greyval)),		// wall
//	Sphere(glm::dvec3(0.5,	 2.5,	0.5),		0.05, Material::green_diffuse()),	// middle backwall
//	Sphere(glm::dvec3(0.5,	-0.2,	0.5),		0.1, Material::white_light(100.0)),	// lightsource
//		});
//}
//
//inline Scene<Sphere> debug_scene()
//{
//	return Scene<Geometries...>({
//		Sphere(glm::dvec3(0.5,0.5,0.5),	0.1,	Material::white_light(20.0)),		// lightsource in the middle of scene
//		Sphere(glm::dvec3(0.2,0.5,0.5),	0.1,	Material::grey_diffuse(0.6)),			// gray sphere left
//		Sphere(glm::dvec3(0.8,0.5,0.5),	0.1,	Material::grey_diffuse(0.6)),			// gray sphere right
//		Sphere(glm::dvec3(0.5,0.5,0.2),	0.1,	Material::grey_diffuse(0.6)),			// gray sphere bottom
//		Sphere(glm::dvec3(0.5,0.5,0.8),	0.1,	Material::grey_diffuse(0.6))			// gray sphere top
//		});
//}

} // Scenes