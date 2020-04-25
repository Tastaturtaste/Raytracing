#include "Sphere.h"
#include <cmath>

std::optional<IntersectionTrace> Sphere::intersect(const Ray& r) const noexcept {
	const Vec3 op = origin_ - r.origin(); // Solve t^2*d.d + 2*t*(o-p).d + (o-p).(o-p)-R^2 = 0
	const double b = op.dot(r.direction());
	const double r_squared = radius_ * radius_;
	double det = b * b - op.lensq() + r_squared;
	if (det < 0)
		return {};

	det = sqrt(det);
	const auto minusT = b - det;
	const auto plusT = b + det;
	if (minusT < constants::EPS&& plusT < constants::EPS)
		return {};

	const auto t = minusT > constants::EPS ? minusT : plusT;
	const auto hitPosition = r.positionAlong(t);
	auto normal = this->getNormal(hitPosition);
	const bool inside = normal.dot(r.direction()) > 0;
	if (inside)
		normal = -normal;
	return IntersectionTrace(inside, normal, hitPosition, t, mat_);
}