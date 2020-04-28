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
	Norm3 basisX_{ Vec3(1.0,0.0,0.0).norm() };
	Norm3 basisY_{ Vec3(0.0,1.0,0.0).norm() };
	Norm3 basisZ_{ Vec3(0.0,0.0,1.0).norm() };
public:
	Scene(std::vector<Sphere> spheres)
		: spheres_(spheres) {}
	Scene(std::vector<Sphere> spheres, Norm3 basisX, Norm3 basisY, Norm3 basisZ)
		: spheres_(spheres), basisX_(basisX), basisY_(basisY), basisZ_(basisZ) {}

	[[nodiscard]] constexpr auto& spheres() const noexcept { return spheres_; }

	void add_sphere(Sphere sphere) { spheres_.push_back(std::move(sphere)); }
	std::optional<IntersectionTrace> find_nearest (const Ray& r) const noexcept;

	[[nodiscard]] constexpr Norm3 basisX() const noexcept { return basisX_; }
	[[nodiscard]] constexpr Norm3 basisY() const noexcept { return basisY_; }
	[[nodiscard]] constexpr Norm3 basisZ() const noexcept { return basisZ_; }

	static Scene basic_scene();
	static Scene hull_sphere();
	static Scene debug_scene();
};