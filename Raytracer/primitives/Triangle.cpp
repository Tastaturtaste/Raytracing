#include "Triangle.h"
#include <cmath>
#include <bitset>

std::optional<IntersectionTrace> Triangle::intersect(const Ray& r) const {
	// https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/ray-triangle-intersection-geometric-solution

	const Vec3 N = leg<0,1>().cross(leg<0,2>());	// not normalized!
	//double area2 = N.len();
	
	const double NdotRd = r.direction().dot(N);
	if (std::abs(NdotRd) < constants::EPS)
		return {};	// Parallel
	const double d = N.dot(vertices_[0]);
	const double t = (N.dot(r.origin()) + d) / NdotRd;
	if (t < constants::EPS) return {};	// Reyorigin behind triangle
	const Vec3 hitPoint = r.positionAlong(t);
	// check for each leg if hitpoint is on the left. Vertices are expected to be in mathmatical order / against the clock
	std::bitset<3> isRight(3);
	const Vec3 pv0 = hitPoint - vertices_[0];
	const Vec3 pv1 = hitPoint - vertices_[1];
	const Vec3 pv2 = hitPoint - vertices_[2];
	isRight.set(0, N.dot(leg<0, 1>().cross(pv0)) < 0);
	isRight.set(1, N.dot(leg<1, 2>().cross(pv1)) < 0);
	isRight.set(2, N.dot(leg<2, 0>().cross(pv2)) < 0);
	if (isRight.all()) return {};
	return IntersectionTrace(false, normal_, hitPoint, t, material());
}
