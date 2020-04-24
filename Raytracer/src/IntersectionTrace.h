#pragma once
#include "Ray.h"
#include "Norm3.h"
#include "Material.h"

struct IntersectionTrace {
	bool inside{ false };
	Norm3 normal{Norm3::xAxis()};
	Vec3 hit_position{};
	double distance{};
	Material material{};
	
	constexpr IntersectionTrace(bool inside, Norm3 normal, Vec3 hit_position, double distance, Material mat) noexcept
		: inside(inside), normal(normal), hit_position(hit_position), distance(distance), material(mat) {}
	constexpr IntersectionTrace() = default;
};