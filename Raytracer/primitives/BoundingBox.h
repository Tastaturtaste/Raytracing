#pragma once
#include <glm\glm.hpp>
#include "Ray.h"
#include "utilities.h"


struct BoundingBox {
	glm::dvec3 min{}; // bottom-left-front
	glm::dvec3 max{}; // top-right-back

	bool intersect(Ray const& ray) const {
		// https://tavianator.com/2011/ray_box.html
		glm::vec3 const inv{ 1. / ray.direction().getVec() };
		auto tx1 = (min.x - ray.origin().x) * inv.x;
		auto tx2 = (max.x - ray.origin().x) * inv.x;

		auto tmin = glm::min(tx1, tx2);
		auto tmax = glm::max(tx1, tx2);

		auto ty1 = (min.y - ray.origin().y) * inv.y;
		auto ty2 = (max.y - ray.origin().y) * inv.y;

		tmin = glm::max(tmin, glm::min(ty1, ty2));
		tmax = glm::min(tmax, glm::max(ty1, ty2));

		auto tz1 = (min.z - ray.origin().z) * inv.z;
		auto tz2 = (max.z - ray.origin().z) * inv.z;

		tmin = glm::max(tmin, glm::min(tz1, tz2));
		tmax = glm::min(tmax, glm::max(tz1, tz2));

		return tmax >= tmin;
	}
};

constexpr BoundingBox combineBoundingBoxes(BoundingBox const& b1, BoundingBox const& b2) noexcept {
	return {
		.min = elemwiseMin(b1.min, b2.min),
		.max = elemwiseMax(b1.max, b2.max)
	};
}