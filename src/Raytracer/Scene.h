#pragma once
#include <vector>
#include <array>
#include <optional>
#include <variant>
#include <memory>

#include <glm\vec3.hpp>

#include "primitives/Sphere.h"
#include "primitives/Triangle.h"
#include "Material.h"
#include "IntersectionTrace.h"
#include "primitives/Geometry.h"
#include "OcTree.h"

#define TREE

class Scene
{
	#ifndef TREE
	std::vector<Geometry> geometries_{};
	#else
	OcTree tree_;
	#endif
	std::vector<std::unique_ptr<Material>> materials_{};
	norm3 basisX_{ glm::dvec3(1.0,0.0,0.0) };	// right
	norm3 basisY_{ glm::dvec3(0.0,1.0,0.0) };	// forward
	norm3 basisZ_{ glm::dvec3(0.0,0.0,1.0) };	// up
public:
	Scene() = default;
	Scene(Scene const&) = delete;
	Scene(Scene&&) = default;
	Scene& operator=(Scene const& rhs) = delete;
	Scene& operator=(Scene && rhs) = default;

	#ifndef TREE
	inline Scene(std::vector<Geometry> geometries, std::vector<std::unique_ptr<Material>>&& materials)
		: geometries_(std::move(geometries)), materials_(std::move(materials)) {}
	inline Scene(std::vector<Geometry> geometries, std::vector<std::unique_ptr<Material>>&& materials, 
		norm3 basisX, norm3 basisY, norm3 basisZ)
		: geometries_(geometries), materials_(std::move(materials)), basisX_(basisX), basisY_(basisY), basisZ_(basisZ) {}
	#else
	inline Scene(std::vector<Geometry> geometries, std::vector<std::unique_ptr<Material>>&& materials)
		: tree_(std::move(geometries)), materials_(std::move(materials)) {}
	inline Scene(std::vector<Geometry> geometries, std::vector<std::unique_ptr<Material>>&& materials,
		norm3 basisX, norm3 basisY, norm3 basisZ)
		: tree_(geometries), materials_(std::move(materials)), basisX_(basisX), basisY_(basisY), basisZ_(basisZ) {}
	#endif // !TREE


	//[[nodiscard]] constexpr auto& geometries() const noexcept { return tree_.getGeometries(); }
	//inline void add_geometry(Geometry geo) { geometries_.push_back(std::move(geo)); }
	//inline void add_sphere(Sphere sphere) { geometries_.push_back(std::move(sphere)); }
	//inline void add_triangle(Triangle tri) { geometries_.push_back(std::move(tri)); }
	[[nodiscard]] std::optional<IntersectionTrace> find_nearest (const Ray& r) const;

	[[nodiscard]] constexpr norm3 basisX() const noexcept { return basisX_; }
	[[nodiscard]] constexpr norm3 basisY() const noexcept { return basisY_; }
	[[nodiscard]] constexpr norm3 basisZ() const noexcept { return basisZ_; }
};

inline std::optional<IntersectionTrace> Scene::find_nearest(const Ray& r) const {
	#ifndef TREE
	std::optional<IntersectionTrace> nearest;
	double min_dist = std::numeric_limits<double>::infinity();
	for (const Geometry& geometry : geometries_) {
		auto temp = geometry.intersect(r);
		if (temp.has_value() && temp->distance < min_dist) {
			min_dist = temp->distance;
			nearest.emplace(std::move(temp.value()));
		}
	}
	return nearest;
	#else
	return tree_.intersect(r);

	#endif // !TREE 
}

namespace Scenes {
	Scene basic_scene();
	//Scene basic_scene_with_triangle();
} // Scenes