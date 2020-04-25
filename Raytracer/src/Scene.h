#pragma once
#include <vector>
#include <array>
#include <optional>
#include "Sphere.h"
#include "Material.h"
#include "IntersectionTrace.h"

class Scene
{
	std::vector<Sphere> spheres_{};
public:
	Scene(std::vector<Sphere> spheres)
		: spheres_(spheres) {}

	[[nodiscard]] constexpr auto& spheres() const noexcept { return spheres_; }

	void add_sphere(Sphere sphere) { spheres_.push_back(std::move(sphere)); }
	std::optional<IntersectionTrace> find_nearest (const Ray& r) const noexcept;

	static Scene basic_scene();
	static Scene hull_sphere();
};