#pragma once
#include "Ray.h"
#include "norm3.h"
#include "Material.h"
#include <glm\glm.hpp>

struct IntersectionTrace {
	bool inside{ false };
	norm3 normal{norm3::xAxis()};
	glm::dvec3 hit_position{};
	double distance{};
	Material material{};
	
	constexpr IntersectionTrace(bool inside, norm3 normal, glm::dvec3 hit_position, double distance, Material mat) noexcept
		: inside(inside), normal(normal), hit_position(hit_position), distance(distance), material(mat) {}
	constexpr IntersectionTrace() = default;
};