#include "Samples.h"
#include <cmath>
#include <glm\glm.hpp>

namespace samples {

	norm3 transform(const std::array<norm3, 3>& basis, const glm::dvec3& vec) {
		return norm3(basis[0].getVec() * vec.x + basis[1].getVec() * vec.y + basis[2].getVec() * vec.z);
	}

	norm3 hemispheresampleCosWeighted(const std::array<norm3, 3>& basis, double u, double v)  {
		//const auto xyz = norm3::orthogonalFromZ(normal);
		//const auto x = xy[0];
		//const auto y = xy[1];

		const auto theta = 2 * constants::pi * u;
		const auto radiusSquared = v;
		const auto radius = glm::sqrt(radiusSquared);
		const auto sample = glm::dvec3(glm::cos(theta) * radius, glm::sin(theta) * radius, glm::sqrt(1 - radiusSquared));
		return transform(basis,sample);
	}

	norm3 conesampleCosWeighted(const std::array<norm3, 3>& basis, double coneAngle, double u, double v)  {
		//const auto xyz = norm3::orthogonalFromZ(normal);
		//const auto x = xy[0];
		//const auto y = xy[1];

		const auto theta = 2 * constants::pi * u;
		const auto radiusSquared = glm::sin(coneAngle) * v;
		const auto radius = glm::sqrt(radiusSquared);
		const auto sample = glm::dvec3(glm::cos(theta) * radius, glm::sin(theta) * radius, glm::sqrt(1 - radiusSquared));
		return transform(basis,sample);
	}
}