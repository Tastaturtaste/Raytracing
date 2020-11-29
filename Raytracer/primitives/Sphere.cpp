#include "Sphere.h"
#include <glm\glm.hpp>

std::optional<IntersectionTrace> Sphere::intersect(const Ray& r) const noexcept {
	const glm::dvec3 op = origin_ - r.origin(); // Solve t^2*d.d + 2*t*(o-p).d + (o-p).(o-p)-R^2 = 0
	const double b = glm::dot(r.direction().getVec(),op);
	const double r_squared = radius_ * radius_;
	double det = b * b - glm::dot(op,op) + r_squared;
	if (det < 0)
		return {};

	det = glm::sqrt(det);
	const auto minusT = b - det;
	const auto plusT = b + det;
	if (minusT < constants::EPS && plusT < constants::EPS)
		return {};

	const auto t = minusT > constants::EPS ? minusT : plusT;
	const auto hitPosition = r.positionAlong(t);
	auto normal = this->normal(hitPosition);
	const bool inside = glm::dot(normal.getVec(),r.direction().getVec()) > 0;
	if (inside)
		normal = -normal;
	return IntersectionTrace(inside, normal, hitPosition, t, material());
}