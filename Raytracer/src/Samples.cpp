#include "Samples.h"
#include <cmath>
#include <glm\glm.hpp>
#include <mutex>


namespace Samples {

	norm3 transform(const std::array<norm3, 3>& basis, const glm::dvec3& vec) {
		return norm3(basis[0].getVec() * vec.x + basis[1].getVec() * vec.y + basis[2].getVec() * vec.z);
	}

	norm3 hemispheresampleCosWeighted(const std::array<norm3, 3>& basis, double u, double v)  {
		const auto theta = 2 * constants::pi * u;
		const auto radiusSquared = v;
		const auto radius = glm::sqrt(radiusSquared);
		const auto sample = glm::dvec3(glm::cos(theta) * radius, glm::sin(theta) * radius, glm::sqrt(1.0 - radiusSquared));
		return transform(basis,sample);
	}

	norm3 conesampleCosWeighted(const std::array<norm3, 3>& basis, double coneAngle, double u, double v)  {
		coneAngle = coneAngle * (1.0 - (2.0 * glm::acos(u) / constants::pi));
		const double radius = glm::sin(coneAngle);
		const double z = glm::cos(coneAngle);
		const double randomAngle = v * 2 * constants::pi;
		const auto sample = glm::dvec3(glm::cos(randomAngle) * radius, glm::sin(randomAngle) * radius, z);
		return transform(basis, sample);
	}
}