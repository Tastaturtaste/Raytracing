#include <glm\glm.hpp>
#include "Samples.h"
#include "util/Constants.h"

namespace Samples {

	inline norm3 transform(const std::array<norm3, 3>& basis, const glm::dvec3& vec) noexcept {
		return norm3(basis[0].getVec() * vec.x + basis[1].getVec() * vec.y + basis[2].getVec() * vec.z);
	}

	norm3 hemispheresampleCosWeighted(const std::array<norm3, 3>& basis, double u, double v) noexcept {
		const auto theta = 2 * constants::pi * u;
		const auto radius = glm::sqrt(v);
		const auto x = radius * glm::cos(theta);
		const auto z = radius * glm::sin(theta);
		const auto y = glm::sqrt(1.0 - x * x - z * z);
		return transform(basis,glm::dvec3(x,y,z));
	}

	norm3 conesampleCosWeighted(const std::array<norm3, 3>& basis, double coneAngle, double u, double v) noexcept {
		coneAngle = coneAngle * (1.0 - (2.0 * glm::acos(u) / constants::pi));
		const double radius = glm::sin(coneAngle);
		const double y = glm::cos(coneAngle);
		const double randomAngle = v * 2 * constants::pi;
		const auto sample = glm::dvec3(glm::cos(randomAngle) * radius, y, glm::sin(randomAngle) * radius);
		return transform(basis, sample);
	}

	//norm3 conesampleCosWeighted(const std::array<norm3, 3>& basis, double coneAngle, double u, double v) noexcept {
	//	// Uniform sampling in a circle projected into a hemisphere
	//	const auto theta = 2 * constants::pi * u;
	//	const auto halfConeAngle = 0.5 * coneAngle * v;
	//	const auto r = glm::sin(halfConeAngle);

	//	const auto x = glm::cos(theta) * r;
	//	const auto y = glm::cos(halfConeAngle);
	//	const auto z = glm::sin(theta) * r;
	//	const auto sample = glm::dvec3(x, y, z);
	//	return transform(basis, sample);
	//}
}