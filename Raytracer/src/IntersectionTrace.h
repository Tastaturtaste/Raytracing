#pragma once
#include "Ray.h"
#include "norm3.h"
#include "Material.h"
#include <glm\glm.hpp>

struct IntersectionTrace {
	glm::dvec3 hit_position;
	norm3 normal;
	double distance{};
	Material const* material{ nullptr };
	bool inside{ false };
};