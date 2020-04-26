#include "Samples.h"
#include <tuple>
#include <cmath>

namespace samples {

	Norm3 hemispheresampleCosWeighted(Norm3 normal, double u, double v) noexcept {
		const auto xy = Norm3::orthogonal_from_z(normal);
		const auto x = xy.first;
		const auto y = xy.second;

		const auto theta = 2 * constants::pi * u;
		const auto radiusSquared = v;
		const auto radius = sqrt(radiusSquared);
		const auto sample = Vec3(cos(theta) * radius, sin(theta) * radius, sqrt(1 - radiusSquared));
		return sample.transform({ x,y,normal }).norm();
	}

	Norm3 hemispheresampleUniform(Norm3 normal, double aroundX, double aroundZ) noexcept {
		const auto xy = Norm3::orthogonal_from_z(normal);
		const auto x = xy.first;
		const auto y = xy.second;
		return normal.rotate(x, aroundX).rotate(normal, aroundZ);
	}

	Norm3 conesampleCosWeighted(Norm3 normal, double coneAngle, double u, double v) noexcept {
		const auto xy = Norm3::orthogonal_from_z(normal);
		const auto x = xy.first;
		const auto y = xy.second;

		const auto theta = 2 * constants::pi * u;
		const auto radiusSquared = sin(coneAngle) * v;
		const auto radius = sqrt(radiusSquared);
		const auto sample = Vec3(cos(theta) * radius, sin(theta) * radius, sqrt(1 - radiusSquared));
		return sample.transform({ x,y,normal }).norm();
	}
}