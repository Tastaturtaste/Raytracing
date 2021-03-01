#pragma once
#include <vector>
#include <array>
#include <optional>
#include <glm\vec3.hpp>
#include "Sphere.h"
#include "Triangle.h"
#include "Material.h"
#include "IntersectionTrace.h"
#include "Geometry.h"
#include <variant>
#include <memory>


class Scene
{
	std::vector<Geometry> geometries_{};
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

	inline Scene(std::vector<Geometry> geometries, std::vector<std::unique_ptr<Material>>&& materials)
		: geometries_(std::move(geometries)), materials_(std::move(materials)) {}
	inline Scene(std::vector<Geometry> geometries, std::vector<std::unique_ptr<Material>>&& materials, 
		norm3 basisX, norm3 basisY, norm3 basisZ)
		: geometries_(geometries), materials_(std::move(materials)), basisX_(basisX), basisY_(basisY), basisZ_(basisZ) {}

	[[nodiscard]] constexpr auto& geometries() const noexcept { return geometries_; }
	inline void add_geometry(Geometry geo) { geometries_.push_back(std::move(geo)); }
	inline void add_sphere(Sphere sphere) { geometries_.push_back(std::move(sphere)); }
	inline void add_triangle(Triangle tri) { geometries_.push_back(std::move(tri)); }
	[[nodiscard]] std::optional<IntersectionTrace> find_nearest (const Ray& r) const;

	[[nodiscard]] constexpr norm3 basisX() const noexcept { return basisX_; }
	[[nodiscard]] constexpr norm3 basisY() const noexcept { return basisY_; }
	[[nodiscard]] constexpr norm3 basisZ() const noexcept { return basisZ_; }
};



namespace Scenes {
	Scene basic_scene();
	//Scene basic_scene_with_triangle();
} // Scenes