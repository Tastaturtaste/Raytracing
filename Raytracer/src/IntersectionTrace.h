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
	const Material& material;
	
	constexpr IntersectionTrace(bool inside, const norm3& normal, const glm::dvec3& hit_position, double distance, const Material& mat) noexcept
		: inside(inside), normal(normal), hit_position(hit_position), distance(distance), material(mat) {}
	constexpr IntersectionTrace(const IntersectionTrace& in) = default;
	constexpr IntersectionTrace operator=(IntersectionTrace&) = delete;
	constexpr IntersectionTrace(IntersectionTrace&& in) = default;
	constexpr IntersectionTrace operator=(IntersectionTrace&&) = delete;
	~IntersectionTrace() = default;
};