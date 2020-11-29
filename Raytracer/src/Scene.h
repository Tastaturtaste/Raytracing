#pragma once
#include <vector>
#include <array>
#include <optional>
#include <glm\vec3.hpp>
#include "Sphere.h"
#include "Material.h"
#include "IntersectionTrace.h"

class Scene
{
	std::vector<Sphere> spheres_{};
	norm3 basisX_{ glm::dvec3(1.0,0.0,0.0) };	// right
	norm3 basisY_{ glm::dvec3(0.0,1.0,0.0) };	// forward
	norm3 basisZ_{ glm::dvec3(0.0,0.0,1.0) };	// up
public:
	Scene(std::vector<Sphere> spheres)
		: spheres_(spheres) {}
	Scene(std::vector<Sphere> spheres, norm3 basisX, norm3 basisY, norm3 basisZ)
		: spheres_(spheres), basisX_(basisX), basisY_(basisY), basisZ_(basisZ) {}

	[[nodiscard]] constexpr auto& spheres() const noexcept { return spheres_; }

	void add_sphere(Sphere sphere) { spheres_.push_back(std::move(sphere)); }
	[[nodiscard]] std::optional<IntersectionTrace> find_nearest (const Ray& r) const;

	[[nodiscard]] constexpr norm3 basisX() const noexcept { return basisX_; }
	[[nodiscard]] constexpr norm3 basisY() const noexcept { return basisY_; }
	[[nodiscard]] constexpr norm3 basisZ() const noexcept { return basisZ_; }

	static Scene basic_scene();
	static Scene hull_sphere();
	static Scene debug_scene();
	static Scene light_distribution();
};